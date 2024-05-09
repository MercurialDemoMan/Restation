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

#ifndef GPUTIMINGS_HPP
#define GPUTIMINGS_HPP

#include "Types.hpp"

namespace PSX
{
    /**
     * GPU screen constants 
     */
    constexpr const u32 PALScanlines  = 314;
    constexpr const u32 NTSCScanlines = 263;
    constexpr const u32 PALVerticalResolution  = 256;
    constexpr const u32 NTSCVerticalResolution = 240;
    constexpr const u32 PALScanlineCycles  = 3406;
    constexpr const u32 NTSCScanlineCycles = 3413;

    /**
     * GPU internal constants 
     */
    constexpr const u32 VRamWidth      = 1024;
    constexpr const u32 VRamHeight     = 512;
    constexpr const u32 VRamHiresScale = 2;
    constexpr const u32 GPUFIFOMaxSize = 32;
    constexpr const u32 ClutCacheSize  = 256;

    /**
     * GPU timings 
     */
    constexpr const u32 DotClockStep256 = 10;
    constexpr const u32 DotClockStep320 = 8;
    constexpr const u32 DotClockStep368 = 7;
    constexpr const u32 DotClockStep512 = 5;
    constexpr const u32 DotClockStep640 = 4;
    constexpr const float NTSCWithInterlaceRefreshRate    = 59.940f;
    constexpr const float NTSCWithoutInterlaceRefreshRate = 59.926f;
    constexpr const float PALWithInterlaceRefreshRate     = 50.000f;
    constexpr const float PALWithoutInterlaceRefreshRate  = 49.761f;
}

#endif // GPUTIMINGS_HPP