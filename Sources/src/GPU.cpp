/**
 * @file      GPU.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation of the PSX Graphics processing unit
 *
 * @version   0.1
 *
 * @date      26. 10. 2023, 16:20 (created)
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

#include "GPU.hpp"
#include "Bus.hpp"
#include "GPUTypes.hpp"
#include "InterruptController.hpp"
#include "Utils.hpp"

namespace PSX
{
    void GPU::execute(u32 num_steps)
    {
        m_meta_cycles += num_steps;
        
        u32 line_cycles = m_display_mode.video_mode ? PALScanlineCycles : NTSCScanlineCycles;
        u32 num_lines   = m_display_mode.video_mode ? PALScanlines : NTSCScanlines;

        m_meta_lines  += m_meta_cycles / line_cycles;
        m_meta_cycles %= line_cycles;

        // did we render whole frame?
        if(m_meta_lines >= NTSCScanlines)
        {
            m_meta_lines %= NTSCScanlines;
            m_is_line_odd = false;
            m_rendered_new_frame = true;
        }
        // we are still rendering
        else
        {
            if(m_display_mode.vertical_interlace_enabled)
            {
                m_is_line_odd = (m_meta_lines % 2);
            }
            m_rendered_new_frame = false;
        }
    }

    u32 GPU::read(u32 address)
    {
        switch(address)
        {
            case 0 ... 3:
            {
                if(m_read_mode == 0)
                {
                    return m_read_register;
                }
                else
                {
                    return read_vram();
                }
            }
            
            case 4 ... 7:
            {
                return read_stat();
            }
        }

        UNREACHABLE();
    }

    void GPU::write(u32 address, u32 value)
    {
        switch(address)
        {
            case 0 ... 3:
            {
                execute_gp0_command(value);
            }

            case 4 ... 7:
            {
                execute_gp1_command(value);
            }
        }

        UNREACHABLE();
    }

    void GPU::reset()
    {
        m_draw_mode.raw              = 0;
        m_texture_window_setting.raw = 0;
        m_mask_bit_setting.raw       = 0;
        m_display_mode.raw           = 0;
        m_read_mode                  = 0;
        m_read_register              = 0;
        m_dma_direction              = 0;
        m_dma_start_x                = 0;
        m_dma_start_y                = 0;
        m_dma_end_x                  = 0;
        m_dma_end_y                  = 0;
        m_dma_current_x              = 0;
        m_dma_current_y              = 0;
        m_display_area_start_x       = 0;
        m_display_area_start_y       = 0;
        m_display_range_x_1          = 0x200;
        m_display_range_x_2          = 0x200 + 256 * 10;
        m_display_range_y_1          = 0x010;
        m_display_range_y_2          = 0x010 + 240;
        m_drawing_area_top           = 0;
        m_drawing_area_left          = 0;
        m_drawing_area_right         = 0;
        m_drawing_area_bottom        = 0;
        m_drawing_offset_x           = 0;
        m_drawing_offset_y           = 0;
        m_new_texture_disable        = false;
        m_display_disable            = true;
        m_interrupt_request          = false;
        m_ready_to_receive_dma_block = 1;
        m_is_line_odd                = false;
        m_current_command            = GPUCommand::Nop;
        m_command_num_arguments      = 0;
        m_meta_cycles                = 0;
    }

    /**
     * @brief accumulate information from registers
     */
    u32 GPU::read_stat()
    {
        u32 data_request = 0;

        switch(m_dma_direction)
        {
            case 0: { data_request = 0; break; }
            case 1: { data_request = m_command_fifo.size() != GPUFIFOMaxSize; break; }
            case 2: { data_request = m_ready_to_receive_dma_block; break; }
            case 3: { data_request = m_read_mode == 1; break; }
        }

        u32 result  = m_draw_mode.raw & 0x7FF;
        result     |= m_mask_bit_setting.set_mask_while_drawing << 11;
        result     |= m_mask_bit_setting.check_mask_before_draw << 12;
        result     |= 1 << 13; // TODO: (or, always 1 when GP1(08h).5=0)
        result     |= m_display_mode.reverse_flag << 14;
        result     |= m_draw_mode.texture_page_y_base_2 << 15;
        result     |= m_display_mode.horizontal_resolution_2 << 16;
        result     |= m_display_mode.horizontal_resolution_1 << 17;
        result     |= m_display_mode.vertical_resolution << 19;
        result     |= m_display_mode.video_mode << 20;
        result     |= m_display_mode.display_area_color_depth << 21;
        result     |= m_display_mode.vertical_interlace_enabled << 22;
        result     |= m_display_disable << 23;
        result     |= m_interrupt_request << 24;
        result     |= data_request << 25;
        result     |= (m_current_command == GPUCommand::Nop) << 26;
        result     |= (m_read_mode == 1) << 27; // ready to send vram to cpu
        result     |= m_ready_to_receive_dma_block << 28;
        result     |= m_dma_direction << 29;
        result     |= m_is_line_odd << 31;
        return result;
    }

    /**
     * @brief read from vram 
     */
    u32 GPU::read_vram()
    {
        u16 low  = m_vram[(VRamHeight) * (m_dma_current_y % VRamHeight) + (m_dma_current_x % VRamWidth)];

        m_dma_current_x++;
        if(m_dma_current_x >= m_dma_end_x)
        {
            m_dma_current_x = m_dma_start_x;
            m_dma_current_y++;
            if(m_dma_current_y >= m_dma_end_y)
            {
                m_read_mode = 0; // switch back to register read
            }
        }

        u16 high = m_vram[(VRamHeight) * (m_dma_current_y % VRamHeight) + (m_dma_current_x % VRamWidth)];

        m_dma_current_x++;
        if(m_dma_current_x >= m_dma_end_x)
        {
            m_dma_current_x = m_dma_start_x;
            m_dma_current_y++;
            if(m_dma_current_y >= m_dma_end_y)
            {
                m_read_mode = 0; // switch back to register read
            }
        }

        return (high << 16) | low;
    }

    /**
     * @brief 
     */
    void GPU::execute_gp0_command(u32 value)
    {
        // set initial command if no command is currently being processed
        if(m_current_command == GPUCommand::Nop)
        {
            m_command_fifo.clear();
            m_command_fifo.push_back(value);

            auto instruction = GPUGP0Instruction(value >> 24);

            switch(instruction)
            {
                case GPUGP0Instruction::Nop:
                {
                    m_command_num_arguments = 1;
                    break;
                }

                case GPUGP0Instruction::InvalidateClutCache:
                {
                    TODO();
                    m_command_num_arguments = 1;
                    break;
                }

                case GPUGP0Instruction::FillVRam:
                {
                    m_current_command       = GPUCommand::VRamFill;
                    m_command_num_arguments = 3;
                    break;
                }

                case GPUGP0Instruction::InterruptRequest:
                {
                    m_interrupt_request = true;
                    m_interrupt_controller->trigger_interrupt(Interrupt::GPU);
                    m_command_num_arguments = 1;
                    break;
                }

                case GPUGP0Instruction::PolygonRenderStart ... GPUGP0Instruction::PolygonRenderEnd:
                {
                    m_current_command = GPUCommand::PolygonRender;
                    m_command_num_arguments = PolygonRenderCommand(value).num_arguments();
                    break;
                }

                case GPUGP0Instruction::LineRenderStart ... GPUGP0Instruction::LineRenderEnd:
                {
                    m_current_command = GPUCommand::LineRender;
                    m_command_num_arguments = LineRenderCommand(value).num_arguments();
                    break;
                }

                case GPUGP0Instruction::RectangleRenderStart ... GPUGP0Instruction::RectangleRenderEnd:
                {
                    m_current_command = GPUCommand::RectangleRender;
                    m_command_num_arguments = RectangleRenderCommand(value).num_arguments();
                    break;
                }

                case GPUGP0Instruction::VRamToVRamStart ... GPUGP0Instruction::VRamToVRamEnd:
                {
                    m_current_command = GPUCommand::CopyVRamToVRam;
                    m_command_num_arguments = 4;
                    break;
                }

                case GPUGP0Instruction::CPUToVRamStart ... GPUGP0Instruction::CPUToVRamEnd:
                {
                    m_current_command = GPUCommand::CopyCPUToVRam;
                    m_command_num_arguments = 3;
                    break;
                }

                case GPUGP0Instruction::VRamToCPUStart ... GPUGP0Instruction::VRamToCPUEnd:
                {
                    m_current_command = GPUCommand::CopyVRamToCPU;
                    m_command_num_arguments = 3;
                    break;
                }

                case GPUGP0Instruction::DrawModeSetting:
                {
                    m_draw_mode.raw = value;
                    m_command_num_arguments = 1;
                    TODO();
                    break;
                }

                case GPUGP0Instruction::TextureWindowSetting:
                {
                    m_texture_window_setting.raw = value;
                    m_command_num_arguments = 1;
                    break;
                }

                case GPUGP0Instruction::SetDrawingAreaTopLeft:
                {
                    m_drawing_area_top  = (value >> 10) & 0b11'1111'1111;
                    m_drawing_area_left = (value >> 0)  & 0b11'1111'1111;
                    m_command_num_arguments = 1;
                    break;
                }

                case GPUGP0Instruction::SetDrawingAreaBottomRight:
                {
                    m_drawing_area_bottom = (value >> 10) & 0b11'1111'1111;
                    m_drawing_area_right  = (value >> 0)  & 0b11'1111'1111;
                    m_command_num_arguments = 1;
                    break;
                }

                case GPUGP0Instruction::SetDrawingOffset:
                {
                    m_drawing_offset_x = extend_sign<s16, 11>(static_cast<s16>((value >>  0) & 0b111'1111'1111));
                    m_drawing_offset_y = extend_sign<s16, 11>(static_cast<s16>((value >> 11) & 0b111'1111'1111));
                    m_command_num_arguments = 1;
                    break;
                }

                case GPUGP0Instruction::MaskBitSetting:
                {
                    m_mask_bit_setting.raw = value;
                    m_command_num_arguments = 1;
                    break;
                }

                default:
                {
                    UNREACHABLE();
                    break;
                }
            }
        }
        // process command arguments
        else if(m_command_fifo.size() < m_command_num_arguments)
        {
            // the command fifo is full / reset the command
            if(m_command_fifo.size() == GPUFIFOMaxSize)
            {
                m_current_command = GPUCommand::Nop;
                return;
            }

            // end list of poly line vertices
            if(m_current_command == GPUCommand::LineRender            && 
               LineRenderCommand(m_command_fifo.front()).is_poly_line &&
               ((value & 0xF000'F000) == 0x5000'5000))
            {
                TODO();
            }
            else
            {
                // save the argument
                m_command_fifo.push_back(value);

                // we have all the arguments -> execute the drawing/copy command
                if(m_command_fifo.size() == m_command_num_arguments)
                {
                    execute_gpu_command();
                }
            }
        }
    }

    /**
     * @brief 
     */
    void GPU::execute_gp1_command(u32 command)
    {
        GPUGP1Instruction opcode   = static_cast<GPUGP1Instruction>((command >> 24) & 0b11'1111);
        u32               argument = command & 0xFF'FFFF;

        switch(opcode)
        {
            case GPUGP1Instruction::ResetGPU:
            {
                reset();
                break;
            }

            case GPUGP1Instruction::ResetCommand:
            {
                m_current_command = GPUCommand::Nop;
                break;
            }

            case GPUGP1Instruction::AcknowledgeInterrupt:
            {
                m_interrupt_request = false;
                break;
            }

            case GPUGP1Instruction::DisplayEnable:
            {
                m_display_disable = argument & 1;
                break;
            }

            case GPUGP1Instruction::DMADirection:
            {
                m_dma_direction = argument & 0b11;
                break;
            }

            case GPUGP1Instruction::StartOfDisplayArea:
            {
                m_display_area_start_x = (argument >>  0) & 0b11'1111'1111;
                m_display_area_start_y = (argument >> 10) &  0b1'1111'1111;
                break;
            }

            case GPUGP1Instruction::HorizontalDisplayRange:
            {
                m_display_range_x_1 = (argument >>  0) & 0b1111'1111'1111;
                m_display_range_x_2 = (argument >> 12) & 0b1111'1111'1111;
                break;
            }

            case GPUGP1Instruction::VerticalDisplayRange:
            {
                m_display_range_y_1 = (argument >>  0) & 0b11'1111'1111;
                m_display_range_y_2 = (argument >> 10) & 0b11'1111'1111;
                break;
            }

            case GPUGP1Instruction::DisplayMode:
            {
                m_display_mode.raw = argument;
                break;
            }

            case GPUGP1Instruction::NewTextureDisable:
            {
                m_new_texture_disable = argument & 1;
                break;
            }

            case GPUGP1Instruction::GetGPUInfoStart ... GPUGP1Instruction::GetGPUInfoEnd:
            {
                switch(argument & 0b1111)
                {
                    // NOP
                    case 0 ... 1:
                    {
                        break;
                    }

                    // Read Texture Window setting
                    case 2:
                    {
                        m_read_register = m_texture_window_setting.raw;
                        break;
                    }

                    // Read Draw area top left
                    case 3:
                    {
                        m_read_register = (m_drawing_area_top << 10) | (m_drawing_area_left);
                        break;
                    }

                    // Read Draw area bottom right
                    case 4:
                    {
                        m_read_register = (m_drawing_area_bottom << 10) | (m_drawing_area_right);
                        break;
                    }

                    // Read Draw offset
                    case 5:
                    {
                        m_read_register = ((m_drawing_offset_y & 0b111'1111'1111) << 11) |
                                          ( m_drawing_offset_x & 0b111'1111'1111);
                        break;
                    }

                    // Read GPU Type (only available on the New 208pin GPU)
                    case 7:
                    {
                        m_read_register = 2;
                        break;
                    }

                    // Return nothing
                    case 8:
                    {
                        m_read_register = 0;
                        break;
                    }
                }

                // make result of this operation be available in the read register
                m_read_mode = 0;
                break;
            }

            case GPUGP1Instruction::SpecialTextureDisable:
            {
                LOG_WARNING("GPU GP1 SpecialTextureDisable encoutered");
                break;
            }
        }
    }

    /**
     * @brief execute drawing/copy command
     */
    void GPU::execute_gpu_command()
    {
        switch(m_current_command)
        {
            case GPUCommand::Nop:
            {
                break;
            }

            case GPUCommand::VRamFill:
            {
                VRamFill();
                break;
            }

            case GPUCommand::PolygonRender:
            {
                PolygonRender();
                break;
            }

            case GPUCommand::LineRender:
            {
                LineRender();
                break;
            }

            case GPUCommand::RectangleRender:
            {
                RectangleRender();
                break;
            }

            case GPUCommand::CopyCPUToVRam:
            {
                CopyCPUToVRam();
                break;
            }

            case GPUCommand::CopyVRamToCPU:
            {
                CopyVRamToCPU();
                break;
            }

            case GPUCommand::CopyVRamToVRam:
            {
                CopyVRamToVRam();
                break;
            }
        }
    }
    /**
     * @brief Quick VRam rectangle fill GPU Command 
     */
    void GPU::VRamFill()
    {
        TODO();
    }
    /**
     * @brief Render Polygon GPU Command 
     */
    void GPU::PolygonRender()
    {
        TODO();
    }
    /**
     * @brief Render Line GPU Command 
     */
    void GPU::LineRender()
    {
        TODO();
    }
    /**
     * @brief Render Rectangle GPU Command 
     */
    void GPU::RectangleRender()
    {
        TODO();
    }
    /**
     * @brief Copy RAM to VRAM GPU Command 
     */
    void GPU::CopyCPUToVRam()
    {
        TODO();
    }
    /**
     * @brief Copy VRAM to RAM GPU Command 
     */
    void GPU::CopyVRamToCPU()
    {
        TODO();
    }
    /**
     * @brief Copy VRAM to VRAM GPU Command 
     */
    void GPU::CopyVRamToVRam()
    {
        TODO();
    }

}