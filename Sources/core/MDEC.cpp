/**
 * @file      MDEC.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation of PSX MDEC
 *
 * @version   0.1
 *
 * @date      26. 10. 2023, 15:43 (created)
 *
 * @section   TODO: replace with actual documentation
 * TODO: documentation text
 *
 * @section License
 * This file is part of the TODO: project \n
 *
 * Copyright (C) 2023
 *
 * This file is part of TODO: project. TODO: project is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TODO: project is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TODO: project. If not, see http://www.gnu.org/licenses/.
 */

#include "MDEC.hpp"
#include "Bus.hpp"
#include "Utils.hpp"

namespace PSX
{
    void MDEC::execute(u32 num_steps)
    {
        MARK_UNUSED(num_steps);
        TODO();
    }

    u32 MDEC::read(u32 address)
    {
        if(in_range<u32>(address, 0, 3))
        {
            return read_data_or_response();
        }
        if(in_range<u32>(address, 4, 7))
        {
            return read_status();
        }

        UNREACHABLE();
    }

    void MDEC::write(u32 address, u32 value)
    {
        if(in_range<u32>(address, 0, 3))
        {
            write_command_or_parameter(value); return;
        }
        if(in_range<u32>(address, 4, 7))
        {
            write_control(value); return;
        }

        UNREACHABLE();
    }

    void MDEC::reset()
    {
        m_status.raw = 0;
        m_status.current_block = 4;
        m_status.data_out_fifo_empty = 1;

        m_current_instruction.raw = 0;
        m_command_num_arguments = 0;
        m_current_command_argument_index = 0;
        m_quantization_table_selector = 0;

        m_input_fifo.clear();
        m_output_fifo.clear();
        std::fill(m_idct_table.begin(), m_idct_table.end(), 0);
        std::fill(m_luma_quantization_table.begin(), m_luma_quantization_table.end(), 0);
        std::fill(m_chroma_quantization_table.begin(), m_chroma_quantization_table.end(), 0);
    }

    /**
     * @brief directly execute the instruction
     */
    void MDEC::execute(const MDECInstruction& ins)
    {
        m_current_command_argument_index = 0;

        switch(ins.opcode)
        {
            case 1: // Decode Macroblock
            {
                m_status.data_output_bit15  = ins.data_output_bit15;
                m_status.data_output_signed = ins.data_output_signed;
                m_status.data_output_depth  = ins.data_output_depth;
                m_status.command_busy       = 1;
                m_command_num_arguments     = ins.num_arguments;

                m_input_fifo.clear();
                m_output_fifo.clear();
            } break;

            case 2: // Set Quantization Tables
            {
                m_status.command_busy = 1;
                m_quantization_table_selector = ins.color;
                // set luma only
                if(!m_quantization_table_selector)
                {
                    m_command_num_arguments = MacroblockSize / sizeof(u32);
                }
                // set both luma and chroma
                else
                {
                    m_command_num_arguments = MacroblockSize * 2 / sizeof(u32);
                }
            } break;

            case 3: // Set Inverse Discrete Cosine Transformation Table
            {
                m_status.command_busy = 1;
                m_command_num_arguments = MacroblockSize / 2;
            } break;

            default:
            {
                UNREACHABLE();
            } break;
        }
    }

    /**
     * @brief modify the control register 
     */
    void MDEC::write_control(u32 value)
    {
        m_control.raw = value;

        if(m_control.reset)
        {
            reset();
        }

        m_status.data_in_request  = m_control.enable_data_in_request;
        m_status.data_out_request = m_control.enable_data_out_request; 
    }
    
    /**
     * @brief execute new command or specify parameter for a command 
     */
    void MDEC::write_command_or_parameter(u32 value)
    {
        // execute new command
        if(m_command_num_arguments == 0)
        {
            m_current_instruction.raw = value;
            execute(m_current_instruction);
            m_status.parameter_remaining_minus_one = m_command_num_arguments - 1;
        }
        // write new parameter
        else
        {
            switch(m_current_instruction.opcode)
            {
                case 1: // Decode Macroblock
                {
                    m_input_fifo.push_back((value >>  0) & 0xFFFF);
                    m_input_fifo.push_back((value >> 16) & 0xFFFF);
                
                    if(m_command_num_arguments == 1)
                    {
                        TODO(); // decode macroblock
                    }
                } break;

                case 2: // Set Quantization Tables
                {
                    if(in_range<u32>(m_current_command_argument_index, 0, MacroblockSize / 4 - 1))
                    {
                        m_luma_quantization_table[m_current_command_argument_index * 4 + 0] = (value >>  0) & 0xFF;
                        m_luma_quantization_table[m_current_command_argument_index * 4 + 1] = (value >>  8) & 0xFF;
                        m_luma_quantization_table[m_current_command_argument_index * 4 + 2] = (value >> 16) & 0xFF;
                        m_luma_quantization_table[m_current_command_argument_index * 4 + 3] = (value >> 24) & 0xFF;
                    }
                    else if(in_range<u32>(m_current_command_argument_index, MacroblockSize / 4, MacroblockSize * 2 / 4 - 1))
                    {
                        m_chroma_quantization_table[(m_current_command_argument_index - MacroblockSize / 4) * 4 + 0] = (value >>  0) & 0xFF;
                        m_chroma_quantization_table[(m_current_command_argument_index - MacroblockSize / 4) * 4 + 1] = (value >>  8) & 0xFF;
                        m_chroma_quantization_table[(m_current_command_argument_index - MacroblockSize / 4) * 4 + 2] = (value >> 16) & 0xFF;
                        m_chroma_quantization_table[(m_current_command_argument_index - MacroblockSize / 4) * 4 + 3] = (value >> 24) & 0xFF;
                    }
                    else
                    {
                        UNREACHABLE();
                    }
                } break;

                case 3: // Set Inverse Discrete Cosine Transformation Table
                {
                    m_idct_table[m_current_command_argument_index * 2 + 0] = (value >>  0) & 0xFFFF;
                    m_idct_table[m_current_command_argument_index * 2 + 1] = (value >> 16) & 0xFFFF;
                } break;

                default:
                {
                    UNREACHABLE();
                } break;
            }

            m_current_command_argument_index += 1;
            m_status.parameter_remaining_minus_one = (--m_command_num_arguments) - 1;
        }
    }

    /**
     * @brief update and obtain the status register 
     */
    u32 MDEC::read_status()
    {
        m_status.data_out_fifo_empty = m_output_fifo.empty();
        m_status.data_in_fifo_full   = !m_input_fifo.empty();
        m_status.command_busy        = !m_output_fifo.empty();
        return m_status.raw;
    }

    /**
     * @brief collect the decoded output 
     */
    u32 MDEC::read_data_or_response()
    {
        TODO();
    }
}