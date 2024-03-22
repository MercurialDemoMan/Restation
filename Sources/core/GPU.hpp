/**
 * @file      GPU.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX Graphics processing unit
 *
 * @version   0.1
 *
 * @date      26. 10. 2023, 16:19 (created)
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

#ifndef GPU_HPP
#define GPU_HPP

#include <memory>
#include <array>
#include <queue>
#include <optional>
#include "Component.hpp"
#include "Forward.hpp"
#include "GPUConstants.hpp"
#include "GPUTypes.hpp"
#include <glm/glm.hpp>

namespace PSX
{
    /**
     * @brief PSX Graphics processing unit
     */
    class GPU final : public Component
    {
    public:

        GPU(const std::shared_ptr<Bus>& bus, 
            const std::shared_ptr<InterruptController>& interrupt_controller) :
            m_bus(bus),
            m_interrupt_controller(interrupt_controller)
        {
            reset();
        }
        
        virtual ~GPU() override = default;

        virtual void execute(u32 num_steps) override;
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;
        virtual void serialize(std::shared_ptr<SaveState>&) override;
        virtual void deserialize(std::shared_ptr<SaveState>&) override;

        /**
         * @brief dump VRAM into a image file 
         */
        void meta_dump_vram() const;

        /**
         * @brief calculate current refresh rate 
         */
        float meta_refresh_rate() const;

        /**
         * @brief get the current rendering information about the display
         */
        DisplayInfo meta_get_display_info() const;

        /**
         * @brief obtain the state of vram from gpu
         */
        const std::array<u16, VRamWidth * VRamHeight>& meta_get_vram_buffer() const;

    private:

        /**
         * @brief soft reset made by GP1 command 
         */
        void soft_reset();

        /**
         * @brief accumulate information from registers into the GPUSTAT register
         */
        u32 read_stat();

        /**
         * @brief read from vram into the GPUREAD register
         */
        u32 read_vram();

        /**
         * @brief execute GP0 command
         */
        void execute_gp0_command(u32 command);

        /**
         * @brief execute GP1 command
         */
        void execute_gp1_command(u32 command);

        /**
         * @brief execute drawing/copy command
         */
        void execute_gpu_command();

        /**
         * @brief Quick VRAM rectangle fill GPU Command parsing
         */
        void vram_fill();

        /**
         * @brief Perform Quick VRAM rectangle fill GPU Command
         */
        void do_vram_fill(VRamFillArguments);

        /**
         * @brief Render Polygon GPU Command 
         */
        void polygon_render();

        /**
         * @brief Perform Render Polygon GPU Command
         */
        void do_polygon_render(PolygonRenderArguments);

        /**
         * @brief Render Line GPU Command 
         */
        void line_render();

        /**
         * @brief Perform Render Line GPU Command 
         */
        void do_line_render(LineRenderArguments);

        /**
         * @brief Render Rectangle GPU Command parsing
         */
        void rectangle_render();

        /**
         * @brief Perform Render Rectangle GPU Command 
         */
        void do_rectangle_render(RectangleRenderArguments);

        /**
         * @brief Copy RAM to VRAM GPU Command 
         */
        void copy_cpu_to_vram_parsing_phase();

        /**
         * @brief Copy RAM to VRAM GPU Command 
         */
        void copy_cpu_to_vram_data_phase();

        /**
         * @brief Copy VRAM to RAM GPU Command 
         */
        void copy_vram_to_cpu();

        /**
         * @brief Copy VRAM to VRAM GPU Command 
         */
        void copy_vram_to_vram();

        /**
         * @brief clamp value to the drawing area
         */
        s32 clamp_drawing_area_left(s32 x) const;

        /**
         * @brief clamp value to the drawing area 
         */
        s32 clamp_drawing_area_right(s32 x) const;

        /**
         * @brief clamp value to the drawing area
         */
        s32 clamp_drawing_area_top(s32 y) const;

        /**
         * @brief clamp value to the drawing area
         */
        s32 clamp_drawing_area_bottom(s32 y) const;

        /**
         * @brief mask texture coordinate
         */
        s32 mask_texture_u(s32 u) const;

        /**
         * @brief mask texture coordinate
         */
        s32 mask_texture_v(s32 v) const;

        /**
         * @brief update clut cache
         */
        void update_clut_cache(u32 color_depth, u32 clut_x, u32 clut_y);

        /**
         * @brief fetch texture color 
         */
        Color15Bit vram_fetch_texture_color(u32 color_depth, s32 uv_x, s32 uv_y, s32 texpage_x, s32 texpage_y);

        /**
         * @brief mask dma copy parameters 
         */
        u32 mask_dma_x(u32 x) const;

        /**
         * @brief mask dma copy parameters 
         */
        u32 mask_dma_y(u32 y) const;

        /**
         * @brief mask dma copy parameters 
         */
        u32 mask_dma_width(u32 width) const;

        /**
         * @brief mask dma copy parameters 
         */
        u32 mask_dma_height(u32 height) const;

        /**
         * @brief write to vram with respect to masking (the highest bit of the 16bit half word)
         *        since the vram contains colors with 15bit color depth the 16th bit can be used
         *        as a mask, whether it is allowed to draw into that specific vram space
         */
        void vram_write_with_mask(u32 x, u32 y, u16 color);

        /**
         * @brief read color from vram with bounds check
         */
        u16 vram_read(u32 x, u32 y) const;

        /**
         * @brief write color into vram with bounds check 
         */
        void vram_write(u32 x, u32 y, u16 value);

        /**
         * @brief dither color if enabled in texpage attribute
         *        to make 16bit colors look nicer
         */
        Color24Bit dither(Color24Bit, u32 x, u32 y);

        /**
         * @brief connections
         */
        std::shared_ptr<Bus> m_bus;
        std::shared_ptr<InterruptController> m_interrupt_controller;

        /**
         * @brief GP0(0xE1) Draw Mode setting register (Texpage)
         */
        union DrawMode
        {
            struct
            {
                u32 texture_page_x_base:   4; // N * 64
                u32 texture_page_y_base_1: 1; // N * 256
                u32 semi_transparency:     2;
                u32 texture_page_colors:   2;
                u32 dither_24_to_15:       1;
                u32 drawing_to_display_area_allowed: 1;
                u32 texture_page_y_base_2: 1; // N * 512
                u32 texture_rect_x_flip:   1;
                u32 texture_rect_y_flip:   1;
                
                u32: 10;

                u32 ins: 8;
            };

            u32 raw;
            u8  bytes[sizeof(u32)];
        };

        /**
         * @brief GP0(0xE2) Texture Window setting register
         */
        union TextureWindowSetting
        {
            struct
            {
                u32 texture_window_mask_x: 5;
                u32 texture_window_mask_y: 5;
                u32 texture_window_offset_x: 5;
                u32 texture_window_offset_y: 5;
                
                u32: 4;

                u32 ins: 8;
            };

            u32 raw;
            u8  bytes[sizeof(u32)];
        };

        /**
         * @brief GP0(0xE6) Mask Bit Setting register
         */
        union MaskBitSetting
        {
            struct
            {
                u32 set_mask_while_drawing: 1;
                u32 check_mask_before_draw: 1;
                
                u32: 22;

                u32 ins: 8;
            };

            u32 raw;
            u8  bytes[sizeof(u32)];
        };

        /**
         * @brief GP1(0x08) Display mode register
         */
        union DisplayMode
        {
            struct
            {
                u32 horizontal_resolution_1:    2;
                u32 vertical_resolution:        1;
                u32 video_mode:                 1; // 0 - NTSC, 1 - PAL
                u32 display_area_color_depth:   1; // 0 - 15bit, 1 - 24bit
                u32 vertical_interlace_enabled: 1;
                u32 horizontal_resolution_2:    1;
                u32 reverse_flag:               1;

                u32: 24;
            };

            u32 raw;
            u8  bytes[sizeof(u32)];
        };

        /**
         * GPU state 
         */
        DrawMode             m_draw_mode;              /// GP0(0xE1)
        TextureWindowSetting m_texture_window_setting; /// GP0(0xE2)
        u16                  m_drawing_area_top;       /// GP0(0xE3)
        u16                  m_drawing_area_left;      /// GP0(0xE3)
        u16                  m_drawing_area_right;     /// GP0(0xE4)
        u16                  m_drawing_area_bottom;    /// GP0(0xE4)
        s16                  m_drawing_offset_x;       /// GP0(0xE5)
        s16                  m_drawing_offset_y;       /// GP0(0xE5)
        MaskBitSetting       m_mask_bit_setting;       /// GP0(0xE6)   
        bool                 m_interrupt_request;      /// GP1(0x02) did GPU request interrupt     
        bool                 m_display_disable;        /// GP1(0x03)
        u32                  m_dma_direction;          /// GP1(0x04) DMA transfering direction
        u16                  m_display_area_start_x;   /// GP1(0x05)
        u16                  m_display_area_start_y;   /// GP1(0x05)
        u16                  m_display_range_x_1;      /// GP1(0x06)
        u16                  m_display_range_x_2;      /// GP1(0x06)
        u16                  m_display_range_y_1;      /// GP1(0x07)
        u16                  m_display_range_y_2;      /// GP1(0x07)
        DisplayMode          m_display_mode;           /// GP1(0x08)
        bool                 m_new_texture_disable;    /// GP1(0x09)
        u32                  m_read_mode;              /// 0 - read from register, 1 - read from vram
        u32                  m_read_register;          
        u32                  m_dma_start_x;
        u32                  m_dma_start_y;
        u32                  m_dma_end_x;
        u32                  m_dma_end_y;
        u32                  m_dma_current_x;
        u32                  m_dma_current_y;
        bool                 m_ready_to_receive_dma_block;
        bool                 m_is_line_odd;
        GPUCommand           m_current_command;
        fixed_queue<u32, GPUFIFOMaxSize> m_command_fifo;
        u32                  m_command_num_arguments;
        std::optional<u32>   m_clut_cache_x;
        std::optional<u32>   m_clut_cache_y;
        u32                  m_clut_cache_depth;

        /**
         * meta state 
         */
        u32 m_meta_cycles;
        u32 m_meta_lines;
        u32 m_meta_frames;

        /**
         * GPU memory regions 
         */
        std::array<u16, VRamWidth * VRamHeight> m_vram;
        std::array<u16, ClutCacheSize>          m_clut_cache;
    };
}

#endif // GPU_HPP