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
        if(m_meta_lines >= num_lines)
        {
            m_meta_lines %= num_lines;
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
        m_clut_cache_x               = -1;
        m_clut_cache_y               = -1;
        m_clut_cache_depth           = 0;
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
                vram_fill();
                m_current_command = GPUCommand::Nop;
                break;
            }

            case GPUCommand::PolygonRender:
            {
                polygon_render();
                break;
            }

            case GPUCommand::LineRender:
            {
                line_render();
                break;
            }

            case GPUCommand::RectangleRender:
            {
                rectangle_render();
                m_current_command = GPUCommand::Nop;
                break;
            }

            case GPUCommand::CopyCPUToVRam:
            {
                copy_cpu_to_vram();
                break;
            }

            case GPUCommand::CopyVRamToCPU:
            {
                copy_vram_to_cpu();
                break;
            }

            case GPUCommand::CopyVRamToVRam:
            {
                copy_vram_to_vram();
                break;
            }
        }
    }

    /**
     * @brief Quick VRam rectangle fill GPU Command 
     */
    void GPU::vram_fill()
    {
        // extract arguments
        u32 color   = (m_command_fifo.at(0) & 0x00FF'FFFF);
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
                .color   = Color::create_from_24bit(color)
        });
    }

    /**
     * @brief Perform Quick VRAM rectangle fill GPU Command
     */
    void GPU::do_vram_fill(const VRamFillArguments& args)
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
        RectangleRenderCommand command(m_command_fifo.at(0));

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
        u32 color = m_command_fifo.at(0) & 0x00FF'FFFF;

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
                case 0: { color_bits = 4; }
                // 8bit depth
                case 1: { color_bits = 8; }
                // 15bit depth
                case 2:
                // reserved = 15bit depth
                case 3: { color_bits = 16; }
                default: { UNREACHABLE(); }
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
            .color       = Color::create_from_24bit(color),
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
    }

    /**
     * @brief Perform Render Rectangle GPU Command 
     */
    void GPU::do_rectangle_render(const RectangleRenderArguments& args)
    {
        if(args.width > 1023 || args.height > 511)
            return;

        s32 min_x = clamp_drawing_area_left(args.start_x);
        s32 min_y = clamp_drawing_area_top(args.start_y);

        s32 max_x = clamp_drawing_area_right(args.start_x + args.width);
        s32 max_y = clamp_drawing_area_bottom(args.start_y + args.height);

        s32 uv_x = args.uv_x + (min_x - args.start_x) + m_draw_mode.texture_rect_x_flip;
        s32 uv_y = args.uv_y + (min_y - args.start_y) + m_draw_mode.texture_rect_y_flip;

        s32 dir_x = m_draw_mode.texture_rect_x_flip ? 1 : -1;
        s32 dir_y = m_draw_mode.texture_rect_y_flip ? 1 : -1;

        update_clut_cache(args.color_depth, args.clut_x, args.clut_y);

        for(s32 y = min_y, v = uv_y; y < max_y; y++, v += dir_y)
        {
            for(s32 x = min_x, u = uv_x; x < max_x; x++, u += dir_x)
            {
                // get original color for blending
                Color original_color = Color(m_vram[y * VRamWidth + x]);

                if(m_mask_bit_setting.check_mask_before_draw)
                {
                    if(original_color.mask)
                        continue;
                }

                // create new color
                Color new_color;

                if(args.color_depth == 0)
                {
                    new_color      = args.color;
                    new_color.mask = 0;
                }
                else
                {
                    // fetch texture color
                    new_color = vram_fetch_texture_color(args.color_depth, mask_texture_u(u), mask_texture_v(v), args.texpage_x, args.texpage_y);

                    // mix texture with the rectangle color
                    if(!args.is_raw_texture)
                    {
                        new_color = Color::create_mix(args.color, new_color);
                    }
                }

                // blend if transparent
                if(args.is_semi_transparent)
                {
                    if(new_color.mask || args.color_depth != 0)
                    {
                        new_color = Color::create_blended(original_color, new_color, m_draw_mode.semi_transparency);
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
    void GPU::copy_cpu_to_vram()
    {
        TODO();
    }

    /**
     * @brief Copy VRAM to RAM GPU Command 
     */
    void GPU::copy_vram_to_cpu()
    {
        TODO();
    }

    /**
     * @brief Copy VRAM to VRAM GPU Command 
     */
    void GPU::copy_vram_to_vram()
    {
        TODO();
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

        auto clut_pos_changed   = (m_clut_cache_x.value_or(-1) == clut_x) && 
                                  (m_clut_cache_y.value_or(-1) == clut_y);
        auto clut_depth_changed = m_clut_cache_depth != color_depth;

        // clut position didn't change and texture format didn't change
        if(!clut_pos_changed && !clut_pos_changed)
            return;

        // update clut cache
        m_clut_cache_depth = color_depth;
        m_clut_cache_x     = clut_x;
        m_clut_cache_y     = clut_y;

        u32 num_entries = color_depth == 2 ? 256 : 16;
        for(u32 i = 0; i < num_entries; i++)
        {
            m_clut_cache[i] = m_vram[clut_y * VRamWidth + (clut_x + i)];
        }
    }

    /**
     * @brief fetch texture color 
     */
    Color GPU::vram_fetch_texture_color(u32 color_depth, u32 uv_x, u32 uv_y, u32 texpage_x, u32 texpage_y)
    {
        switch(color_depth)
        {
            // 4bit color depth
            case 1:
            {
                u32 x = (texpage_x + uv_x / 4) % 1024;
                u32 y = (texpage_y + uv_y) % 512;
                u16 clut_cache_index = m_vram[y * VRamWidth + x];
                return Color(m_clut_cache[(clut_cache_index >> ((uv_x & 3) * 4)) & 0x0F]);
            }
            // 8bit color depth
            case 2:
            {
                u32 x = (texpage_x + uv_x / 2) % 1024;
                u32 y = (texpage_y + uv_y) % 512;
                u16 clut_cache_index = m_vram[y * VRamWidth + x];
                return Color(m_clut_cache[(clut_cache_index >> ((uv_x & 1) * 8)) & 0xFF]);
            }
            // 16bit color depth
            case 3:
            {
                u32 x = (texpage_x + uv_x) % 1024;
                u32 y = (texpage_y + uv_y) % 512;
                return Color(m_vram[y * VRamWidth + x]);
            }
        }

        UNREACHABLE();
        return Color();
    }
}