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
#include "Utils.hpp"
#include "GPUTypes.hpp"
#include "InterruptController.hpp"

#include <algorithm>

#include <glm/glm.hpp>
#include <stb_image_write.h>

namespace PSX
{
    void GPU::execute(u32 num_steps)
    {
        m_meta_cycles += num_steps;
        
        u32 line_cycles = m_display_mode.video_mode ? PALScanlineCycles : NTSCScanlineCycles;
        u32 num_lines   = m_display_mode.video_mode ? PALScanlines      : NTSCScanlines;

        u32 new_lines   = m_meta_cycles / line_cycles;

        if(new_lines == 0)
            return;

        m_meta_lines  += new_lines;
        m_meta_cycles %= line_cycles;
        
        // did we render whole frame?
        if(m_meta_lines >= num_lines)
        {
            m_meta_lines %= num_lines;
            m_meta_frames++;
            m_is_line_odd = false;
            m_interrupt_controller->trigger_interrupt(Interrupt::VBlank);
            m_bus->meta_set_vblank();
        }
        // we are still rendering
        else
        {
            switch(m_display_mode.vertical_resolution)
            {
                // 240 lines
                case 0:
                {
                    m_is_line_odd = m_meta_lines & 1;
                } break;

                // 480 lines
                case 1:
                {
                    m_is_line_odd = m_meta_frames & 1;
                } break;
            }
        }
    }

    u32 GPU::read(u32 address)
    {
        if(in_range(address, 0u, 3u))
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
        if(in_range(address, 4u, 7u))
        {
            return read_stat();
        }

        UNREACHABLE();
    }

