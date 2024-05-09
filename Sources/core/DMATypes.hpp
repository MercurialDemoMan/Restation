/**
 * @file      DMATypes.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Definition for the PSX Direct memory access types
 *
 * @version   0.1
 *
 * @date      27. 11. 2023, 17:36 (created)
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

#ifndef DMATYPES_HPP
#define DMATYPES_HPP

namespace PSX
{
    enum class ChannelType
    {
        MDECIN  = 0,
        MDECOUT = 1,
        GPU     = 2,
        CDROM   = 3,
        SPU     = 4,
        PIO     = 5,
        OTC     = 6,
        Size    = 7
    };
}

#endif // DMATYPES_HPP