/**
 * @file      RamControl.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for the PSX RAM Controller
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

#include "RamControl.hpp"

namespace PSX
{
    u32 RamControl::read(u32 address)
    {
        switch(address)
        {
            case  0 ... 3: return m_ram_size.read(address);
        }

        UNREACHABLE();
    }

    void RamControl::write(u32 address, u32 value)
    {
        switch(address)
        {
            case  0 ... 3: m_ram_size.write(address, static_cast<u8>(value)); return;
        }

        UNREACHABLE();
    }

    void RamControl::reset()
    {
        m_ram_size.raw() = 0x00000B88;
    }
}