    void GPU::write(u32 address, u32 value)
    {
        if(in_range(address, 0u, 3u))
        {
            execute_gp0_command(value); return;
        }

        if(in_range(address, 4u, 7u))
        {
            execute_gp1_command(value); return;
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
        m_clut_cache_x               = {};
        m_clut_cache_y               = {};
        m_clut_cache_depth           = 0;
        m_command_num_arguments      = 0;
        m_meta_cycles                = 0;
        m_meta_lines                 = 0;
        m_meta_frames                = 0;
        m_meta_resolution            = RenderTarget::VRam1x;
        m_vram.fill(0);
        m_vram_hires.fill(0);
        m_clut_cache.fill(0);
    }

    void GPU::serialize(std::shared_ptr<SaveState>& save_state)
    {
        save_state->serialize_from(m_draw_mode.raw);
        save_state->serialize_from(m_texture_window_setting.raw);
        save_state->serialize_from(m_drawing_area_top);
        save_state->serialize_from(m_drawing_area_left);
        save_state->serialize_from(m_drawing_area_right);
        save_state->serialize_from(m_drawing_area_bottom);
        save_state->serialize_from(m_drawing_offset_x);
        save_state->serialize_from(m_drawing_offset_y);
        save_state->serialize_from(m_mask_bit_setting.raw);
        save_state->serialize_from(m_interrupt_request);
        save_state->serialize_from(m_display_disable);
        save_state->serialize_from(m_dma_direction);
        save_state->serialize_from(m_display_area_start_x);
        save_state->serialize_from(m_display_area_start_y);
        save_state->serialize_from(m_display_range_x_1);
        save_state->serialize_from(m_display_range_x_2);
        save_state->serialize_from(m_display_range_y_1);
        save_state->serialize_from(m_display_range_y_2);
        save_state->serialize_from(m_display_mode.raw);
        save_state->serialize_from(m_new_texture_disable);
        save_state->serialize_from(m_read_mode);
        save_state->serialize_from(m_read_register);
        save_state->serialize_from(m_dma_start_x);
        save_state->serialize_from(m_dma_start_y);
        save_state->serialize_from(m_dma_end_x);
        save_state->serialize_from(m_dma_end_y);
        save_state->serialize_from(m_dma_current_x);
        save_state->serialize_from(m_dma_current_y);
        save_state->serialize_from(m_ready_to_receive_dma_block);
        save_state->serialize_from(m_is_line_odd);
        save_state->serialize_from(m_current_command);
        save_state->serialize_from(m_command_fifo);
        save_state->serialize_from(m_command_num_arguments);
        save_state->serialize_from(m_clut_cache_x);
        save_state->serialize_from(m_clut_cache_y);
        save_state->serialize_from(m_clut_cache_depth);
        save_state->serialize_from(m_meta_cycles);
        save_state->serialize_from(m_meta_lines);
        save_state->serialize_from(m_meta_frames);
        save_state->serialize_from(m_vram);
        save_state->serialize_from(m_clut_cache);
    }

    void GPU::deserialize(std::shared_ptr<SaveState>& save_state)
    {
        save_state->deserialize_to(m_draw_mode.raw);
        save_state->deserialize_to(m_texture_window_setting.raw);
        save_state->deserialize_to(m_drawing_area_top);
        save_state->deserialize_to(m_drawing_area_left);
        save_state->deserialize_to(m_drawing_area_right);
        save_state->deserialize_to(m_drawing_area_bottom);
        save_state->deserialize_to(m_drawing_offset_x);
        save_state->deserialize_to(m_drawing_offset_y);
        save_state->deserialize_to(m_mask_bit_setting.raw);
        save_state->deserialize_to(m_interrupt_request);
        save_state->deserialize_to(m_display_disable);
        save_state->deserialize_to(m_dma_direction);
        save_state->deserialize_to(m_display_area_start_x);
        save_state->deserialize_to(m_display_area_start_y);
        save_state->deserialize_to(m_display_range_x_1);
        save_state->deserialize_to(m_display_range_x_2);
        save_state->deserialize_to(m_display_range_y_1);
        save_state->deserialize_to(m_display_range_y_2);
        save_state->deserialize_to(m_display_mode.raw);
        save_state->deserialize_to(m_new_texture_disable);
        save_state->deserialize_to(m_read_mode);
        save_state->deserialize_to(m_read_register);
        save_state->deserialize_to(m_dma_start_x);
        save_state->deserialize_to(m_dma_start_y);
        save_state->deserialize_to(m_dma_end_x);
        save_state->deserialize_to(m_dma_end_y);
        save_state->deserialize_to(m_dma_current_x);
        save_state->deserialize_to(m_dma_current_y);
        save_state->deserialize_to(m_ready_to_receive_dma_block);
        save_state->deserialize_to(m_is_line_odd);
        save_state->deserialize_to(m_current_command);
        save_state->deserialize_to(m_command_fifo);
        save_state->deserialize_to(m_command_num_arguments);
        save_state->deserialize_to(m_clut_cache_x);
        save_state->deserialize_to(m_clut_cache_y);
        save_state->deserialize_to(m_clut_cache_depth);
        save_state->deserialize_to(m_meta_cycles);
        save_state->deserialize_to(m_meta_lines);
        save_state->deserialize_to(m_meta_frames);
        save_state->deserialize_to(m_vram);
        save_state->deserialize_to(m_clut_cache);
    }

    /**
     * @brief soft reset made by GP1 command 
     */
    void GPU::soft_reset()
    {
        m_draw_mode.raw              = 0;
        m_texture_window_setting.raw = 0;
        m_mask_bit_setting.raw       = 0;
        m_display_mode.raw           = 0;
        m_dma_direction              = 0;
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
        m_display_disable            = true;
        m_interrupt_request          = false;
        m_clut_cache_x               = {};
        m_clut_cache_y               = {};
    }

    /**
     * @brief accumulate information from registers
     */
    u32 GPU::read_stat()
    {
        u32 data_request = 0;

        switch(m_dma_direction)
        {
            case 0: 
            { 
                data_request = 0;
            } break;
            case 1: 
            { 
                data_request = m_command_fifo.size() != GPUFIFOMaxSize; 
            } break; 
            case 2: 
            { 
                data_request = m_ready_to_receive_dma_block; 
            } break; 
            case 3: 
            { 
                data_request = m_read_mode == 1; 
            } break; 
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
     * @brief read from vram into the GPUREAD
     */
    u32 GPU::read_vram()
    {
        u16 low = vram_read(m_dma_current_x % VRamWidth, m_dma_current_y % VRamHeight);

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

        u16 high = vram_read(m_dma_current_x % VRamWidth, m_dma_current_y % VRamHeight);

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
     * @brief execute GP0 command
     */
    void GPU::execute_gp0_command(u32 value)
    {
        // set initial command if no command is currently being processed
        if(m_current_command == GPUCommand::Nop)
        {
            m_command_fifo.clear();
            m_command_fifo.push(value);

            u32  instruction_raw = value >> 24;
            auto instruction = GPUGP0Instruction(instruction_raw);

            if(instruction == GPUGP0Instruction::Nop)
            {
                m_command_num_arguments = 1;
            }
            else if(instruction == GPUGP0Instruction::InvalidateClutCache)
            {
                m_clut_cache_x = {};
                m_clut_cache_y = {};
                m_command_num_arguments = 1;
            }
            else if(instruction == GPUGP0Instruction::FillVRam)
            {
                m_current_command       = GPUCommand::VRamFill;
                m_command_num_arguments = 3;
            }
            else if(instruction == GPUGP0Instruction::InterruptRequest)
            {
                m_interrupt_request = true;
                m_interrupt_controller->trigger_interrupt(Interrupt::GPU);
                m_command_num_arguments = 1;
            }
            else if(in_range(instruction, GPUGP0Instruction::PolygonRenderStart, GPUGP0Instruction::PolygonRenderEnd))
            {
                m_current_command = GPUCommand::PolygonRender;
                m_command_num_arguments = PolygonRenderCommand(instruction_raw).num_arguments();
            }
            else if(in_range(instruction, GPUGP0Instruction::LineRenderStart, GPUGP0Instruction::LineRenderEnd))
            {
                m_current_command = GPUCommand::LineRender;
                m_command_num_arguments = LineRenderCommand(instruction_raw).num_arguments();
            }
            else if(in_range(instruction, GPUGP0Instruction::RectangleRenderStart, GPUGP0Instruction::RectangleRenderEnd))
            {
                m_current_command = GPUCommand::RectangleRender;
                m_command_num_arguments = RectangleRenderCommand(instruction_raw).num_arguments();
            }
            else if(in_range(instruction, GPUGP0Instruction::VRamToVRamStart, GPUGP0Instruction::VRamToVRamEnd))
            {
                m_current_command = GPUCommand::CopyVRamToVRam;
                m_command_num_arguments = 4;
            }
            else if(in_range(instruction, GPUGP0Instruction::CPUToVRamStart, GPUGP0Instruction::CPUToVRamEnd))
            {
                m_current_command = GPUCommand::CopyCPUToVRamParsingPhase;
                m_command_num_arguments = 3;
            }
            else if(in_range(instruction, GPUGP0Instruction::VRamToCPUStart, GPUGP0Instruction::VRamToCPUEnd))
            {
                m_current_command = GPUCommand::CopyVRamToCPU;
                m_command_num_arguments = 3;
            }
            else if(instruction == GPUGP0Instruction::DrawModeSetting)
            {
                m_draw_mode.raw = value;
                m_command_num_arguments = 1;
            }
            else if(instruction == GPUGP0Instruction::TextureWindowSetting)
            {
                m_texture_window_setting.raw = value;
                m_command_num_arguments = 1;
            }
            else if(instruction == GPUGP0Instruction::SetDrawingAreaTopLeft)
            {
                m_drawing_area_top  = (value >> 10) & 0b11'1111'1111;
                m_drawing_area_left = (value >> 0)  & 0b11'1111'1111;
                m_command_num_arguments = 1;
            }
            else if(instruction == GPUGP0Instruction::SetDrawingAreaBottomRight)
            {
                m_drawing_area_bottom = (value >> 10) & 0b11'1111'1111;
                m_drawing_area_right  = (value >> 0)  & 0b11'1111'1111;
                m_command_num_arguments = 1;
            }
            else if(instruction == GPUGP0Instruction::SetDrawingOffset)
            {
                m_drawing_offset_x = extend_sign<s16, 11>(static_cast<s16>((value >>  0) & 0b111'1111'1111));
                m_drawing_offset_y = extend_sign<s16, 11>(static_cast<s16>((value >> 11) & 0b111'1111'1111));
                m_command_num_arguments = 1;
            }
            else if(instruction == GPUGP0Instruction::MaskBitSetting)
            {
                m_mask_bit_setting.raw = value;
                m_command_num_arguments = 1;
            }
            else
            {
                UNREACHABLE();
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
            if(m_current_command == GPUCommand::LineRender          && 
               LineRenderCommand(m_command_fifo.at(0)).is_poly_line &&
               ((value & 0xF000'F000) == 0x5000'5000))
            {
                m_current_command = GPUCommand::Nop;
            }
            else
            {
                // save the argument
                m_command_fifo.push(value);

                // we have all the arguments -> execute the drawing/copy command
                if(m_command_fifo.size() == m_command_num_arguments)
                {
                    execute_gpu_command();
                }
            }
        }
    }

    /**
     * @brief execute GP1 command
     */
    void GPU::execute_gp1_command(u32 command)
    {
        GPUGP1Instruction opcode   = static_cast<GPUGP1Instruction>((command >> 24) & 0b11'1111);
        u32               argument = command & 0xFF'FFFF;

        if(opcode == GPUGP1Instruction::ResetGPU)
        {
            soft_reset();
        }
        else if(opcode == GPUGP1Instruction::ResetCommand)
        {
            m_current_command = GPUCommand::Nop;
        }
        else if(opcode == GPUGP1Instruction::AcknowledgeInterrupt)
        {
            m_interrupt_request = false;
        }
        else if(opcode == GPUGP1Instruction::DisplayEnable)
        {
            m_display_disable = argument & 1;
        }
        else if(opcode == GPUGP1Instruction::DMADirection)
        {
            m_dma_direction = argument & 0b11;
        }
        else if(opcode == GPUGP1Instruction::StartOfDisplayArea)
        {
            m_display_area_start_x = (argument >>  0) & 0b11'1111'1111;
            m_display_area_start_y = (argument >> 10) &  0b1'1111'1111;
        }
        else if(opcode == GPUGP1Instruction::HorizontalDisplayRange)
        {
            m_display_range_x_1 = (argument >>  0) & 0b1111'1111'1111;
            m_display_range_x_2 = (argument >> 12) & 0b1111'1111'1111;
        }
        else if(opcode == GPUGP1Instruction::VerticalDisplayRange)
        {
            m_display_range_y_1 = (argument >>  0) & 0b11'1111'1111;
            m_display_range_y_2 = (argument >> 10) & 0b11'1111'1111;
        }
        else if(opcode == GPUGP1Instruction::DisplayMode)
        {
            m_display_mode.raw = argument;
        }
        else if(opcode == GPUGP1Instruction::NewTextureDisable)
        {
            m_new_texture_disable = argument & 1;
        }
        else if(in_range(opcode, GPUGP1Instruction::GetGPUInfoStart, GPUGP1Instruction::GetGPUInfoEnd))
        {
            switch(argument & 0b1111)
            {
                // NOP
                case 0:
                case 1:
                {
                    
                } break;

                // Read Texture Window setting
                case 2:
                {
                    m_read_register = m_texture_window_setting.raw;
                } break;

                // Read Draw area top left
                case 3:
                {
                    m_read_register = (m_drawing_area_top << 10) | (m_drawing_area_left);
                } break;

                // Read Draw area bottom right
                case 4:
                {
                    m_read_register = (m_drawing_area_bottom << 10) | (m_drawing_area_right);
                } break;

                // Read Draw offset
                case 5:
                {
                    m_read_register = ((m_drawing_offset_y & 0b111'1111'1111) << 11) |
                                       (m_drawing_offset_x & 0b111'1111'1111);
                } break;

                // Read GPU Type (only available on the New 208pin GPU)
                case 7:
                {
                    m_read_register = 2;
                } break;

                // Return nothing
                case 8:
                {
                    m_read_register = 0;
                } break;
            }

            // make result of this operation be available in the read register
            m_read_mode = 0;
        }
        else if(opcode == GPUGP1Instruction::SpecialTextureDisable)
        {
            LOG_WARNING("GPU GP1 SpecialTextureDisable encoutered");
        }
        else
        {
            UNREACHABLE();
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
                return;
            }

            case GPUCommand::VRamFill:
            {
                vram_fill(); return;
            }

            case GPUCommand::PolygonRender:
            {
                polygon_render(); return;
            }

            case GPUCommand::LineRender:
            {
                line_render(); return;
            }

            case GPUCommand::RectangleRender:
            {
                rectangle_render(); return;
            }

            case GPUCommand::CopyCPUToVRamParsingPhase:
            {
                copy_cpu_to_vram_parsing_phase(); return;
            }

            case GPUCommand::CopyCPUToVRamDataPhase:
            {
                copy_cpu_to_vram_data_phase(); return;
            }

            case GPUCommand::CopyVRamToCPU:
            {
                copy_vram_to_cpu(); return;
            }

            case GPUCommand::CopyVRamToVRam:
            {
                copy_vram_to_vram(); return;
            }
        }

        UNREACHABLE();
    }

    /**
     * @brief Quick VRam rectangle fill GPU Command 
     */
    void GPU::vram_fill()
    {
        // extract arguments
        auto color  = Color24Bit(m_command_fifo.at(0) & 0x00FF'FFFF);
        u32 start_x = (m_command_fifo.at(1) >>  0) & 0xFFFF;
        u32 start_y = (m_command_fifo.at(1) >> 16) & 0xFFFF;
        u32 size_x  = (m_command_fifo.at(2) >>  0) & 0xFFFF;
        u32 size_y  = (m_command_fifo.at(2) >> 16) & 0xFFFF;

        // masking and rounding for FILL command
        start_x &= 0x3F0;
        start_y &= 0x1FF;
        size_x   = ((size_x & 0x3FF) + 0x0F) & (~0x0F);
        size_y  &= 0x1FF;

        // perform the filling
        do_vram_fill(VRamFillArguments
        {
            .start_x = start_x,
            .start_y = start_y,
            .size_x  = size_x,
            .size_y  = size_y,
            .color   = Color15Bit::create_from_24bit(color)
        }, m_meta_resolution);

        // reset command queue
        m_current_command = GPUCommand::Nop;
    }

    /**
     * @brief Perform Quick VRAM rectangle fill GPU Command
     */
    void GPU::do_vram_fill(VRamFillArguments args, RenderTarget target)
    {
        switch(target)
        {
            case RenderTarget::VRam1x: { do_vram_fill(args); } break;
            case RenderTarget::VRam2x: { do_vram_fill(args); do_vram_fill_hires(args); } break;
            default:
            {
                UNREACHABLE();
            } break;
        }
    }

    void GPU::do_vram_fill(VRamFillArguments args)
    {
        // fill vram with a color value
        // final row and column is not filled
        for(u32 y = args.start_y; y < args.start_y + args.size_y; y++)
        {
            for(u32 x = args.start_x; x < args.start_x + args.size_x; x++)
            {
                vram_write(x, y, args.color.raw);
            }
        }
    }

    void GPU::do_vram_fill_hires(VRamFillArguments args)
    {
        for(u32 y = args.start_y * VRamHiresScale; y < args.start_y * VRamHiresScale + args.size_y * VRamHiresScale; y++)
        {
            for(u32 x = args.start_x * VRamHiresScale; x < args.start_x * VRamHiresScale + args.size_x * VRamHiresScale; x++)
            {
                vram_write_hires(x, y, args.color.raw);
            }
        }
    }

    /**
     * @brief Render Polygon GPU Command 
     */
    void GPU::polygon_render()
    {
        PolygonRenderCommand command(m_command_fifo.at(0) >> 24);

        std::vector<Vertex> vertices(command.num_vertices());
        TextureInfo         texture_info;

        // parse vertices
        for(u32 i = 0, j = 1; i < command.num_vertices(); i++)
        {
            // vertex position
            vertices[i].pos_x = m_drawing_offset_x + extend_sign<s16, 11>((m_command_fifo.at(j) >>  0) & 0xFFFF);
            vertices[i].pos_y = m_drawing_offset_y + extend_sign<s16, 11>((m_command_fifo.at(j) >> 16) & 0xFFFF);
            j++;

            // vertex color
            if(i == 0 || !command.is_gouraud_shaded)
            {
                vertices[i].color = m_command_fifo.at(0) & 0x00FF'FFFF;
            }

            // vertex uv coordinates + texture info
            if(command.is_texture_mapped)
            {
                if(i == 0)
                    texture_info.palette_index = m_command_fifo.at(j);
                if(i == 1)
                    texture_info.texpage_index = m_command_fifo.at(j);

                vertices[i].uv_x = (m_command_fifo.at(j) >> 0) & 0xFF;
                vertices[i].uv_y = (m_command_fifo.at(j) >> 8) & 0xFF;
                j++;
            }

            // vertex interpolation
            if(command.is_gouraud_shaded && i < command.num_vertices() - 1)
            {
                vertices[i + 1].color = m_command_fifo.at(j) & 0x00FF'FFFF;
                j++;
            }
        }

        auto args = PolygonRenderArguments
        {
            .vertex_a = vertices[0],
            .vertex_b = vertices[1],
            .vertex_c = vertices[2],
            .color_depth = 0,
            .texpage_x   = 0,
            .texpage_y   = 0,
            .clut_x      = 0,
            .clut_y      = 0,
            .semi_transparency   = m_draw_mode.semi_transparency,
            .is_raw_texture      = command.is_raw_texture,
            .is_semi_transparent = command.is_semi_transparent,
            .is_gouraud_shaded   = command.is_gouraud_shaded
        };

        // accumulate detailed information about the texture
        if(command.is_texture_mapped)
        {
            switch((texture_info.texpage_index & 0x0180'0000) >> 23)
            {
                // 4bit
                case 0:
                {
                    args.color_depth = 1;
                } break;

                // 8bit
                case 1:
                {
                    args.color_depth = 2;
                } break;

                // 15bit
                case 2:
                // reserved
                case 3:
                {
                    args.color_depth = 3;
                } break;

                default: 
                { 
                    UNREACHABLE(); 
                } break;
            }

            args.texpage_x   = ((texture_info.texpage_index & 0x000F'0000) >> 16) * 64;
            args.texpage_y   = ((texture_info.texpage_index & 0x0010'0000) >> 20) * 256;
            args.clut_x      = ((texture_info.palette_index & 0x003F'0000) >> 16) * 16;
            args.clut_y      = ((texture_info.palette_index & 0x7FC0'0000) >> 22);
            args.semi_transparency = (texture_info.texpage_index & 0x0060'0000) >> 21;

            // update draw mode texture information texpage
            // index identically maps on the structure
            // of the GP0(0xE1) DrawMode register
            u16 new_bits = (texture_info.texpage_index >> 16) & 0b0000100111111111;
            if(!m_new_texture_disable)
            {
                new_bits &= ~(1 << 11);
            }
            m_draw_mode.raw &= ~0b0000100111111111;
            m_draw_mode.raw |= new_bits;
        }

        if(command.is_quad)
        {
            do_polygon_render(args, m_meta_resolution);
            args.vertex_a = vertices[1];
            args.vertex_b = vertices[2];
            args.vertex_c = vertices[3];
            do_polygon_render(args, m_meta_resolution);
        }
        else
        {
            do_polygon_render(args, m_meta_resolution);
        }

        // reset command queue
        m_current_command = GPUCommand::Nop;
    }

    /**
     * @brief Perform Render Polygon GPU Command
     */
    void GPU::do_polygon_render(PolygonRenderArguments args, RenderTarget target)
    {
        switch(target)
        {
            case RenderTarget::VRam1x: { do_polygon_render(args); } break;
            case RenderTarget::VRam2x: { do_polygon_render(args); do_polygon_render_hires(args); } break;
            default:
            {
                UNREACHABLE();
            } break;
        }
    }

    /**
     * @brief Depending on the GPU::VRamHiresScale constant render polygon in different resolutions
     */
    void GPU::do_polygon_render(PolygonRenderArguments args)
    {
        /**
         * @brief calculate 2D cector cross product 
         */
        auto cross = [](const glm::ivec2& a, const glm::ivec2& b) 
        -> s32
        {
            return a.x * b.y - a.y * b.x;
        };

        /**
         * @brief calculate vertex argument interpolation
         */
        auto fragment_attribute = [cross](s32 area, glm::ivec2 vertices[3], s32 biases[3], s32 atrributes[3])
        -> fixed_point
        {
            float attr_a = cross(vertices[1], vertices[2]) * atrributes[0] - biases[0];
            float attr_b = cross(vertices[2], vertices[0]) * atrributes[1] - biases[1];
            float attr_c = cross(vertices[0], vertices[1]) * atrributes[2] - biases[2];

            return fixed_point((attr_a + attr_b + attr_c) / area + 0.5f);
        };

        /**
         * @brief calculate attribute 2D offset 
         */
        auto fragment_attribute_delta = [](s32 area, glm::ivec2 vertices[3], s32 attributes[3])
        -> FragmentAttributeDelta
        {
            return
            {
                fixed_point(((vertices[1].y - vertices[2].y) * attributes[0] + 
                             (vertices[2].y - vertices[0].y) * attributes[1] + 
                             (vertices[0].y - vertices[1].y) * attributes[2]) / float(area)),
                fixed_point(((vertices[2].x - vertices[1].x) * attributes[0] + 
                             (vertices[0].x - vertices[2].x) * attributes[1] + 
                             (vertices[1].x - vertices[0].x) * attributes[2]) / float(area)),
            };
        };

        /**
         * @brief efficiently update the attributes interpolation when
         *        moving onto the next pixel in the horizontal direction 
         */
        auto update_attributes_x = [args](FragmentAttributes& attributes, const FragmentAttributesDeltas& deltas, s32 delta)
        {
            if(args.is_gouraud_shaded)
            {
                attributes.r = fixed_point(attributes.r.to_float() + deltas.r.x.to_float() * delta);
                attributes.g = fixed_point(attributes.g.to_float() + deltas.g.x.to_float() * delta);
                attributes.b = fixed_point(attributes.b.to_float() + deltas.b.x.to_float() * delta);
            }

            if(args.color_depth != 0)
            {
                attributes.u = fixed_point(attributes.u.to_float() + deltas.u.x.to_float() * delta);
                attributes.v = fixed_point(attributes.v.to_float() + deltas.v.x.to_float() * delta);
            }
        };

        /**
         * @brief efficiently update the attributes interpolation when
         *        moving onto the next pixel in the vertical direction 
         */
        auto update_attributes_y = [args](FragmentAttributes& attributes, const FragmentAttributesDeltas& deltas, s32 delta)
        {
            if(args.is_gouraud_shaded)
            {
                attributes.r = fixed_point(attributes.r.to_float() + deltas.r.y.to_float() * delta);
                attributes.g = fixed_point(attributes.g.to_float() + deltas.g.y.to_float() * delta);
                attributes.b = fixed_point(attributes.b.to_float() + deltas.b.y.to_float() * delta);
            }

            if(args.color_depth != 0)
            {
                attributes.u = fixed_point(attributes.u.to_float() + deltas.u.y.to_float() * delta);
                attributes.v = fixed_point(attributes.v.to_float() + deltas.v.y.to_float() * delta);
            }
        };

        // assure counter clock wise winding of the rasterized triangle
        {
            glm::ivec2 temp_vertex_a = { args.vertex_a.pos_x, args.vertex_a.pos_y };
            glm::ivec2 temp_vertex_b = { args.vertex_b.pos_x, args.vertex_b.pos_y };
            glm::ivec2 temp_vertex_c = { args.vertex_c.pos_x, args.vertex_c.pos_y };
            auto temp_ba = temp_vertex_b - temp_vertex_a;
            auto temp_ca = temp_vertex_c - temp_vertex_a;

            // if clock-wise windwing -> flip the triangle order
            if(cross(temp_ba, temp_ca) < 0)
            {
                std::swap(args.vertex_b, args.vertex_c);
            }
        }

        // collect vertex positions so we can use
        // "glm" to work with them more efficiently
        glm::ivec2 vertices[3] =
        {
            { args.vertex_a.pos_x, args.vertex_a.pos_y },
            { args.vertex_b.pos_x, args.vertex_b.pos_y },
            { args.vertex_c.pos_x, args.vertex_c.pos_y }
        };

        s32 area = cross(vertices[1] - vertices[0], 
                         vertices[2] - vertices[0]);

        if(area == 0)
            return;

        // calculate triangle bounds
        glm::ivec2 min = 
        {
            std::min(std::min(vertices[0].x, vertices[1].x), vertices[2].x),
            std::min(std::min(vertices[0].y, vertices[1].y), vertices[2].y),
        };

        glm::ivec2 max = 
        {
            std::max(std::max(vertices[0].x, vertices[1].x), vertices[2].x),
            std::max(std::max(vertices[0].y, vertices[1].y), vertices[2].y),
        };

        // limit triangle size
        glm::ivec2 size = max - min;

        if(size.x > 1023 || size.y > 511)
            return;

        update_clut_cache(args.color_depth, args.clut_x, args.clut_y);

        min.x = clamp_drawing_area_left(min.x);
        min.y = clamp_drawing_area_top(min.y);
        max.x = clamp_drawing_area_right(max.x);
        max.y = clamp_drawing_area_bottom(max.y);

        glm::ivec2 delta_ba
        (
            vertices[1].x - vertices[0].x, 
            vertices[0].y - vertices[1].y
        );

        glm::ivec2 delta_cb
        (
            vertices[2].x - vertices[1].x, 
            vertices[1].y - vertices[2].y
        );

        glm::ivec2 delta_ac
        (
            vertices[0].x - vertices[2].x, 
            vertices[2].y - vertices[0].y
        );

        s32 biases[3] =
        {
            (delta_cb.y < 0 || (delta_cb.y == 0 && delta_cb.x < 0)) ? -1 : 0,
            (delta_ac.y < 0 || (delta_ac.y == 0 && delta_ac.x < 0)) ? -1 : 0,
            (delta_ba.y < 0 || (delta_ba.y == 0 && delta_ba.x < 0)) ? -1 : 0
        };

        glm::ivec3 half_space_y =
        {
            cross(vertices[2] - vertices[1],
                  min         - vertices[1]) + biases[0],
            cross(vertices[0] - vertices[2],
                  min         - vertices[2]) + biases[1],
            cross(vertices[1] - vertices[0],
                  min         - vertices[0]) + biases[2],
        };

        // calculate per-fragment initial attributes and deltas
        FragmentAttributes       frag_attrs_init;
        FragmentAttributesDeltas frag_attrs_deltas;

        if(args.is_gouraud_shaded)
        {
            s32 attr_color_r[3] = { s32(args.vertex_a.color.r), s32(args.vertex_b.color.r), s32(args.vertex_c.color.r) };
            s32 attr_color_g[3] = { s32(args.vertex_a.color.g), s32(args.vertex_b.color.g), s32(args.vertex_c.color.g) };
            s32 attr_color_b[3] = { s32(args.vertex_a.color.b), s32(args.vertex_b.color.b), s32(args.vertex_c.color.b) };

            frag_attrs_init.r = fragment_attribute(area, vertices, biases, attr_color_r);
            frag_attrs_init.g = fragment_attribute(area, vertices, biases, attr_color_g);
            frag_attrs_init.b = fragment_attribute(area, vertices, biases, attr_color_b);
            frag_attrs_deltas.r = fragment_attribute_delta(area, vertices, attr_color_r);
            frag_attrs_deltas.g = fragment_attribute_delta(area, vertices, attr_color_g);
            frag_attrs_deltas.b = fragment_attribute_delta(area, vertices, attr_color_b);
        }

        if(args.color_depth != 0)
        {
            s32 u[3] = { args.vertex_a.uv_x, args.vertex_b.uv_x, args.vertex_c.uv_x };
            s32 v[3] = { args.vertex_a.uv_y, args.vertex_b.uv_y, args.vertex_c.uv_y };
            frag_attrs_init.u = fragment_attribute(area, vertices, biases, u);
            frag_attrs_init.v = fragment_attribute(area, vertices, biases, v);
            frag_attrs_deltas.u = fragment_attribute_delta(area, vertices, u);
            frag_attrs_deltas.v = fragment_attribute_delta(area, vertices, v);
        }

        update_attributes_y(frag_attrs_init, frag_attrs_deltas, min.y);
        update_attributes_x(frag_attrs_init, frag_attrs_deltas, min.x);
        
        // begin rasterization
        for(s32 y = min.y; y <= max.y; y++)
        {
            // start interpolating vertex attributes
            FragmentAttributes current_attributes = frag_attrs_init;
            glm::ivec3 half_space_x =
            {
                half_space_y.x,
                half_space_y.y,
                half_space_y.z
            };
            for(s32 x = min.x; x <= max.x; x++)
            {
                // if we are inside the triangle
                if( (half_space_x.x >= 0 || half_space_x.y >= 0 || half_space_x.z >= 0) &&
                   !(half_space_x.x <  0 || half_space_x.y <  0 || half_space_x.z <  0))
                {
                    auto original_color = Color15Bit(vram_read(x, y));

                    if(m_mask_bit_setting.check_mask_before_draw)
                    {
                        // skip masked fragments
                        if(original_color.mask)
                        {
                            half_space_x.x += delta_cb.y;
                            half_space_x.y += delta_ac.y;
                            half_space_x.z += delta_ba.y;
                            update_attributes_x(current_attributes, frag_attrs_deltas, 1);
                            continue;
                        }
                    }

                    auto new_color_from_interpolation = Color24Bit
                    (
                        current_attributes.r.to_float(),
                        current_attributes.g.to_float(),
                        current_attributes.b.to_float()
                    );

                    if(!args.is_raw_texture && m_draw_mode.dither_24_to_15)
                    {
                        new_color_from_interpolation = dither(new_color_from_interpolation, x, y);
                    }

                    auto new_color = Color15Bit();
                    if(args.color_depth == 0)
                    {
                        if(args.is_gouraud_shaded)
                        {
                            new_color = Color15Bit::create_from_24bit(new_color_from_interpolation);
                        }
                        else
                        {
                            new_color = Color15Bit::create_from_24bit(args.vertex_a.color);
                        }
                    }
                    else
                    {
                        new_color = vram_fetch_texture_color
                        (
                            args.color_depth, 
                            mask_texture_u(current_attributes.u.to_float()), 
                            mask_texture_v(current_attributes.v.to_float()), 
                            args.texpage_x, 
                            args.texpage_y
                        );
                        
                        // 16bit texture is fully transparent when the source color is 0
                        if(new_color.raw == 0x0000)
                        {
                            half_space_x.x += delta_cb.y;
                            half_space_x.y += delta_ac.y;
                            half_space_x.z += delta_ba.y;
                            update_attributes_x(current_attributes, frag_attrs_deltas, 1);
                            continue;
                        }

                        if(!args.is_raw_texture)
                        {
                            if(args.is_gouraud_shaded)
                            {
                                new_color = Color15Bit::create_mix(new_color_from_interpolation, new_color);
                            }
                            else
                            {
                                new_color = Color15Bit::create_mix(args.vertex_a.color, new_color);
                            }
                        }
                    }

                    // blend if transparent
                    if(args.is_semi_transparent)
                    {
                        if(new_color.mask || args.color_depth == 0)
                        {
                            new_color = Color15Bit::create_blended(original_color, new_color, args.semi_transparency);
                        }
                    }
                    
                    // update vram
                    new_color.mask |= m_mask_bit_setting.set_mask_while_drawing;
                    vram_write(x, y, new_color.raw);
                }

                // update per-fragment attributes in the horizontal direction
                half_space_x.x += delta_cb.y;
                half_space_x.y += delta_ac.y;
                half_space_x.z += delta_ba.y;
                update_attributes_x(current_attributes, frag_attrs_deltas, 1);
            }

            // update per-fragment attributes in the vertical direction
            half_space_y.x += delta_cb.x;
            half_space_y.y += delta_ac.x;
            half_space_y.z += delta_ba.x;
            update_attributes_y(frag_attrs_init, frag_attrs_deltas, 1);
        }
    }

    void GPU::do_polygon_render_hires(PolygonRenderArguments args)
    {
        /**
         * @brief calculate 2D cector cross product 
         */
        auto cross = [](const glm::ivec2& a, const glm::ivec2& b) 
        -> s32
        {
            return a.x * b.y - a.y * b.x;
        };

        /**
         * @brief calculate vertex argument interpolation
         */
        auto fragment_attribute = [cross](s32 area, glm::ivec2 vertices[3], s32 biases[3], s32 atrributes[3])
        -> fixed_point
        {
            float attr_a = cross(vertices[1], vertices[2]) * atrributes[0] - biases[0];
            float attr_b = cross(vertices[2], vertices[0]) * atrributes[1] - biases[1];
            float attr_c = cross(vertices[0], vertices[1]) * atrributes[2] - biases[2];

            return fixed_point((attr_a + attr_b + attr_c) / area + 0.5f);
        };

        /**
         * @brief calculate attribute 2D offset 
         */
        auto fragment_attribute_delta = [](s32 area, glm::ivec2 vertices[3], s32 attributes[3])
        -> FragmentAttributeDelta
        {
            return
            {
                fixed_point(((vertices[1].y - vertices[2].y) * attributes[0] + 
                             (vertices[2].y - vertices[0].y) * attributes[1] + 
                             (vertices[0].y - vertices[1].y) * attributes[2]) / float(area)),
                fixed_point(((vertices[2].x - vertices[1].x) * attributes[0] + 
                             (vertices[0].x - vertices[2].x) * attributes[1] + 
                             (vertices[1].x - vertices[0].x) * attributes[2]) / float(area)),
            };
        };

        /**
         * @brief efficiently update the attributes interpolation when
         *        moving onto the next pixel in the horizontal direction 
         */
        auto update_attributes_x = [args](FragmentAttributes& attributes, const FragmentAttributesDeltas& deltas, s32 delta)
        {
            if(args.is_gouraud_shaded)
            {
                attributes.r = fixed_point(attributes.r.to_float() + deltas.r.x.to_float() * delta);
                attributes.g = fixed_point(attributes.g.to_float() + deltas.g.x.to_float() * delta);
                attributes.b = fixed_point(attributes.b.to_float() + deltas.b.x.to_float() * delta);
            }

            if(args.color_depth != 0)
            {
                attributes.u = fixed_point(attributes.u.to_float() + deltas.u.x.to_float() * delta);
                attributes.v = fixed_point(attributes.v.to_float() + deltas.v.x.to_float() * delta);
            }
        };

        /**
         * @brief efficiently update the attributes interpolation when
         *        moving onto the next pixel in the vertical direction 
         */
        auto update_attributes_y = [args](FragmentAttributes& attributes, const FragmentAttributesDeltas& deltas, s32 delta)
        {
            if(args.is_gouraud_shaded)
            {
                attributes.r = fixed_point(attributes.r.to_float() + deltas.r.y.to_float() * delta);
                attributes.g = fixed_point(attributes.g.to_float() + deltas.g.y.to_float() * delta);
                attributes.b = fixed_point(attributes.b.to_float() + deltas.b.y.to_float() * delta);
            }

            if(args.color_depth != 0)
            {
                attributes.u = fixed_point(attributes.u.to_float() + deltas.u.y.to_float() * delta);
                attributes.v = fixed_point(attributes.v.to_float() + deltas.v.y.to_float() * delta);
            }
        };

        // assure counter clock wise winding of the rasterized triangle
        {
            glm::ivec2 temp_vertex_a = { args.vertex_a.pos_x, args.vertex_a.pos_y };
            glm::ivec2 temp_vertex_b = { args.vertex_b.pos_x, args.vertex_b.pos_y };
            glm::ivec2 temp_vertex_c = { args.vertex_c.pos_x, args.vertex_c.pos_y };
            auto temp_ba = temp_vertex_b - temp_vertex_a;
            auto temp_ca = temp_vertex_c - temp_vertex_a;

            // if clock-wise windwing -> flip the triangle order
            if(cross(temp_ba, temp_ca) < 0)
            {
                std::swap(args.vertex_b, args.vertex_c);
            }
        }

        // collect vertex positions so we can use
        // "glm" to work with them more efficiently
        glm::ivec2 vertices[3] =
        {
            { args.vertex_a.pos_x * VRamHiresScale, args.vertex_a.pos_y * VRamHiresScale },
            { args.vertex_b.pos_x * VRamHiresScale, args.vertex_b.pos_y * VRamHiresScale },
            { args.vertex_c.pos_x * VRamHiresScale, args.vertex_c.pos_y * VRamHiresScale }
        };

        s32 area = cross(vertices[1] - vertices[0], 
                         vertices[2] - vertices[0]);

        if(area == 0)
            return;

        // calculate triangle bounds
        glm::ivec2 min = 
        {
            std::min(std::min(vertices[0].x, vertices[1].x), vertices[2].x),
            std::min(std::min(vertices[0].y, vertices[1].y), vertices[2].y),
        };

        glm::ivec2 max = 
        {
            std::max(std::max(vertices[0].x, vertices[1].x), vertices[2].x),
            std::max(std::max(vertices[0].y, vertices[1].y), vertices[2].y),
        };

        // limit triangle size
        glm::ivec2 size = max - min;

        if(size.x > 1023 * VRamHiresScale || size.y > 511 * VRamHiresScale)
            return;

        //update_clut_cache(args.color_depth, args.clut_x, args.clut_y);

        min.x = clamp_drawing_area_left_hires(min.x);
        min.y = clamp_drawing_area_top_hires(min.y);
        max.x = clamp_drawing_area_right_hires(max.x);
        max.y = clamp_drawing_area_bottom_hires(max.y);

        glm::ivec2 delta_ba
        (
            vertices[1].x - vertices[0].x, 
            vertices[0].y - vertices[1].y
        );

        glm::ivec2 delta_cb
        (
            vertices[2].x - vertices[1].x, 
            vertices[1].y - vertices[2].y
        );

        glm::ivec2 delta_ac
        (
            vertices[0].x - vertices[2].x, 
            vertices[2].y - vertices[0].y
        );

        s32 biases[3] =
        {
            (delta_cb.y < 0 || (delta_cb.y == 0 && delta_cb.x < 0)) ? -1 : 0,
            (delta_ac.y < 0 || (delta_ac.y == 0 && delta_ac.x < 0)) ? -1 : 0,
            (delta_ba.y < 0 || (delta_ba.y == 0 && delta_ba.x < 0)) ? -1 : 0
        };

        glm::ivec3 half_space_y =
        {
            cross(vertices[2] - vertices[1],
                  min         - vertices[1]) + biases[0],
            cross(vertices[0] - vertices[2],
                  min         - vertices[2]) + biases[1],
            cross(vertices[1] - vertices[0],
                  min         - vertices[0]) + biases[2],
        };

        // calculate per-fragment initial attributes and deltas
        FragmentAttributes       frag_attrs_init;
        FragmentAttributesDeltas frag_attrs_deltas;

        if(args.is_gouraud_shaded)
        {
            s32 attr_color_r[3] = { s32(args.vertex_a.color.r), s32(args.vertex_b.color.r), s32(args.vertex_c.color.r) };
            s32 attr_color_g[3] = { s32(args.vertex_a.color.g), s32(args.vertex_b.color.g), s32(args.vertex_c.color.g) };
            s32 attr_color_b[3] = { s32(args.vertex_a.color.b), s32(args.vertex_b.color.b), s32(args.vertex_c.color.b) };

            frag_attrs_init.r = fragment_attribute(area, vertices, biases, attr_color_r);
            frag_attrs_init.g = fragment_attribute(area, vertices, biases, attr_color_g);
            frag_attrs_init.b = fragment_attribute(area, vertices, biases, attr_color_b);
            frag_attrs_deltas.r = fragment_attribute_delta(area, vertices, attr_color_r);
            frag_attrs_deltas.g = fragment_attribute_delta(area, vertices, attr_color_g);
            frag_attrs_deltas.b = fragment_attribute_delta(area, vertices, attr_color_b);
        }

        if(args.color_depth != 0)
        {
            s32 u[3] = { args.vertex_a.uv_x, args.vertex_b.uv_x, args.vertex_c.uv_x };
            s32 v[3] = { args.vertex_a.uv_y, args.vertex_b.uv_y, args.vertex_c.uv_y };
            frag_attrs_init.u = fragment_attribute(area, vertices, biases, u);
            frag_attrs_init.v = fragment_attribute(area, vertices, biases, v);
            frag_attrs_deltas.u = fragment_attribute_delta(area, vertices, u);
            frag_attrs_deltas.v = fragment_attribute_delta(area, vertices, v);
        }

        update_attributes_y(frag_attrs_init, frag_attrs_deltas, min.y);
        update_attributes_x(frag_attrs_init, frag_attrs_deltas, min.x);
        
        // begin rasterization
        for(s32 y = min.y; y <= max.y; y++)
        {
            // start interpolating vertex attributes
            FragmentAttributes current_attributes = frag_attrs_init;
            glm::ivec3 half_space_x =
            {
                half_space_y.x,
                half_space_y.y,
                half_space_y.z
            };
            for(s32 x = min.x; x <= max.x; x++)
            {
                // if we are inside the triangle
                if( (half_space_x.x >= 0 || half_space_x.y >= 0 || half_space_x.z >= 0) &&
                   !(half_space_x.x <  0 || half_space_x.y <  0 || half_space_x.z <  0))
                {
                    auto original_color = Color15Bit(vram_read_hires(x, y));

                    if(m_mask_bit_setting.check_mask_before_draw)
                    {
                        // skip masked fragments
                        if(original_color.mask)
                        {
                            half_space_x.x += delta_cb.y;
                            half_space_x.y += delta_ac.y;
                            half_space_x.z += delta_ba.y;
                            update_attributes_x(current_attributes, frag_attrs_deltas, 1);
                            continue;
                        }
                    }

                    auto new_color_from_interpolation = Color24Bit
                    (
                        current_attributes.r.to_float(),
                        current_attributes.g.to_float(),
                        current_attributes.b.to_float()
                    );

                    if(!args.is_raw_texture && m_draw_mode.dither_24_to_15)
                    {
                        new_color_from_interpolation = dither(new_color_from_interpolation, x, y);
                    }

                    auto new_color = Color15Bit();
                    if(args.color_depth == 0)
                    {
                        if(args.is_gouraud_shaded)
                        {
                            new_color = Color15Bit::create_from_24bit(new_color_from_interpolation);
                        }
                        else
                        {
                            new_color = Color15Bit::create_from_24bit(args.vertex_a.color);
                        }
                    }
                    else
                    {
                        new_color = vram_fetch_texture_color
                        (
                            args.color_depth, 
                            mask_texture_u(current_attributes.u.to_float()), 
                            mask_texture_v(current_attributes.v.to_float()), 
                            args.texpage_x, 
                            args.texpage_y
                        );
                        
                        // 16bit texture is fully transparent when the source color is 0
                        if(new_color.raw == 0x0000)
                        {
                            half_space_x.x += delta_cb.y;
                            half_space_x.y += delta_ac.y;
                            half_space_x.z += delta_ba.y;
                            update_attributes_x(current_attributes, frag_attrs_deltas, 1);
                            continue;
                        }

                        if(!args.is_raw_texture)
                        {
                            if(args.is_gouraud_shaded)
                            {
                                new_color = Color15Bit::create_mix(new_color_from_interpolation, new_color);
                            }
                            else
                            {
                                new_color = Color15Bit::create_mix(args.vertex_a.color, new_color);
                            }
                        }
                    }

                    // blend if transparent
                    if(args.is_semi_transparent)
                    {
                        if(new_color.mask || args.color_depth == 0)
                        {
                            new_color = Color15Bit::create_blended(original_color, new_color, args.semi_transparency);
                        }
                    }
                    
                    // update vram
                    new_color.mask |= m_mask_bit_setting.set_mask_while_drawing;
                    vram_write_hires(x, y, new_color.raw);
                }

                // update per-fragment attributes in the horizontal direction
                half_space_x.x += delta_cb.y;
                half_space_x.y += delta_ac.y;
                half_space_x.z += delta_ba.y;
                update_attributes_x(current_attributes, frag_attrs_deltas, 1);
            }

            // update per-fragment attributes in the vertical direction
            half_space_y.x += delta_cb.x;
            half_space_y.y += delta_ac.x;
            half_space_y.z += delta_ba.x;
            update_attributes_y(frag_attrs_init, frag_attrs_deltas, 1);
        }
    }

    /**
     * @brief Render Line GPU Command 
     */
    void GPU::line_render()
    {
        LineRenderCommand command(m_command_fifo.at(0) >> 24);

        u32 start_color =  m_command_fifo.at(0) & 0x00FF'FFFF;
        s32 start_x     = (m_command_fifo.at(1) >>  0) & 0xFFFF;
        s32 start_y     = (m_command_fifo.at(1) >> 16) & 0xFFFF;
        u32 end_color   = 0;
        s32 end_x       = 0;
        s32 end_y       = 0;
        if(command.num_arguments() == 4)
        {
            end_color = m_command_fifo.at(2) & 0x00FF'FFFF;
            end_x     = (m_command_fifo.at(3) >>  0) & 0xFFFF;
            end_y     = (m_command_fifo.at(3) >> 16) & 0xFFFF;
        }
        else // command.num_arguments() == 3
        {
            end_color = start_color;
            end_x     = (m_command_fifo.at(2) >>  0) & 0xFFFF;
            end_y     = (m_command_fifo.at(2) >> 16) & 0xFFFF;
        }

        do_line_render(LineRenderArguments
        {
            .start_x     = m_drawing_offset_x + extend_sign<s32, 11>(start_x),
            .start_y     = m_drawing_offset_y + extend_sign<s32, 11>(start_y),
            .end_x       = m_drawing_offset_x + extend_sign<s32, 11>(end_x),
            .end_y       = m_drawing_offset_y + extend_sign<s32, 11>(end_y),
            .start_color = start_color,
            .end_color   = end_color,
            .is_semi_transparent = command.is_semi_transparent,
            .is_gouraud_shaded   = command.is_gouraud_shaded
        }, m_meta_resolution);

        if(!command.is_poly_line)
        {
            // reset command queue
            m_current_command = GPUCommand::Nop;
        }
        else
        {
            // shift arguments in command fifo
            TODO();
        }
    }

    /**
     * @brief Perform Render Line GPU Command 
     */
    void GPU::do_line_render(LineRenderArguments args, RenderTarget target)
    {
        switch(target)
        {
            case RenderTarget::VRam1x: { do_line_render(args); } break;
            case RenderTarget::VRam2x: { do_line_render(args); do_line_render_hires(args); } break;
            default:
            {
                UNREACHABLE();
            } break;
        }
    }

    void GPU::do_line_render(LineRenderArguments args)
    {
        if(args.start_x > args.end_x)
        {
            std::swap(args.start_x, args.end_x);
        }
        if(args.start_y > args.end_y)
        {
            std::swap(args.start_y, args.end_y);
        }

        args.start_x = clamp_drawing_area_left(args.start_x);
        args.start_y = clamp_drawing_area_top(args.start_y);
        args.end_x = clamp_drawing_area_right(args.end_x);
        args.end_y = clamp_drawing_area_bottom(args.end_y);

        if(args.start_x > args.end_x)
        {
            std::swap(args.start_x, args.end_x);
        }
        if(args.start_y > args.end_y)
        {
            std::swap(args.start_y, args.end_y);
        }

        args.start_x = clamp_drawing_area_left(args.start_x);
        args.start_y = clamp_drawing_area_top(args.start_y);
        args.end_x = clamp_drawing_area_right(args.end_x);
        args.end_y = clamp_drawing_area_bottom(args.end_y);

        s32 delta_x = (args.end_x - args.start_x);
        s32 delta_y = (args.end_y - args.start_y);

        bool swapped = false;

        if(std::abs(delta_y) > std::abs(delta_x))
        {
            std::swap(delta_x,      delta_y);
            std::swap(args.start_x, args.start_y);
            std::swap(args.end_x,   args.end_y);
            swapped = true;
        }

        if(delta_x == 0)
            return;

        s32 y = args.start_y << 8;
        s32 k = (delta_y << 8) / delta_x;

        for(s32 x = args.start_x; x <= args.end_x; x++)
        {
            auto original_color = Color15Bit(vram_read(swapped ? (y >> 8) : x, swapped ? x : (y >> 8)));

            if(m_mask_bit_setting.check_mask_before_draw)
            {
                // skip masked fragments
                if(original_color.mask)
                {
                    y += k;
                    continue;
                }
            }

            //TODO: line interpolation
            auto new_color_from_interpolation = Color24Bit(args.start_color);

            if(m_draw_mode.dither_24_to_15)
            {
                new_color_from_interpolation = dither(new_color_from_interpolation, swapped ? (y >> 8) : x, swapped ? x : (y >> 8));
            }

            auto new_color = Color15Bit();
            if(args.is_gouraud_shaded)
            {
                new_color = Color15Bit::create_from_24bit(new_color_from_interpolation);
            }
            else
            {
                new_color = Color15Bit::create_from_24bit(args.start_color);
            }

            // blend if transparent
            if(args.is_semi_transparent)
            {
                if(new_color.mask)
                {
                    new_color = Color15Bit::create_blended(original_color, new_color, m_draw_mode.semi_transparency);
                }
            }
            
            // update vram
            new_color.mask |= m_mask_bit_setting.set_mask_while_drawing;
            vram_write(swapped ? (y >> 8) : x, swapped ? x : (y >> 8), new_color.raw);
            y += k;
        }
    }

    void GPU::do_line_render_hires(LineRenderArguments args)
    {
        if(args.start_x > args.end_x)
        {
            std::swap(args.start_x, args.end_x);
        }
        if(args.start_y > args.end_y)
        {
            std::swap(args.start_y, args.end_y);
        }

        args.start_x = clamp_drawing_area_left(args.start_x);
        args.start_y = clamp_drawing_area_top(args.start_y);
        args.end_x = clamp_drawing_area_right(args.end_x);
        args.end_y = clamp_drawing_area_bottom(args.end_y);


        if(args.start_x > args.end_x)
        {
            std::swap(args.start_x, args.end_x);
        }
        if(args.start_y > args.end_y)
        {
            std::swap(args.start_y, args.end_y);
        }

        args.start_x = clamp_drawing_area_left(args.start_x);
        args.start_y = clamp_drawing_area_top(args.start_y);
        args.end_x = clamp_drawing_area_right(args.end_x);
        args.end_y = clamp_drawing_area_bottom(args.end_y);

        args.start_x *= VRamHiresScale;
        args.start_y *= VRamHiresScale;
        args.end_x   *= VRamHiresScale;
        args.end_y   *= VRamHiresScale;

        s32 delta_x = (args.end_x - args.start_x);
        s32 delta_y = (args.end_y - args.start_y);

        bool swapped = false;

        if(std::abs(delta_y) > std::abs(delta_x))
        {
            std::swap(delta_x,      delta_y);
            std::swap(args.start_x, args.start_y);
            std::swap(args.end_x,   args.end_y);
            swapped = true;
        }

        if(delta_x == 0)
            return;

        s32 y = args.start_y << 8;
        s32 k = (delta_y << 8) / delta_x;

        for(s32 x = args.start_x; x <= args.end_x; x++)
        {
            auto original_color = Color15Bit(vram_read_hires(swapped ? (y >> 8) : x, swapped ? x : (y >> 8)));

            if(m_mask_bit_setting.check_mask_before_draw)
            {
                // skip masked fragments
                if(original_color.mask)
                {
                    y += k;
                    continue;
                }
            }

            //TODO: line interpolation
            auto new_color_from_interpolation = Color24Bit(args.start_color);

            if(m_draw_mode.dither_24_to_15)
            {
                new_color_from_interpolation = dither(new_color_from_interpolation, swapped ? (y >> 8) : x, swapped ? x : (y >> 8));
            }

            auto new_color = Color15Bit();
            if(args.is_gouraud_shaded)
            {
                new_color = Color15Bit::create_from_24bit(new_color_from_interpolation);
            }
            else
            {
                new_color = Color15Bit::create_from_24bit(args.start_color);
            }

            // blend if transparent
            if(args.is_semi_transparent)
            {
                if(new_color.mask)
                {
                    new_color = Color15Bit::create_blended(original_color, new_color, m_draw_mode.semi_transparency);
                }
            }
            
            // update vram
            new_color.mask |= m_mask_bit_setting.set_mask_while_drawing;
            vram_write_hires(swapped ? (y >> 8) : x, swapped ? x : (y >> 8), new_color.raw);
            y += k;
        }
    }

    /**
     * @brief Render Rectangle GPU Command 
     */
    void GPU::rectangle_render()
    {
        RectangleRenderCommand command(m_command_fifo.at(0) >> 24);

        u32 width, height; width = height = command.actual_size();

        // 0 indicates variable size based on arguments
        if(width == 0)
        {
            // the width/height argument can be the 3rd or 4th argument
            // depending on if the rectangle is texture mapped
            u32 width_height_argument_index = command.is_texture_mapped ? 3 : 2;
            u32 width_height_value          = m_command_fifo.at(width_height_argument_index);
            width  = (width_height_value >>  0) & 0xFFFF;
            height = (width_height_value >> 16) & 0xFFFF;
        }

        // get top-left vertex
        u32 start_value = m_command_fifo.at(1);
        s32 start_x = extend_sign<s32, 11>((start_value >>  0) & 0xFFFF) + m_drawing_offset_x;
        s32 start_y = extend_sign<s32, 11>((start_value >> 16) & 0xFFFF) + m_drawing_offset_y;

        // get fill color
        auto color = Color24Bit(m_command_fifo.at(0) & 0x00FF'FFFF);

        // color bit depth
        u32 color_bits = 0;

        // if rectangle is textured we will need uv coordinates, 
        // clut coordinates and texture page coordinates
        u32 uv_x = 0;
        u32 uv_y = 0;
        u32 clut_x = 0;
        u32 clut_y = 0;
        u32 texpage_x = 0;
        u32 texpage_y = 0;

        if(command.is_texture_mapped)
        {
            switch(m_draw_mode.texture_page_colors)
            {
                // 4bit depth
                case 0: { color_bits = 1; } break;
                // 8bit depth
                case 1: { color_bits = 2; } break;
                // 15bit depth
                case 2:
                // reserved = 15bit depth
                case 3: { color_bits = 3; } break;
                default: { UNREACHABLE(); } break;
            }

            u32 clut_value = m_command_fifo.at(2);

            uv_x = (clut_value >> 0) & 0xFF;
            uv_y = (clut_value >> 8) & 0xFF;
            clut_x = ((clut_value >> 16) & 0b0011'1111) * 16;
            clut_y = (clut_value >> 22) & 0b0001'1111'1111;
            texpage_x = m_draw_mode.texture_page_x_base * 64;
            texpage_y = m_draw_mode.texture_page_y_base_1 * 256;
        }

        // rasterize rectangle
        do_rectangle_render(RectangleRenderArguments
        {
            .start_x     = start_x,
            .start_y     = start_y,
            .width       = width,
            .height      = height,
            .color       = color,
            .color_depth = color_bits,
            .uv_x        = uv_x,
            .uv_y        = uv_y,
            .clut_x      = clut_x,
            .clut_y      = clut_y,
            .texpage_x   = texpage_x,
            .texpage_y   = texpage_y,
            .is_semi_transparent = command.is_semi_transparent,
            .is_raw_texture      = command.is_raw_texture
        }, m_meta_resolution);

        // reset command queue
        m_current_command = GPUCommand::Nop;
    }

    /**
     * @brief Perform Render Rectangle GPU Command 
     */
    void GPU::do_rectangle_render(RectangleRenderArguments args, RenderTarget target)
    {
        switch(target)
        {
            case RenderTarget::VRam1x: { do_rectangle_render(args); } break;
            case RenderTarget::VRam2x: { do_rectangle_render(args); do_rectangle_render_hires(args); } break;
            default:
            {
                UNREACHABLE();
            } break;
        }
    }

    void GPU::do_rectangle_render(RectangleRenderArguments args)
    {
        if(args.width > 1023 || args.height > 511)
            return;

        update_clut_cache(args.color_depth, args.clut_x, args.clut_y);

        s32 min_x = clamp_drawing_area_left(args.start_x);
        s32 min_y = clamp_drawing_area_top(args.start_y);

        s32 max_x = clamp_drawing_area_right(args.start_x + args.width - 1);
        s32 max_y = clamp_drawing_area_bottom(args.start_y + args.height - 1);

        s32 uv_x = args.uv_x + (min_x - args.start_x) + m_draw_mode.texture_rect_x_flip;
        s32 uv_y = args.uv_y + (min_y - args.start_y) + m_draw_mode.texture_rect_y_flip;

        s32 dir_x = m_draw_mode.texture_rect_x_flip ? -1 : 1;
        s32 dir_y = m_draw_mode.texture_rect_y_flip ? -1 : 1;

        Color15Bit color_15_bit = Color15Bit::create_from_24bit(args.color);

        for(s32 y = min_y, v = uv_y; y <= max_y; y++, v += dir_y)
        {
            for(s32 x = min_x, u = uv_x; x <= max_x; x++, u += dir_x)
            {
                // get original color for blending
                Color15Bit original_color = Color15Bit(vram_read(x, y));

                if(m_mask_bit_setting.check_mask_before_draw)
                {
                    if(original_color.mask)
                        continue;
                }

                // create new color
                Color15Bit new_color;

                if(args.color_depth == 0)
                {
                    new_color      = color_15_bit;
                    new_color.mask = 0;
                }
                else
                {
                    // fetch texture color
                    new_color = vram_fetch_texture_color
                    (
                        args.color_depth, 
                        mask_texture_u(u), 
                        mask_texture_v(v), 
                        args.texpage_x, 
                        args.texpage_y
                    );

                    // 16bit texture is fully transparent when the source color is 0
                    if(new_color.raw == 0x0000)
                    {
                        continue;
                    }

                    // mix texture with the rectangle color
                    if(!args.is_raw_texture)
                    {
                        new_color = Color15Bit::create_mix(args.color, new_color);
                    }
                }

                // blend if transparent
                if(args.is_semi_transparent)
                {
                    if(new_color.mask || args.color_depth == 0)
                    {
                        new_color = Color15Bit::create_blended(original_color, new_color, m_draw_mode.semi_transparency);
                    }
                }

                // update vram
                new_color.mask |= m_mask_bit_setting.set_mask_while_drawing;
                vram_write(x, y, new_color.raw);
            }
        }
    }

    void GPU::do_rectangle_render_hires(RectangleRenderArguments args)
    {
        if(args.width > 1023 || args.height > 511)
            return;

        s32 min_x = clamp_drawing_area_left(args.start_x);
        s32 min_y = clamp_drawing_area_top(args.start_y);

        s32 max_x = clamp_drawing_area_right(args.start_x + args.width - 1);
        s32 max_y = clamp_drawing_area_bottom(args.start_y + args.height - 1);

        s32 uv_x = args.uv_x + (min_x - args.start_x) + m_draw_mode.texture_rect_x_flip;
        s32 uv_y = args.uv_y + (min_y - args.start_y) + m_draw_mode.texture_rect_y_flip;

        s32 dir_x = m_draw_mode.texture_rect_x_flip ? -1 : 1;
        s32 dir_y = m_draw_mode.texture_rect_y_flip ? -1 : 1;

        Color15Bit color_15_bit = Color15Bit::create_from_24bit(args.color);

        for(s32 y = min_y, v = uv_y; y <= max_y; y++, v += dir_y)
        {
            for(s32 x = min_x, u = uv_x; x <= max_x; x++, u += dir_x)
            {
                for(s32 res_y = 0; res_y < VRamHiresScale; res_y++)
                {
                    for(s32 res_x = 0; res_x < VRamHiresScale; res_x++)
                    {
                        // get original color for blending
                        Color15Bit original_color = Color15Bit(vram_read_hires(x * VRamHiresScale + res_x, y * VRamHiresScale + res_y));

                        if(m_mask_bit_setting.check_mask_before_draw)
                        {
                            if(original_color.mask)
                                continue;
                        }

                        // create new color
                        Color15Bit new_color;

                        if(args.color_depth == 0)
                        {
                            new_color      = color_15_bit;
                            new_color.mask = 0;
                        }
                        else
                        {
                            // fetch texture color
                            new_color = vram_fetch_texture_color
                            (
                                args.color_depth, 
                                mask_texture_u(u), 
                                mask_texture_v(v), 
                                args.texpage_x, 
                                args.texpage_y
                            );

                            // 16bit texture is fully transparent when the source color is 0
                            if(new_color.raw == 0x0000)
                            {
                                continue;
                            }

                            // mix texture with the rectangle color
                            if(!args.is_raw_texture)
                            {
                                new_color = Color15Bit::create_mix(args.color, new_color);
                            }
                        }

                        // blend if transparent
                        if(args.is_semi_transparent)
                        {
                            if(new_color.mask || args.color_depth == 0)
                            {
                                new_color = Color15Bit::create_blended(original_color, new_color, m_draw_mode.semi_transparency);
                            }
                        }

                        // update vram
                        new_color.mask |= m_mask_bit_setting.set_mask_while_drawing;
                        vram_write_hires(x * VRamHiresScale + res_x, y * VRamHiresScale + res_y, new_color.raw);
                    }
                }
            }
        }
    }

    /**
     * @brief Copy RAM to VRAM GPU Command 
     */
    void GPU::copy_cpu_to_vram_parsing_phase()
    {
        m_dma_start_x = m_dma_current_x = mask_dma_x((m_command_fifo.at(1) >>  0) & 0xFFFF);
        m_dma_start_y = m_dma_current_y = mask_dma_y((m_command_fifo.at(1) >> 16) & 0xFFFF);
        m_dma_end_x   = m_dma_start_x + mask_dma_width((m_command_fifo.at(2) >>  0) & 0xFFFF);
        m_dma_end_y   = m_dma_start_y + mask_dma_height((m_command_fifo.at(2) >> 16) & 0xFFFF);

        // expect cpu data from the command queue
        m_current_command = GPUCommand::CopyCPUToVRamDataPhase;
        m_command_fifo.clear();
        m_command_num_arguments = 1;
    }

    /**
     * @brief Copy RAM to VRAM GPU Command 
     */
    void GPU::copy_cpu_to_vram_data_phase()
    {
        // extract value
        u32 value = m_command_fifo.at(0);

        // expect next data point
        m_command_fifo.clear();
        
        do_copy_cpu_to_vram_data_phase(value, m_meta_resolution);
        
        // keep the current command as CopyCPUToVRamDataPhase
        // only change it when we reached the end of the DMA
    }

    void GPU::do_copy_cpu_to_vram_data_phase(u32 value, RenderTarget target)
    {
        switch(target)
        {
            case RenderTarget::VRam1x: { do_copy_cpu_to_vram_data_phase(value); } break;
            case RenderTarget::VRam2x: { do_copy_cpu_to_vram_data_phase_hires(value); } break;
            default:
            {
                UNREACHABLE();
            } break;
        }
    }

    void GPU::do_copy_cpu_to_vram_data_phase(u32 value)
    {
        vram_write_with_mask(m_dma_current_x, m_dma_current_y, (value >>  0) & 0xFFFF);

        m_dma_current_x++;
        if(m_dma_current_x >= m_dma_end_x)
        {
            m_dma_current_x = m_dma_start_x;
            m_dma_current_y++;
            if(m_dma_current_y >= m_dma_end_y)
            {
                // we reached the end, reset the command queue
                m_current_command = GPUCommand::Nop;
                return;
            }
        }

        vram_write_with_mask(m_dma_current_x, m_dma_current_y, (value >> 16) & 0xFFFF);

        m_dma_current_x++;
        if(m_dma_current_x >= m_dma_end_x)
        {
            m_dma_current_x = m_dma_start_x;
            m_dma_current_y++;
            if(m_dma_current_y >= m_dma_end_y)
            {
                // we reached the end, reset the command queue
                m_current_command = GPUCommand::Nop;
                return;
            }
        }
    }

    void GPU::do_copy_cpu_to_vram_data_phase_hires(u32 value)
    {
        vram_write_with_mask(m_dma_current_x, m_dma_current_y, (value >>  0) & 0xFFFF);
        for(u32 y = 0; y < VRamHiresScale; y++)
        {
            for(u32 x = 0; x < VRamHiresScale; x++)
            {
                vram_write_with_mask_hires(m_dma_current_x * VRamHiresScale + x, m_dma_current_y * VRamHiresScale + y, (value >>  0) & 0xFFFF);
            }
        }

        m_dma_current_x++;
        if(m_dma_current_x >= m_dma_end_x)
        {
            m_dma_current_x = m_dma_start_x;
            m_dma_current_y++;
            if(m_dma_current_y >= m_dma_end_y)
            {
                // we reached the end, reset the command queue
                m_current_command = GPUCommand::Nop;
                return;
            }
        }

        vram_write_with_mask(m_dma_current_x, m_dma_current_y, (value >> 16) & 0xFFFF);
        for(u32 y = 0; y < VRamHiresScale; y++)
        {
            for(u32 x = 0; x < VRamHiresScale; x++)
            {
                vram_write_with_mask_hires(m_dma_current_x * VRamHiresScale + x, m_dma_current_y * VRamHiresScale + y, (value >> 16) & 0xFFFF);
            }
        }

        m_dma_current_x++;
        if(m_dma_current_x >= m_dma_end_x)
        {
            m_dma_current_x = m_dma_start_x;
            m_dma_current_y++;
            if(m_dma_current_y >= m_dma_end_y)
            {
                // we reached the end, reset the command queue
                m_current_command = GPUCommand::Nop;
                return;
            }
        }
    }

    /**
     * @brief Copy VRAM to RAM GPU Command 
     */
    void GPU::copy_vram_to_cpu()
    {
        m_dma_start_x = m_dma_current_x = mask_dma_x((m_command_fifo.at(1) >>  0) & 0xFFFF);
        m_dma_start_y = m_dma_current_y = mask_dma_y((m_command_fifo.at(1) >> 16) & 0xFFFF);
        m_dma_end_x   = m_dma_start_x + mask_dma_width((m_command_fifo.at(2) >>  0) & 0xFFFF);
        m_dma_end_y   = m_dma_start_y + mask_dma_height((m_command_fifo.at(2) >> 16) & 0xFFFF);

        // make VRAM accessible to DMA through the GPUREAD register
        m_read_mode = 1;

        // reset command queue
        m_current_command = GPUCommand::Nop;
    }

    /**
     * @brief Copy VRAM to VRAM GPU Command 
     */
    void GPU::copy_vram_to_vram()
    {
        u32 source_x      = mask_dma_x((m_command_fifo.at(1) >>  0) & 0xFFFF);
        u32 source_y      = mask_dma_y((m_command_fifo.at(1) >> 16) & 0xFFFF);
        u32 destination_x = mask_dma_x((m_command_fifo.at(2) >>  0) & 0xFFFF);
        u32 destination_y = mask_dma_y((m_command_fifo.at(2) >> 16) & 0xFFFF);
        u32 width         = mask_dma_width((m_command_fifo.at(3) >>  0) & 0xFFFF);
        u32 height        = mask_dma_height((m_command_fifo.at(3) >> 16) & 0xFFFF);

        do_copy_vram_to_vram(CopyVRAMToVRAMArguments
        {
            .source_x = source_x,
            .source_y = source_y,
            .destination_x = destination_x,
            .destination_y = destination_y,
            .width = width,
            .height = height,
        }, m_meta_resolution);

        // reset command queue
        m_current_command = GPUCommand::Nop;
    }

    void GPU::do_copy_vram_to_vram(CopyVRAMToVRAMArguments args, RenderTarget target)
    {
        switch(target)
        {
            case RenderTarget::VRam1x: { do_copy_vram_to_vram(args); } break;
            case RenderTarget::VRam2x: { do_copy_vram_to_vram(args); do_copy_vram_to_vram_hires(args); } break;
            default:
            {
                UNREACHABLE();
            } break;
        }
    }

    void GPU::do_copy_vram_to_vram(CopyVRAMToVRAMArguments args)
    {
        for(u32 y = 0; y < args.height; y++)
        {
            for(u32 x = 0; x < args.width; x++)
            {
                auto color = vram_read((args.source_x + x) % VRamWidth, (args.source_y + y) % VRamHeight);
                vram_write_with_mask(args.destination_x + x, args.destination_y + y, color);
            }
        }
    }

    void GPU::do_copy_vram_to_vram_hires(CopyVRAMToVRAMArguments args)
    {
        for(u32 y = 0; y < args.height * VRamHiresScale; y++)
        {
            for(u32 x = 0; x < args.width * VRamHiresScale; x++)
            {
                auto color = vram_read_hires(
                    (args.source_x * VRamHiresScale + x) % (VRamWidth * VRamHiresScale), 
                    (args.source_y * VRamHiresScale + y) % (VRamHeight * VRamHiresScale));
                vram_write_with_mask_hires(args.destination_x * VRamHiresScale + x, args.destination_y * VRamHiresScale + y, color);
            }
        }
    }
    /**
     * @brief clamp value to the drawing area
     */
    s32 GPU::clamp_drawing_area_left(s32 x) const
    {
        return std::max(std::max(x, 0), static_cast<s32>(m_drawing_area_left));
    }

    s32 GPU::clamp_drawing_area_left_hires(s32 x) const
    {
        return std::max(std::max(x, 0), static_cast<s32>(m_drawing_area_left * VRamHiresScale));
    }

    /**
     * @brief clamp value to the drawing area 
     */
    s32 GPU::clamp_drawing_area_right(s32 x) const
    {
        return std::min(std::min(x, static_cast<s32>(VRamWidth)), static_cast<s32>(m_drawing_area_right));
    }

    s32 GPU::clamp_drawing_area_right_hires(s32 x) const
    {
        return std::min(std::min(x, static_cast<s32>(VRamWidth * VRamHiresScale)), static_cast<s32>(m_drawing_area_right * VRamHiresScale));
    }

    /**
     * @brief clamp value to the drawing area
     */
    s32 GPU::clamp_drawing_area_top(s32 y) const
    {
        return std::max(std::max(y, 0), static_cast<s32>(m_drawing_area_top));
    }

    s32 GPU::clamp_drawing_area_top_hires(s32 y) const
    {
        return std::max(std::max(y, 0), static_cast<s32>(m_drawing_area_top * VRamHiresScale));
    }

    /**
     * @brief clamp value to the drawing area
     */
    s32 GPU::clamp_drawing_area_bottom(s32 y) const
    {
        return std::min(std::min(y, static_cast<s32>(VRamHeight)), static_cast<s32>(m_drawing_area_bottom));
    }

    s32 GPU::clamp_drawing_area_bottom_hires(s32 y) const
    {
        return std::min(std::min(y, static_cast<s32>(VRamHeight * VRamHiresScale)), static_cast<s32>(m_drawing_area_bottom * VRamHiresScale));
    }

    /**
     * @brief mask texture coordinate
     */
    s32 GPU::mask_texture_u(s32 u) const
    {
        return ((u % 256u) & ~(m_texture_window_setting.texture_window_mask_x * 8)) | 
                ((m_texture_window_setting.texture_window_offset_x & 
                  m_texture_window_setting.texture_window_mask_x) * 8);
    }

    /**
     * @brief mask texture coordinate
     */
    s32 GPU::mask_texture_v(s32 v) const
    {
        return ((v % 256u) & ~(m_texture_window_setting.texture_window_mask_y * 8)) | 
               ((m_texture_window_setting.texture_window_offset_y & 
                 m_texture_window_setting.texture_window_mask_y) * 8);
    }

    /**
     * @brief update clut cache
     */
    void GPU::update_clut_cache(u32 color_depth, u32 clut_x, u32 clut_y)
    {
        // only update cache if 4bit or 8bit color depth
        if(color_depth != 1 && color_depth != 2)
            return;

        auto clut_pos_changed   = (m_clut_cache_x.value_or(-1) != clut_x) || 
                                  (m_clut_cache_y.value_or(-1) != clut_y);
        auto clut_depth_changed = m_clut_cache_depth != color_depth;

        // clut position didn't change and texture color depth didn't change
        if(!clut_pos_changed && !clut_depth_changed)
            return;

        // update clut cache
        m_clut_cache_depth = color_depth;
        m_clut_cache_x     = clut_x;
        m_clut_cache_y     = clut_y;

        u32 num_entries = color_depth == 2 ? 256 : 16;
        for(u32 i = 0; i < num_entries; i++)
        {
            m_clut_cache[i] = vram_read(clut_x + i, clut_y);
        }
    }

    /**
     * @brief fetch texture color 
     */
    Color15Bit GPU::vram_fetch_texture_color(u32 color_depth, s32 uv_x, s32 uv_y, s32 texpage_x, s32 texpage_y)
    {
        switch(color_depth)
        {
            // 4bit color depth
            case 1:
            {
                u32 x = (texpage_x + uv_x / 4) % VRamWidth;
                u32 y = (texpage_y + uv_y) % VRamHeight;
                u16 clut_cache_index = vram_read(x, y);
                return Color15Bit(m_clut_cache[(clut_cache_index >> ((uv_x & 3) * 4)) & 0x0F]);
            }
            // 8bit color depth
            case 2:
            {
                u32 x = (texpage_x + uv_x / 2) % VRamWidth;
                u32 y = (texpage_y + uv_y) % VRamHeight;
                u16 clut_cache_index = vram_read(x, y);
                return Color15Bit(m_clut_cache[(clut_cache_index >> ((uv_x & 1) * 8)) & 0xFF]);
            }
            // 16bit color depth
            case 3:
            {
                u32 x = (texpage_x + uv_x) % VRamWidth;
                u32 y = (texpage_y + uv_y) % VRamHeight;
                return Color15Bit(vram_read(x, y));
            }
        }

        UNREACHABLE();
        return Color15Bit();
    }

    /**
     * @brief mask dma copy parameters 
     */
    u32 GPU::mask_dma_x(u32 x) const
    {
        return x & 0x3FF;
    }

    /**
     * @brief mask dma copy parameters 
     */
    u32 GPU::mask_dma_y(u32 y) const
    {
        return y & 0x1FF;
    }

    /**
     * @brief mask dma copy parameters 
     */
    u32 GPU::mask_dma_width(u32 width) const
    {
        return ((width - 1) & 0x3FF) + 1;
    }

    /**
     * @brief mask dma copy parameters 
     */
    u32 GPU::mask_dma_height(u32 height) const
    {
        return ((height - 1) & 0x1FF) + 1;
    }

    /**
     * @brief write to vram with respect to masking (the highest bit of the 16bit half word)
     *        since the vram contains colors with 15bit color depth the 16th bit can be used
     *        as a mask, whether it is allowed to draw into that specific vram space
     */
    void GPU::vram_write_with_mask(u32 x, u32 y, u16 color)
    {
        x %= VRamWidth;
        y %= VRamHeight;

        u16 mask = m_mask_bit_setting.set_mask_while_drawing ? 0x8000 : 0x0000;

        if(m_mask_bit_setting.check_mask_before_draw)
        {
            if(vram_read(x, y) & 0x8000)
                return;
        }

        vram_write(x, y, color | mask);
    }

    void GPU::vram_write_with_mask_hires(u32 x, u32 y, u16 color)
    {
        x %= VRamWidth * VRamHiresScale;
        y %= VRamHeight * VRamHiresScale;

        u16 mask = m_mask_bit_setting.set_mask_while_drawing ? 0x8000 : 0x0000;

        if(m_mask_bit_setting.check_mask_before_draw)
        {
            if(vram_read_hires(x, y) & 0x8000)
                return;
        }

        vram_write_hires(x, y, color | mask);
    }

    /**
     * @brief read color from vram
     */
    u16 GPU::vram_read(u32 x, u32 y) const
    {
        if(x > VRamWidth - 1) 
        {
            UNREACHABLE();
        }
        if(y > VRamHeight - 1)
        {
            UNREACHABLE();
        }
        return m_vram[y * VRamWidth + x];
    }

    u16 GPU::vram_read_hires(u32 x, u32 y) const
    {
        if(x > VRamWidth * VRamHiresScale - 1) 
        {
            UNREACHABLE();
        }
        if(y > VRamHeight * VRamHiresScale - 1)
        {
            printf("WHAT %u %i\n", y, (int)m_current_command);
            UNREACHABLE();
        }
        return m_vram_hires[y * VRamWidth * VRamHiresScale + x];
    }

    /**
     * @brief write color into vram with bounds check 
     */
    void GPU::vram_write(u32 x, u32 y, u16 value)
    {
        if(x > VRamWidth - 1) 
        {
            UNREACHABLE();
        }
        if(y > VRamHeight - 1)
        {
            UNREACHABLE();
        }
        m_vram[y * VRamWidth + x] = value;
    }

    void GPU::vram_write_hires(u32 x, u32 y, u16 value)
    {
        if(x > VRamWidth * VRamHiresScale - 1)
        {
            UNREACHABLE();
        }

        if(y > VRamHeight * VRamHiresScale - 1)
        {
            UNREACHABLE();
        }

        m_vram_hires[y * VRamWidth * VRamHiresScale + x] = value;
    }

    /**
     * @brief dither color if enabled in texpage attribute
     */
    Color24Bit GPU::dither(Color24Bit color, u32 x, u32 y)
    {
        static constexpr s32 dither_table[] =
        {
            -4, +0, -3, +1,
            +2, -2, +3, -1,
            -3, +1, -4, +0,
            +3, -1, +2, -2
        };

        x &= 0b11;
        y &= 0b11;

        color.r = std::clamp<s32>(color.r + dither_table[y * 4 + x], 0, 255);
        color.g = std::clamp<s32>(color.g + dither_table[y * 4 + x], 0, 255);
        color.b = std::clamp<s32>(color.b + dither_table[y * 4 + x], 0, 255);

        return color;
    }

    /**
     * @brief dump VRAM into a image file 
     */
    void GPU::meta_dump_vram() const
    {
        LOG("dumping vram to vram.png");

        std::vector<u8> vram(VRamWidth * VRamHeight * 4);

        for(u32 i = 0; i < VRamWidth * VRamHeight; i++)
        {
            Color15Bit parsed_color = Color15Bit(m_vram[i]);
            vram[i * 4 + 0] = parsed_color.r << 3;
            vram[i * 4 + 1] = parsed_color.g << 3;
            vram[i * 4 + 2] = parsed_color.b << 3;
            vram[i * 4 + 3] = 255;
        }

        stbi_write_png(fmt::format("vram/vram{}.png", m_meta_frames).c_str(), VRamWidth, VRamHeight, 4, vram.data(), VRamWidth * 4);
    }

    /**
     * @brief obtain the state of vram from gpu
     */
    const std::array<u16, VRamWidth * VRamHeight>& GPU::meta_get_vram_buffer() const
    {
        return m_vram;
    }

    /**
     * @brief obtain the state of high resolution vram from gpu
     */
    const std::array<u16, VRamWidth * VRamHiresScale * VRamHeight * VRamHiresScale>& GPU::meta_get_vram_hires_buffer() const
    {
        return m_vram_hires;
    }

    /**
     * @brief calculate current refresh rate 
     */
    float GPU::meta_refresh_rate() const
    {
        switch(m_display_mode.video_mode)
        {
            case 0: // NTSC
            {
                switch(m_display_mode.vertical_interlace_enabled)
                {
                    case 0: return NTSCWithoutInterlaceRefreshRate;
                    case 1: return NTSCWithInterlaceRefreshRate;
                }
            } break;
            case 1: // PAL
            {
                switch(m_display_mode.vertical_interlace_enabled)
                {
                    case 0: return PALWithoutInterlaceRefreshRate;
                    case 1: return PALWithInterlaceRefreshRate;
                }
            } break;
        }

        UNREACHABLE();
        return 0;
    }

    /**
     * @brief get the current rendering information about the display
     */
    DisplayInfo GPU::meta_get_display_info() const
    {
        u32 width = 0, height = 0;

        switch(m_display_mode.vertical_resolution)
        {
            case 0: { height = 240; } break;
            case 1: { height = 480; } break;
            default: { UNREACHABLE(); } break;
        }

        switch(m_display_mode.horizontal_resolution_2)
        {
            case 1: { width = 368; } break;
            default:
            {
                switch(m_display_mode.horizontal_resolution_1)
                {
                    case 0: { width = 256; } break;
                    case 1: { width = 320; } break;
                    case 2: { width = 512; } break;
                    case 3: { width = 640; } break;
                    default: { UNREACHABLE(); } break;
                }
            }
        }

        // TODO: calculate right chop-off
        float height_fraction = (m_display_range_y_2 - m_display_range_y_1) / float(m_display_mode.video_mode == 0 ? NTSCVerticalResolution : PALVerticalResolution);
        
        return DisplayInfo
        {
            .start_x = m_display_area_start_x,
            .start_y = m_display_area_start_y,
            .width = width,
            .height = u32(height * height_fraction),
            .enabled = !m_display_disable,
            .color_depth = DisplayAreaColorDepth(m_display_mode.display_area_color_depth),
            .resolution = m_meta_resolution
        };
    }

    /**
     * @brief select the rendering target resolution 
     */
    void GPU::meta_set_resolution(RenderTarget resolution)
    {
        m_meta_resolution = resolution;
    }
}
