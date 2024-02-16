/**
 * @file      CacheControl.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for the PSX Cache Controller
 *
 * @version   0.1
 *
 * @date      4. 11. 2023, 12:56 (created)
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

#include "CacheController.hpp"

namespace PSX
{
    u32 CacheController::read(u32 address)
    {
        if(in_range(address, 0u, 3u))
        {
            return m_cache_config.read(address - 0);
        }
        
        UNREACHABLE();
    }

    void CacheController::write(u32 address, u32 value)
    {
        if(in_range(address, 0u, 3u))
        {
            m_cache_config.write(address - 0, value); return;
        }

        UNREACHABLE();
    }

    void CacheController::reset()
    {
        m_cache_config.raw() = 0;
    }
}