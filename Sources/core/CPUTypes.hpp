/**
 * @file      CPUTypes.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Types for the PSX MIPS R3000A CPU
 *
 * @version   0.1
 *
 * @date      15. 03. 2024, 14:50 (created)
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

#ifndef CPUTYPES_HPP
#define CPUTYPES_HPP

#include "Types.hpp"
#include "CPUInstruction.hpp"

namespace PSX
{
    /**
     * @brief structure for keeping track of load delays when setting a register 
     */
    struct LoadDelaySlot
    {
        u32 register_id;
        u32 value;
    };

    /**
     * @brief structure for keeping track of past executed instructions 
     */
    struct ExecutedInstruction
    {
        u64            cycles;
        u32            address;
        CPUInstruction ins;
    };

    /**
     * @brief enumeration for indexing load delay slots
     */
    enum LoadDelaySlotIndex
    {
        Current = 0,
        Next    = 1
    };
}

#endif // CPUTYPES_HPP