/**
 * @file      GPUTypes.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Definition of GPU specific types
 *
 * @version   0.1
 *
 * @date      6. 11. 2023, 10:54 (created)
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

#ifndef GPUTYPES_HPP
#define GPUTYPES_HPP

#include "Types.hpp"
#include "Macros.hpp"
#include "Utils.hpp"
#include <algorithm>

namespace PSX
{
    /**
     * @brief GP0 instructions
     */
    enum class GPUGP0Instruction
    {
        Nop                 = 0,
        InvalidateClutCache = 1,
        FillVRam            = 2,

        InterruptRequest    = 31,

        PolygonRenderStart  = 32,
        PolygonRenderEnd    = 63,
 
        LineRenderStart     = 64,
        LineRenderEnd       = 95,
 
        RectangleRenderStart = 96,
        RectangleRenderEnd   = 127,
 
        VRamToVRamStart  = 128,
        VRamToVRamEnd    = 159,
 
        CPUToVRamStart   = 160,
        CPUToVRamEnd     = 191,
 
        VRamToCPUStart   = 192,
        VRamToCPUEnd     = 223,

        DrawModeSetting           = 225,
        TextureWindowSetting      = 226,
        SetDrawingAreaTopLeft     = 227,
        SetDrawingAreaBottomRight = 228,
        SetDrawingOffset          = 229,
        MaskBitSetting            = 230,
    };

    /**
     * @brief GP1 instructions
     */
    enum class GPUGP1Instruction
    {
        ResetGPU               = 0,
        ResetCommand           = 1,
        AcknowledgeInterrupt   = 2,
        DisplayEnable          = 3,
        DMADirection           = 4,
        StartOfDisplayArea     = 5,
        HorizontalDisplayRange = 6,
        VerticalDisplayRange   = 7,
        DisplayMode            = 8,
        NewTextureDisable      = 9,
        GetGPUInfoStart        = 16,
        GetGPUInfoEnd          = 31,
        SpecialTextureDisable  = 32
    };

    /**
     * @brief GPU command 
     */
    enum class GPUCommand : u8
    {
        Nop             = 0,
        VRamFill        = 1,
        PolygonRender   = 2,
        LineRender      = 3,
        RectangleRender = 4,
        CopyCPUToVRamParsingPhase = 5,
        CopyCPUToVRamDataPhase    = 6,
        CopyVRamToCPU   = 7,
        CopyVRamToVRam  = 8
    };

    /**
     * @brief GPU current color depth 
     */
    enum class DisplayAreaColorDepth
    {
        Depth15Bit = 0,
        Depth24Bit = 1
    };

    /**
     * @brief GPU 24bit Color 
     */
    union Color24Bit
    {
        Color24Bit()
            : raw(0)
        {

        }

        Color24Bit(u32 raw)
            : raw(raw)
        {

        }

        Color24Bit(u8 r, u8 g, u8 b)
            : r(r),
              g(g),
              b(b)
        {

        }

        struct
        {
            u32 r: 8;
            u32 g: 8;
            u32 b: 8;
 
            u32: 8;
        };
        
        u32 raw;
    };

    /**
     * @brief GPU VRAM 15bit Color 
     */
    union Color15Bit
    {
        Color15Bit() :
            raw(0)
        {

        }

        Color15Bit(u16 raw) :
            raw(raw)
        {
            
        }

        Color15Bit(u8 r, u8 g, u8 b, u8 mask) :
            r(r),
            g(g),
            b(b),
            mask(mask)
        {
            
        }

        static Color15Bit create_from_24bit(Color24Bit color)
        {
            return Color15Bit
            (
                color.r >> 3,
                color.g >> 3,
                color.b >> 3,
                0
            );
        }

        static Color15Bit create_blended(Color15Bit source, Color15Bit destination, u32 transparency_type)
        {
            switch(transparency_type)
            {
                /// half from each color
                case 0:
                {
                    return Color15Bit
                    {
                        static_cast<u8>(std::min(31, (source.r + destination.r) / 2)),
                        static_cast<u8>(std::min(31, (source.g + destination.g) / 2)),
                        static_cast<u8>(std::min(31, (source.b + destination.b) / 2)),
                        static_cast<u8>(destination.mask)
                    };
                }
                /// additive
                case 1:
                {
                    return Color15Bit
                    {
                        static_cast<u8>(std::min(31, source.r + destination.r)),
                        static_cast<u8>(std::min(31, source.g + destination.g)),
                        static_cast<u8>(std::min(31, source.b + destination.b)),
                        static_cast<u8>(destination.mask)
                    };
                }
                /// subtractive
                case 2:
                {
                    return Color15Bit
                    {
                        static_cast<u8>(std::max(0, source.r - destination.r)),
                        static_cast<u8>(std::max(0, source.g - destination.g)),
                        static_cast<u8>(std::max(0, source.b - destination.b)),
                        static_cast<u8>(destination.mask)
                    };
                }
                /// additive/4
                case 3:
                {
                    return Color15Bit
                    {
                        static_cast<u8>(std::min(31, source.r + (destination.r / 4))),
                        static_cast<u8>(std::min(31, source.g + (destination.g / 4))),
                        static_cast<u8>(std::min(31, source.b + (destination.b / 4))),
                        static_cast<u8>(destination.mask)
                    };
                }
            }

            UNREACHABLE();
            return Color15Bit();
        }

        static Color15Bit create_mix(Color24Bit source, Color15Bit destination)
        {
            return Color15Bit
            (
                static_cast<u8>(std::min(31, (source.r * destination.r) >> 7)),
                static_cast<u8>(std::min(31, (source.g * destination.g) >> 7)),
                static_cast<u8>(std::min(31, (source.b * destination.b) >> 7)),
                static_cast<u8>(destination.mask)
            );
        }

        struct
        {
            u16 r: 5;
            u16 g: 5;
            u16 b: 5;
            u16 mask: 1;
        };
        
        u16 raw;
    };

    /**
     * @brief GPU Polygon Vertex 
     */
    struct Vertex
    {
        s16 pos_x;
        s16 pos_y;
        Color24Bit color;
        u16 uv_x;
        u16 uv_y;
    };

    /**
     * @brief Per-fragment attributes
     */
    struct FragmentAttributes
    {
        fixed<s32, 16> r;
        fixed<s32, 16> g;
        fixed<s32, 16> b;
        fixed<s32, 16> u;
        fixed<s32, 16> v;
    };

    /**
     * @brief Per-fragment attribute delta
     */
    struct FragmentAttributeDelta
    {
        fixed<s32, 16> x;
        fixed<s32, 16> y;
    };

    /**
     * @brief Per-fragment attributes deltas
     */
    struct FragmentAttributesDeltas
    {
        FragmentAttributeDelta r;
        FragmentAttributeDelta g;
        FragmentAttributeDelta b;
        FragmentAttributeDelta u;
        FragmentAttributeDelta v;
    };

    /**
     * @brief GPU Texture Info 
     */
    struct TextureInfo
    {
        u32 palette_index { 0 };
        u32 texpage_index { 0 };
        u16 uv_x { 0 };
        u16 uv_y { 0 };
    };

    /**
     * @brief Parse Polygon render command 
     */
    union PolygonRenderCommand
    {
        PolygonRenderCommand(u32 raw) :
            raw(raw)
        {

        }
        
        /**
         * @brief calculate number of arguments needed to render the polygon 
         */
        u32 num_arguments() const
        {
            u32 result = 0;

            if(is_quad) 
                result = 4; 
            else
                result = 3; 

            if(is_texture_mapped) 
                result *= 2;
            
            if(is_gouraud_shaded)
                result += (is_quad ? 4 : 3) - 1;

            return 1 + result;
        }

        /**
         * @brief calculate number of vertices of the current polygon
         */
        u32 num_vertices() const
        {
            return is_quad ? 4 : 3;
        }

        struct
        {
            u32 is_raw_texture:      1;
            u32 is_semi_transparent: 1;
            u32 is_texture_mapped:   1;
            u32 is_quad:             1;
            u32 is_gouraud_shaded:   1;

            u32: 27;
        };

        u32 raw;
    };

    /**
     * @brief Parse Line render command 
     */
    union LineRenderCommand
    {
        LineRenderCommand(u32 raw) :
            raw(raw)
        {

        }
        
        /**
         * @brief calculate number of arguments needed to render the line 
         */
        u32 num_arguments() const
        {
            return 1 + (is_gouraud_shaded ? 3 : 2);
        }

        struct
        {
            u32: 1;

            u32 is_semi_transparent: 1;

            u32: 1;

            u32 is_poly_line:        1;
            u32 is_gouraud_shaded:   1;

            u32: 27;
        };

        u32 raw;
    };

    /**
     * @brief Parse Rectangle render command 
     */
    union RectangleRenderCommand
    {
        RectangleRenderCommand(u32 raw) :
            raw(raw)
        {

        }
        
        /**
         * @brief calculate number of arguments needed to render the rectangle 
         */
        u32 num_arguments() const
        {
            u32 result = 0;

            if(size == 0)
                result = 2;
            else
                result = 1;
            
            return 1 + result + is_texture_mapped;
        }

        /**
         * @brief extract rectangle size 
         */
        u32 actual_size() const
        {
            switch(size)
            {
                case 0: return 0;  /// variable size - specified by arguments
                case 1: return 1;  /// 1x1
                case 2: return 8;  /// 8x8
                case 3: return 16; /// 16x16
            }

            UNREACHABLE();
            return 0;
        }

        struct
        {
            u32 is_raw_texture:      1;
            u32 is_semi_transparent: 1;
            u32 is_texture_mapped:   1;
            u32 size:                2;

            u32: 27;
        };

        u32 raw;
    };

    /**
     * @brief helper for passing arguments to the VRamFill command
     */
    struct VRamFillArguments
    {
        u32 start_x;
        u32 start_y;
        u32 size_x;
        u32 size_y;
        Color15Bit color;
    };

    /**
     * @brief helper for passing arguments to the RectangleRender command
     */
    struct RectangleRenderArguments
    {
        s32 start_x;
        s32 start_y;
        u32 width;
        u32 height;
        Color24Bit color;
        u32 color_depth;
        u32 uv_x;
        u32 uv_y;
        u32 clut_x;
        u32 clut_y;
        u32 texpage_x;
        u32 texpage_y;
        u32 is_semi_transparent;
        u32 is_raw_texture;
    };

    /**
     * @brief helper for passing arguments to the PolygonRender command
     */
    struct PolygonRenderArguments
    {
        Vertex vertex_a;
        Vertex vertex_b;
        Vertex vertex_c;
        u32    color_depth;
        u16    texpage_x;
        u16    texpage_y;
        u16    clut_x;
        u16    clut_y;
        u32    semi_transparency;
        u32    is_raw_texture;
        u32    is_semi_transparent;
        u32    is_gouraud_shaded;
    };

    /**
     * @brief helper for passing arguments to the LineRender command 
     */
    struct LineRenderArguments
    {
        s32 start_x;
        s32 start_y;
        s32 end_x;
        s32 end_y;
        Color24Bit start_color;
        Color24Bit end_color;
        u32 is_semi_transparent;
        u32 is_gouraud_shaded;
    };
}

#endif // GPUTYPES_HPP