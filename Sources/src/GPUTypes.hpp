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
    enum class GPUCommand
    {
        Nop             = 0,
        VRamFill        = 1,
        PolygonRender   = 2,
        LineRender      = 3,
        RectangleRender = 4,
        CopyCPUToVRam   = 5,
        CopyVRamToCPU   = 6,
        CopyVRamToVRam  = 7
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
}

#endif // GPUTYPES_HPP