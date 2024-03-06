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
            TODO();
        }
        if(in_range<u32>(address, 4, 7))
        {
            TODO();
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
    }

    /**
     * @brief directly execute the instruction
     */
    void MDEC::execute(const MDECInstruction&)
    {
        TODO();
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
        if(m_command_num_arguments == 0)
        {
            m_current_instruction.raw = value;
            execute(m_current_instruction);
            m_status.parameter_remaining_minus_one = m_command_num_arguments - 1;
        }
        else
        {
            switch(m_current_instruction.opcode)
            {
                case 1: // Decode Macroblock
                {
                    TODO();
                } break;

                case 2: // Set Quantization Table
                {
                    TODO();
                } break;

                case 3: // Set Inverse Discrete Cosine Transformation Table
                {
                    TODO();
                } break;

                default:
                {
                    UNREACHABLE();
                } break;
            }

            m_status.parameter_remaining_minus_one = (--m_command_num_arguments) - 1;
        }
    }
}