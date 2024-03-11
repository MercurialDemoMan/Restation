/**
 * @file      MDECInstruction.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX MDEC Instruction/Command
 *
 * @version   0.1
 *
 * @date      05. 03. 2024, 16:51 (created)
 *
 * @section   TODO: replace with actual documentation
 * TODO: documentation text
 *
 * @section License
 * This file is part of the TODO: project \n
 *
 * Copyright (C) 2024
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

#ifndef MDECINSTRUCTION_HPP
#define MDECINSTRUCTION_HPP

#include "Types.hpp"

namespace PSX
{
    /**
     * @brief Instruction of MDEC sent by CPU
     */
    union MDECInstruction
    {
        /**
         * parameters for Set Quantization Tables command 
         */
        struct
        {
            u32 color: 1;
            
            u32: 31;
        };

        /**
         * parameters for Decode Macroblock command 
         */
        struct
        {
            u32 num_arguments:     16;

            u32: 9;
            
            u32 data_output_bit15:  1;
            u32 data_output_signed: 1;
            u32 data_output_depth:  3;

            u32: 3;
        };

        /**
         * parameters for parsing instruction 
         */
        struct
        {
            u32 data:   29;
            u32 opcode: 3;
        };

        u32 raw;
    };
}

#endif // MDECINSTRUCTION_HPP