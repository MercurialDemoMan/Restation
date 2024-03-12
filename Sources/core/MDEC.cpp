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
#include "GPUTypes.hpp"
#include "Bus.hpp"
#include "Utils.hpp"
#include <algorithm>

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
        m_status.raw                 = 0;
        m_status.current_block       = 4;
        m_status.data_out_fifo_empty = 1;

        m_current_instruction.raw        = 0;
        m_command_num_arguments          = 0;
        m_current_command_argument_index = 0;
        m_quantization_table_selector    = 0;
        m_input_fifo_cursor              = 0;
        m_output_fifo_cursor             = 0;
        m_read24bit_value_cursor         = 0;

        m_input_fifo.clear();
        m_output_fifo.clear();
        std::fill(m_idct_table.begin(), m_idct_table.end(), 0);
        std::fill(m_luma_quantization_table.begin(), m_luma_quantization_table.end(), 0);
        std::fill(m_chroma_quantization_table.begin(), m_chroma_quantization_table.end(), 0);
        std::fill(m_y_block[0].begin(), m_y_block[0].end(), 0);
        std::fill(m_y_block[1].begin(), m_y_block[1].end(), 0);
        std::fill(m_y_block[2].begin(), m_y_block[2].end(), 0);
        std::fill(m_y_block[3].begin(), m_y_block[3].end(), 0);
        std::fill(m_cb_block.begin(), m_cb_block.end(), 0);
        std::fill(m_cr_block.begin(), m_cr_block.end(), 0);
    }

    /**
     * @brief reset the MDEC from a command
     */
    void MDEC::soft_reset()
    {
        m_current_instruction.raw    = 0;
        m_status.raw                 = 0;
        m_status.current_block       = 4;
        m_status.data_out_fifo_empty = 1;
        m_output_fifo_cursor         = 0;

        LOG(fmt::format("soft reset {}", m_command_num_arguments));

        if(m_command_num_arguments != 0)
        {
            ABORT_WITH_MESSAGE(fmt::format("??? {}", m_command_num_arguments));
        }
    }

    /**
     * @brief used by DMAChannelMDECIN to check if input data fifo is ready for writing
     */
    bool MDEC::is_input_fifo_ready() const
    {
        return m_status.data_in_request &&
               m_output_fifo.empty();
    }

    /**
     * @brief used by DMAChannelMDECOUT to check if output data fifo is ready for reading
     */
    bool MDEC::is_output_fifo_ready() const
    {
        return m_status.data_out_request &&
               !m_output_fifo.empty();
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
                m_read24bit_value_cursor    = 0;

                m_input_fifo.clear();
                m_output_fifo.clear();
                m_output_fifo_cursor = 0;
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
                m_command_num_arguments = MacroblockSize / sizeof(u16);
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
            soft_reset();
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
                        decode_all_macroblocks_from_input_fifo();
                    }
                } break;

                case 2: // Set Quantization Tables
                {
                    if(in_range<u32>(m_current_command_argument_index, 0, MacroblockSize / sizeof(u32) - 1))
                    {
                        m_luma_quantization_table[m_current_command_argument_index * sizeof(u32) + 0] = (value >>  0) & 0xFF;
                        m_luma_quantization_table[m_current_command_argument_index * sizeof(u32) + 1] = (value >>  8) & 0xFF;
                        m_luma_quantization_table[m_current_command_argument_index * sizeof(u32) + 2] = (value >> 16) & 0xFF;
                        m_luma_quantization_table[m_current_command_argument_index * sizeof(u32) + 3] = (value >> 24) & 0xFF;
                    }
                    else if(in_range<u32>(m_current_command_argument_index, MacroblockSize / sizeof(u32), MacroblockSize * 2 / sizeof(u32) - 1))
                    {
                        m_chroma_quantization_table[(m_current_command_argument_index - MacroblockSize / sizeof(u32)) * sizeof(u32) + 0] = (value >>  0) & 0xFF;
                        m_chroma_quantization_table[(m_current_command_argument_index - MacroblockSize / sizeof(u32)) * sizeof(u32) + 1] = (value >>  8) & 0xFF;
                        m_chroma_quantization_table[(m_current_command_argument_index - MacroblockSize / sizeof(u32)) * sizeof(u32) + 2] = (value >> 16) & 0xFF;
                        m_chroma_quantization_table[(m_current_command_argument_index - MacroblockSize / sizeof(u32)) * sizeof(u32) + 3] = (value >> 24) & 0xFF;
                    }
                    else
                    {
                        UNREACHABLE();
                    }
                } break;

                case 3: // Set Inverse Discrete Cosine Transformation Table
                {
                    m_idct_table[m_current_command_argument_index * sizeof(s16) + 0] = (value >>  0) & 0xFFFF;
                    m_idct_table[m_current_command_argument_index * sizeof(s16) + 1] = (value >> 16) & 0xFFFF;
                } break;

                default:
                {
                    UNREACHABLE();
                } break;
            }

            if(m_command_num_arguments == 0)
            {
                ABORT_WITH_MESSAGE("invalid parsing of MDEC command: consumed all arguments, but didn't escape parsing");
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
        if(!m_output_fifo.empty() && m_output_fifo_cursor < m_output_fifo.size())
        {
            u32 result = 0;

            switch(m_status.data_output_depth)
            {
                case 0:
                case 1:
                {
                    TODO();
                } break;

                case 2: // 24bit
                {
                    TODO();
                } break;

                case 3: // 15bit
                {
                    auto color_low  = Color15Bit::create_from_24bit(m_output_fifo[m_output_fifo_cursor++]);
                    auto color_high = Color15Bit::create_from_24bit(m_output_fifo[m_output_fifo_cursor++]);

                    result = (((m_status.data_output_bit15 << 15) | color_low.raw)  <<  0) |
                             (((m_status.data_output_bit15 << 15) | color_high.raw) << 16);
                } break;

                default:
                {
                    UNREACHABLE();
                } break;
            }

            if(m_input_fifo_cursor >= m_output_fifo.size())
            {
                m_output_fifo.clear();
            }

            return result;
        }
        else
        {
            return 0;
        }
    }

    /**
     * @brief consumes input and decodes macroblocks from the input fifo 
     */
    void MDEC::decode_all_macroblocks_from_input_fifo()
    {
        m_input_fifo_cursor = 0;

        while(m_input_fifo_cursor < m_input_fifo.size())
        {
            // decode macroblock from input fifo based on the offset m_input_fifo_cursor
            std::vector<u32> decoded_block = decode_macroblock();

            // append the result to output
            std::copy(decoded_block.begin(), decoded_block.end(), std::back_inserter(m_output_fifo));
        }

        m_input_fifo.clear();
    }

    /**
     * @brief decodes 1 macroblock from input fifo (offset is specified by the m_input_fifo_cursor) 
     */
    std::vector<u32> MDEC::decode_macroblock()
    {
        std::vector<u32> result;

        switch(m_status.data_output_depth)
        {
            case 0: // 4bit
            case 1: // 8bit
            {
                TODO(); 
            } break;

            case 2: // 24bit
            case 3: // 15bit
            {
                if(!decode_block_with_quantization_table(m_cr_block, m_chroma_quantization_table)) return {};
                if(!decode_block_with_quantization_table(m_cb_block, m_chroma_quantization_table)) return {};
                if(!decode_block_with_quantization_table(m_y_block[0], m_luma_quantization_table)) return {};
                if(!decode_block_with_quantization_table(m_y_block[1], m_luma_quantization_table)) return {};
                if(!decode_block_with_quantization_table(m_y_block[2], m_luma_quantization_table)) return {};
                if(!decode_block_with_quantization_table(m_y_block[3], m_luma_quantization_table)) return {};
                result = convert_macroblock_from_ycbcr_to_rgb();
            } break;
        }

        return result;
    }

    /**
     * @brief decode 1 component of a macroblock based on a specific quantization table 
     */
    bool MDEC::decode_block_with_quantization_table
    (
        std::array<s16, MacroblockSize>& output_block, 
        const std::array<u8, MacroblockSize>& quantization_table)
    {
        static constexpr const u16 SkipMarker = 0xFE00;

        static constexpr const std::array<u8, 64> ZagZig = 
        {
            0,  1,  8,  16, 9,  2,  3,  10, 
            17, 24, 32, 25, 18, 11, 4,  5, 
            12, 19, 26, 33, 40, 48, 41, 34, 
            27, 20, 13, 6,  7,  14, 21, 28, 
            35, 42, 49, 56, 57, 50, 43, 36, 
            29, 22, 15, 23, 30, 37, 44, 51, 
            58, 59, 52, 45, 38, 31, 39, 46, 
            53, 60, 61, 54, 47, 55, 62, 63, 
        };

        output_block.fill(0);
        
        // skip padding using marker
        for(; m_input_fifo_cursor < m_input_fifo.size() && m_input_fifo[m_input_fifo_cursor] == SkipMarker; m_input_fifo_cursor++)
        {

        }

        if(m_input_fifo_cursor >= m_input_fifo.size())
        {
            LOG_WARNING("reached the end of the block while decoding dct");
            return false;
        }

        // extract dct component
        auto dct = m_input_fifo[m_input_fifo_cursor++];
        u16 q_scale = (dct >> 10) & 0x3F;
        s16 current = extend_sign<s16, 10>(dct & 0x03FF);
        s16 value   = current * quantization_table[0];

        // decode rle components
        for(u32 i = 0; i < MacroblockSize; )
        {
            if(q_scale == 0)
            {
                value = current * 2;
            }

            value = std::clamp<s16>(value, -0x0400, 0x03FF);
            if(q_scale > 0)
            {
                output_block[ZagZig[i]] = value;
            }
            if(q_scale == 0)
            {
                output_block[i] = value;
            }

            if(m_input_fifo_cursor >= m_input_fifo.size())
            {
                LOG_WARNING("reached the end of the block while decoding dct");
                return false;
            }

            auto rle = m_input_fifo[m_input_fifo_cursor++];

            i = i + ((rle >> 10) & 0x3F) + 1;

            current = extend_sign<s16, 10>(rle & 0x03FF);

            value = (current * quantization_table[i] * q_scale + 4) / 8;
        }

        do_idct(output_block);

        return true;
    }

    /**
     * @brief perform inverse discrete cosine transform to convert from frequencies to actual color 
     */
    void MDEC::do_idct(std::array<s16, MacroblockSize>& block)
    {
        std::array<s64, MacroblockSize> result;
        result.fill(0);

        for(u32 x = 0; x < 8; x++)
        {
            for(u32 y = 0; y < 8; y++)
            {
                s32 sum = 0;

                for(u32 z = 0; z < 8; z++)
                {
                    sum += block[y + z * 8] * (m_idct_table[x + z * 8] / 8);
                }

                result[x + y * 8] = (sum + 0x0FFF) / 0x2000;
            }
        }

        for(u32 x = 0; x < 8; x++)
        {
            for(u32 y = 0; y < 8; y++)
            {
                s32 sum = 0;

                for(u32 z = 0; z < 8; z++)
                {
                    sum += result[y + z * 8] * (m_idct_table[x + z * 8] / 8);
                }

                block[x + y * 8] = (sum + 0x0FFF) / 0x2000;
            }
        }
    }

    /**
     * @brief convert a decoded macroblock from yuv colorspace to rgb colorspace 
     */
    std::vector<u32> MDEC::convert_macroblock_from_ycbcr_to_rgb()
    {
        std::vector<u32> result(16 * 16);
        
        auto yuv_to_rgb = [&](u32 x_offset, u32 y_offset)
        {
            for(u32 y = 0; y < 8; y++)
            {
                for(u32 x = 0; x < 8; x++)
                {
                    u32 y_block_offset = (y_offset / 8) * 2 + (x_offset / 8);
                    s32 Y  = m_y_block[y_block_offset][x + y * 8];
                    s32 Cr = m_cr_block[((x + x_offset) / 2) + ((y + y_offset) / 2) * 8];
                    s32 Cb = m_cb_block[((x + x_offset) / 2) + ((y + y_offset) / 2) * 8];

                    s32 R = (1.402 * Cr);
                    s32 G = (-0.3437 * Cb) + (-0.7143 * Cr);
                    s32 B = (1.772 * Cb);
                    u8  Rbyte = std::clamp(Y + R, -128, 127) + 128;
                    u8  Gbyte = std::clamp(Y + G, -128, 127) + 128;
                    u8  Bbyte = std::clamp(Y + B, -128, 127) + 128;
                    result[(x + x_offset) + (y + y_offset) * 16] = (Bbyte << 16) | (Gbyte << 8) | (Rbyte << 0);
                }
            }
        };

        yuv_to_rgb(0, 0);
        yuv_to_rgb(0, 8);
        yuv_to_rgb(8, 0);
        yuv_to_rgb(8, 8);


        return result;
    }
}