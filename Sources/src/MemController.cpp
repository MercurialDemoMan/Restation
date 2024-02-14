/**
 * @file      MemController.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for the PSX Memory Controller
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

#include "MemController.hpp"

namespace PSX
{
    u32 MemController::read(u32 address)
    {
        if(in_range(address,  0u,  3u)) return m_expansion1_base.read(address - 0);
        if(in_range(address,  4u,  7u)) return m_expansion2_base.read(address - 4);
        if(in_range(address,  8u, 11u)) return m_expansion1_size.read(address - 8);
        if(in_range(address, 12u, 15u)) return m_expansion3_size.read(address - 12);
        if(in_range(address, 16u, 19u)) return m_bios_size.read(address - 16);
        if(in_range(address, 20u, 23u)) return m_spu_size.read(address - 20);
        if(in_range(address, 24u, 27u)) return m_cdrom_size.read(address - 24);
        if(in_range(address, 28u, 31u)) return m_expansion2_size.read(address - 28);
        if(in_range(address, 32u, 35u)) return m_common_delay.read(address - 32);

        UNREACHABLE();
    }

    void MemController::write(u32 address, u32 value)
    {
        if(in_range(address,  0u,  3u))
        {
            m_expansion1_base.write(address - 0, static_cast<u8>(value)); 
            return;
        }
        if(in_range(address,  4u,  7u))
        {
            m_expansion2_base.write(address - 4, static_cast<u8>(value)); 
            return;
        }
        if(in_range(address,  8u, 11u))
        {
            m_expansion1_size.write(address - 8, static_cast<u8>(value)); 
            return;
        }
        if(in_range(address, 12u, 15u))
        {
            m_expansion3_size.write(address - 12, static_cast<u8>(value)); 
            return;
        }
        if(in_range(address, 16u, 19u))
        {
            m_bios_size.write(address - 16, static_cast<u8>(value)); 
            return;
        }
        if(in_range(address, 20u, 23u))
        {
            m_spu_size.write(address - 20, static_cast<u8>(value)); 
            return;
        }
        if(in_range(address, 24u, 27u))
        {
            m_cdrom_size.write(address - 24, static_cast<u8>(value)); 
            return;
        }
        if(in_range(address, 28u, 31u))
        {
            m_expansion2_size.write(address - 28, static_cast<u8>(value)); 
            return;
        }
        if(in_range(address, 32u, 35u))
        {
            m_common_delay.write(address - 32, static_cast<u8>(value)); 
            return;
        }

        UNREACHABLE();
    }

    void MemController::reset()
    {
        m_expansion1_base = 0;
        m_expansion2_base = 0;
        m_expansion1_size = 0;
        m_expansion2_size = 0;
        m_expansion3_size = 0;
        m_bios_size       = 0;
        m_spu_size        = 0;
        m_cdrom_size      = 0;
        m_common_delay    = 0;
    }
}