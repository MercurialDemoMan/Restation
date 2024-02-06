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
#include <algorithm>
#include <fmt/core.h>
#include <glm/glm.hpp>

extern "C"
{
    #include <stb_image_write.h>
}

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

        m_meta_lines  += m_meta_cycles / line_cycles;
        m_meta_cycles %= line_cycles;

        // did we render whole frame?
        if(m_meta_lines >= num_lines)
        {
            m_meta_lines %= num_lines;
            m_meta_frames++;
            m_is_line_odd = false;
            m_interrupt_controller->trigger_interrupt(Interrupt::VBlank);
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
                    break;
                }

                // 480 lines
                case 1:
                {
                    m_is_line_odd = m_meta_frames & 1;
                    break;
                }
            }
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
                execute_gp0_command(value); return;
            }

            case 4 ... 7:
            {
                execute_gp1_command(value); return;
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
        m_clut_cache_x               = {};
        m_clut_cache_y               = {};
        m_clut_cache_depth           = 0;
        m_command_num_arguments      = 0;
        m_meta_cycles                = 0;
        m_meta_lines                 = 0;
        m_meta_frames                = 0;
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
            m_command_fifo.push_back(value);

            u32  instruction_raw = value >> 24;
            auto instruction = GPUGP0Instruction(instruction_raw);

            switch(instruction)
            {
                case GPUGP0Instruction::Nop:
                {
                    m_command_num_arguments = 1;
                    break;
                }

                case GPUGP0Instruction::InvalidateClutCache:
                {
                    m_clut_cache_x = {};
                    m_clut_cache_y = {};
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
                    m_command_num_arguments = PolygonRenderCommand(instruction_raw).num_arguments();
                    break;
                }

                case GPUGP0Instruction::LineRenderStart ... GPUGP0Instruction::LineRenderEnd:
                {
                    m_current_command = GPUCommand::LineRender;
                    m_command_num_arguments = LineRenderCommand(instruction_raw).num_arguments();
                    break;
                }

                case GPUGP0Instruction::RectangleRenderStart ... GPUGP0Instruction::RectangleRenderEnd:
                {
                    m_current_command = GPUCommand::RectangleRender;
                    m_command_num_arguments = RectangleRenderCommand(instruction_raw).num_arguments();
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
                    m_current_command = GPUCommand::CopyCPUToVRamParsingPhase;
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
     * @brief execute GP1 command
     */
    void GPU::execute_gp1_command(u32 command)
    {
        GPUGP1Instruction opcode   = static_cast<GPUGP1Instruction>((command >> 24) & 0b11'1111);
        u32               argument = command & 0xFF'FFFF;

        switch(opcode)
        {
            case GPUGP1Instruction::ResetGPU:
            {
                soft_reset();
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
        });

        // reset command queue
        m_current_command = GPUCommand::Nop;
    }

    /**
     * @brief Perform Quick VRAM rectangle fill GPU Command
     */
    void GPU::do_vram_fill(VRamFillArguments args)
    {
        // fill vram with a color value
        // final row and column is not filled
        for(u32 y = 0; y < args.size_y; y++)
        {
            for(u32 x = 0; x < args.size_x; x++)
            {
                if(x > VRamWidth - 1) UNREACHABLE();
                if(y > VRamHeight - 1) UNREACHABLE();
                m_vram[y * VRamWidth + x] = args.color.raw;
            }
        }
    }

    /**
     * @brief Render Polygon GPU Command 
     */
    void GPU::polygon_render()
    {
        PolygonRenderCommand command(m_command_fifo.at(0) >> 24);

        std::vector<Vertex> vertices;
        TextureInfo         texture_info;

        // preallocate vertices
        for(u32 i = 0; i < command.num_vertices(); i++)
            vertices.push_back(Vertex());

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
                    break;
                }

                // 8bit
                case 1:
                {
                    args.color_depth = 2;
                    break;
                }

                // 15bit
                case 2:
                // reserved
                case 3:
                {
                    args.color_depth = 3;
                    break;
                }

                default: { UNREACHABLE(); }
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
            do_polygon_render(args);
            args.vertex_a = vertices[1];
            args.vertex_b = vertices[2];
            args.vertex_c = vertices[3];
            do_polygon_render(args);
        }
        else
        {
            do_polygon_render(args);
        }

        // reset command queue
        m_current_command = GPUCommand::Nop;
    }

    /**
     * @brief Perform Render Polygon GPU Command
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
        -> fixed<s32, 16>
        {
            float attr_a = cross(vertices[1], vertices[2]) * atrributes[0] - biases[0];
            float attr_b = cross(vertices[2], vertices[0]) * atrributes[1] - biases[1];
            float attr_c = cross(vertices[0], vertices[1]) * atrributes[2] - biases[2];

            return fixed<s32, 16>((attr_a + attr_b + attr_c) / area + 0.5f);
        };

        /**
         * @brief calculate attribute 2D offset 
         */
        auto fragment_attribute_delta = [](s32 area, glm::ivec2 vertices[3], s32 attributes[3])
        -> FragmentAttributeDelta
        {
            return
            {
                fixed<s32, 16>(((vertices[1].y - vertices[2].y) * attributes[0] + 
                                (vertices[2].y - vertices[0].y) * attributes[1] + 
                                (vertices[0].y - vertices[1].y) * attributes[2]) / float(area)),
                fixed<s32, 16>(((vertices[2].x - vertices[1].x) * attributes[0] + 
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
                attributes.r = fixed<s32, 16>(attributes.r.to_float() + deltas.r.x.to_float() * delta);
                attributes.g = fixed<s32, 16>(attributes.g.to_float() + deltas.g.x.to_float() * delta);
                attributes.b = fixed<s32, 16>(attributes.b.to_float() + deltas.b.x.to_float() * delta);
            }

            if(args.color_depth != 0)
            {
                attributes.u = fixed<s32, 16>(attributes.u.to_float() + deltas.u.x.to_float() * delta);
                attributes.v = fixed<s32, 16>(attributes.v.to_float() + deltas.v.x.to_float() * delta);
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
                attributes.r = fixed<s32, 16>(attributes.r.to_float() + deltas.r.y.to_float() * delta);
                attributes.g = fixed<s32, 16>(attributes.g.to_float() + deltas.g.y.to_float() * delta);
                attributes.b = fixed<s32, 16>(attributes.b.to_float() + deltas.b.y.to_float() * delta);
            }

            if(args.color_depth != 0)
            {
                attributes.u = fixed<s32, 16>(attributes.u.to_float() + deltas.u.y.to_float() * delta);
                attributes.v = fixed<s32, 16>(attributes.v.to_float() + deltas.v.y.to_float() * delta);
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
            s32 attr_color_r[3] = { args.vertex_a.color.r, args.vertex_b.color.r, args.vertex_c.color.r };
            s32 attr_color_g[3] = { args.vertex_a.color.g, args.vertex_b.color.g, args.vertex_c.color.g };
            s32 attr_color_b[3] = { args.vertex_a.color.b, args.vertex_b.color.b, args.vertex_c.color.b };

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
                if( (half_space_x.x > 0 || half_space_x.y > 0 || half_space_x.z > 0) &&
                   !(half_space_x.x < 0 || half_space_x.y < 0 || half_space_x.z < 0))
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
                    m_vram[y * VRamWidth + x] = new_color.raw;
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

        TODO();
    }

    /**
     * @brief Render Line GPU Command 
     */
    void GPU::line_render()
    {
        TODO();
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
                case 0: { color_bits = 1; break; }
                // 8bit depth
                case 1: { color_bits = 2; break; }
                // 15bit depth
                case 2:
                // reserved = 15bit depth
                case 3: { color_bits = 3; break; }
                default: { UNREACHABLE(); break; }
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
        });

        // reset command queue
        m_current_command = GPUCommand::Nop;
    }

    /**
     * @brief Perform Render Rectangle GPU Command 
     */
    void GPU::do_rectangle_render(RectangleRenderArguments args)
    {
        if(args.width > 1023 || args.height > 511)
            return;

        update_clut_cache(args.color_depth, args.clut_x, args.clut_y);

        s32 min_x = clamp_drawing_area_left(args.start_x);
        s32 min_y = clamp_drawing_area_top(args.start_y);

        s32 max_x = clamp_drawing_area_right(args.start_x + args.width);
        s32 max_y = clamp_drawing_area_bottom(args.start_y + args.height);

        s32 uv_x = args.uv_x + (min_x - args.start_x) + m_draw_mode.texture_rect_x_flip;
        s32 uv_y = args.uv_y + (min_y - args.start_y) + m_draw_mode.texture_rect_y_flip;

        s32 dir_x = m_draw_mode.texture_rect_x_flip ? 1 : -1;
        s32 dir_y = m_draw_mode.texture_rect_y_flip ? 1 : -1;

        Color15Bit color_15_bit = Color15Bit::create_from_24bit(args.color);

        for(s32 y = min_y, v = uv_y; y < max_y; y++, v += dir_y)
        {
            for(s32 x = min_x, u = uv_x; x < max_x; x++, u += dir_x)
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
                    new_color = vram_fetch_texture_color(args.color_depth, mask_texture_u(u), mask_texture_v(v), args.texpage_x, args.texpage_y);

                    // mix texture with the rectangle color
                    if(!args.is_raw_texture)
                    {
                        new_color = Color15Bit::create_mix(args.color, new_color);
                    }
                }

                // blend if transparent
                if(args.is_semi_transparent)
                {
                    if(new_color.mask || args.color_depth != 0)
                    {
                        new_color = Color15Bit::create_blended(original_color, new_color, m_draw_mode.semi_transparency);
                    }
                }

                // update vram
                new_color.mask |= m_mask_bit_setting.set_mask_while_drawing;
                m_vram[y * VRamWidth + x] = new_color.raw;
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

        // keep the current command as CopyCPUToVRamDataPhase
        // only change it when we reached the end of the DMA
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

        for(u32 y = 0; y < height; y++)
        {
            for(u32 x = 0; x < width; x++)
            {
                auto color = vram_read((source_x + x) % VRamWidth, (source_y + y) % VRamHeight);
                vram_write_with_mask(destination_x + x, destination_y + y, color);
            }
        }

        // reset command queue
        m_current_command = GPUCommand::Nop;
    }

    /**
     * @brief clamp value to the drawing area
     */
    s32 GPU::clamp_drawing_area_left(s32 x) const
    {
        return std::max(std::max(x, 0), static_cast<s32>(m_drawing_area_left));
    }

    /**
     * @brief clamp value to the drawing area 
     */
    s32 GPU::clamp_drawing_area_right(s32 x) const
    {
        return std::min(std::min(x, static_cast<s32>(VRamWidth)), static_cast<s32>(m_drawing_area_right));
    }

    /**
     * @brief clamp value to the drawing area
     */
    s32 GPU::clamp_drawing_area_top(s32 y) const
    {
        return std::max(std::max(y, 0), static_cast<s32>(m_drawing_area_top));
    }

    /**
     * @brief clamp value to the drawing area
     */
    s32 GPU::clamp_drawing_area_bottom(s32 y) const
    {
        return std::min(std::min(y, static_cast<s32>(VRamHeight)), static_cast<s32>(m_drawing_area_bottom));
    }

    /**
     * @brief mask texture coordinate
     */
    s32 GPU::mask_texture_u(s32 u) const
    {
        return ((u % 256) & ~(m_texture_window_setting.texture_window_mask_x * 8)) | 
                ((m_texture_window_setting.texture_window_offset_x & 
                  m_texture_window_setting.texture_window_mask_x) * 8);
    }

    /**
     * @brief mask texture coordinate
     */
    s32 GPU::mask_texture_v(s32 v) const
    {
        return ((v % 256) & ~(m_texture_window_setting.texture_window_mask_y * 8)) | 
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
                u32 x = (texpage_x + uv_x / 4) % 1024;
                u32 y = (texpage_y + uv_y) % 512;
                u16 clut_cache_index = vram_read(x, y);
                return Color15Bit(m_clut_cache[(clut_cache_index >> ((uv_x & 3) * 4)) & 0x0F]);
            }
            // 8bit color depth
            case 2:
            {
                u32 x = (texpage_x + uv_x / 2) % 1024;
                u32 y = (texpage_y + uv_y) % 512;
                u16 clut_cache_index = vram_read(x, y);
                return Color15Bit(m_clut_cache[(clut_cache_index >> ((uv_x & 1) * 8)) & 0xFF]);
            }
            // 16bit color depth
            case 3:
            {
                u32 x = (texpage_x + uv_x) % 1024;
                u32 y = (texpage_y + uv_y) % 512;
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

        m_vram[y * VRamWidth + x] = color | mask;
    }

    /**
     * @brief read color from vram
     */
    u16 GPU::vram_read(u32 x, u32 y) const
    {
        if(x > VRamWidth - 1) UNREACHABLE();
        if(y > VRamHeight - 1) UNREACHABLE();
        return m_vram[y * VRamWidth + x];
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

        color.r = std::clamp(color.r + dither_table[y * 4 + x], 0, 255);
        color.g = std::clamp(color.g + dither_table[y * 4 + x], 0, 255);
        color.b = std::clamp(color.b + dither_table[y * 4 + x], 0, 255);

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
}