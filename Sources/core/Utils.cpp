/**
 * @file      Utils.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for bit manipulation and arithmetic utilities
 *
 * @version   0.1
 *
 * @date      12. 2. 2023, 20:39 (created)
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

#include "Utils.hpp"

namespace PSX
{
    /**
     * @brief convert binary-coded decimal to binary number
     */
    u8 bcd_to_binary(u8 bcd)
    {
        return ((bcd >> 4) & 0x0F) * 10 + (bcd & 0x0F);
    }

    /**
     * @brief convert binary number to binary-coded decimal 
     */
    u8 binary_to_bcd(u8 binary)
    {
        return ((binary / 10) << 4) | (binary % 10);
    }
}