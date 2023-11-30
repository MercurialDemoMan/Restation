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

namespace PSX
{
    void GPU::execute(u32 num_steps)
    {
        MARK_UNUSED(num_steps);
        TODO();
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
        m_display_disable            = true;
        m_interrupt_request          = false;
        m_ready_to_receive_dma_block = 1;
        m_is_frame_odd               = false;
        TODO();
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
            case 1: { TODO(); break; }
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
        TODO(); // 26bit
        result     |= (m_read_mode == 1) << 27; // ready to send vram to cpu
        result     |= m_ready_to_receive_dma_block << 28;
        result     |= m_dma_direction << 29;
        result     |= m_is_frame_odd << 31;
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
}