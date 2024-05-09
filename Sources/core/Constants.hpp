/**
 * @file      Constants.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Data units constants
 *
 * @version   0.1
 *
 * @date      25. 10. 2023, 20:39 (created)
 *
 * @section License
 * This file is part of the Restation \n
 *
 * Copyright (C) 2023
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

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include "Types.hpp"

namespace PSX
{
    /**
     * data units
     */
    constexpr const u32 KiB = 1024;
    constexpr const u32 MiB = KiB * KiB;
    constexpr const u32 GiB = MiB * MiB;
}

#endif // CONSTANTS_HPP