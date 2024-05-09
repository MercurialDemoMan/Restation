/**
 * @file      PeripheralsTypes.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Types for the PSX Peripherals interfalce
 *
 * @version   0.1
 *
 * @date      15. 03. 2024, 14:50 (created)
 *
 * @section License
 * This file is part of the Restation \n
 *
 * Copyright (C) 2024
 *
 * This file is part of Restation. Restation is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Restation is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Restation. If not, see http://www.gnu.org/licenses/.
 */

#ifndef PERIPHERALSTYPES_HPP
#define PERIPHERALSTYPES_HPP

#include "Types.hpp"

namespace PSX
{
    /**
     * @brief keep track of who are we currently communicating with 
     */
    enum class CurrentlyCommunicatingWith : u8
    {
        None,
        Controller,
        MemoryCard
    };
}

#endif // PERIPHERALSTYPES_HPP