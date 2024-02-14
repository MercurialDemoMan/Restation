/**
 * @file      GTEInstruction.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX Geometry Transformation Engine Instruction
 *
 * @version   0.1
 *
 * @date      9. 11. 2023, 10:12 (created)
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

#ifndef GTEINSTRUCTION_HPP
#define GTEINSTRUCTION_HPP

#include "Types.hpp"

namespace PSX
{
    /**
     * @brief PSX Geometry Transformation Engine Instruction
     */
    union GTEInstruction
    {
        GTEInstruction() :
            raw(0)
        {
            
        }

        GTEInstruction(u32 value) :
            raw(value)
        {
            
        }

        struct
        {
            u32 opcode:         6; // operation code
            u32:                4;
            u32 lm:             1; // saturate IR1, IR2 and IR3 result
            u32:                2;
            u32 trans_vector:   2; // 0 - TR, 1 - BK, 3 - FC, 3 - None
            u32 mul_vector:     2; // 0 - V0, 1 - V1, 2 - V2, 3 - IR
            u32 mul_matrix:     2; // 0 - rotation, 1 - light, 2 - color, 3 - reserved
            u32 shift_fraction: 1; // shift fraction in IR registers
            u32: 12;
        };
        

        u32 raw; // raw representation of the instruction
    };

    /*
     * make sure instruction is exactly 32-bits long 
     */
    static_assert(sizeof(GTEInstruction) == sizeof(u32), "GTEInstruction has to be 4 bytes long");

    /*
     * make sure bit fields are correctly aligned
     */
    static_assert(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__, "big endian architecture is unsupported");
}

#endif // GTEINSTRUCTION_HPP