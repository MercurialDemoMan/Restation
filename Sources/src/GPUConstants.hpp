/**
 * @file      GPUConstants.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Definition of GPU constants and timings
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

#ifndef GPUTIMINGS_HPP
#define GPUTIMINGS_HPP

#include "Types.hpp"

namespace PSX
{
    /**
     * GPU screen constants 
     */
    constexpr const u32 PALScanlines  = 314;
    constexpr const u32 NTSCScanlines = 264;
    constexpr const u32 PALScanlineCycles  = 3406;
    constexpr const u32 NTSCScanlineCycles = 3413;

    /**
     * GPU internal constants 
     */
    constexpr const u32 VRamWidth   = 1024;
    constexpr const u32 VRamHeight  = 512;
    constexpr const u32 GPUFIFOSize = 32;

    /**
     * GPU timings 
     */
    constexpr const u32 DotClockStep256 = 10;
    constexpr const u32 DotClockStep320 = 8;
    constexpr const u32 DotClockStep368 = 7;
    constexpr const u32 DotClockStep512 = 5;
    constexpr const u32 DotClockStep640 = 4;
}

#endif // GPUTIMINGS_HPP