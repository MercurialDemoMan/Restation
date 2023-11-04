/**
 * @file      MemControl.cpp
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

#include "MemControl.hpp"

namespace PSX
{
    u32 MemControl::read(u32 address)
    {
        switch(address)
        {
            case  0 ...  3: return m_expansion1_base.read(address -  0);
            case  4 ...  7: return m_expansion1_base.read(address -  4);
            case  8 ... 11: return m_expansion1_base.read(address -  8);
            case 12 ... 15: return m_expansion1_base.read(address - 12);
            case 16 ... 19: return m_expansion1_base.read(address - 16);
            case 20 ... 23: return m_expansion1_base.read(address - 20);
            case 24 ... 27: return m_expansion1_base.read(address - 24);
            case 28 ... 31: return m_expansion1_base.read(address - 28);
        }

        UNREACHABLE();
    }

    void MemControl::write(u32 address, u32 value)
    {
        switch(address)
        {
            case  0 ...  3: m_expansion1_base.write(address -  0, static_cast<u8>(value)); return;
            case  4 ...  7: m_expansion1_base.write(address -  4, static_cast<u8>(value)); return;
            case  8 ... 11: m_expansion1_base.write(address -  8, static_cast<u8>(value)); return;
            case 12 ... 15: m_expansion1_base.write(address - 12, static_cast<u8>(value)); return;
            case 16 ... 19: m_expansion1_base.write(address - 16, static_cast<u8>(value)); return;
            case 20 ... 23: m_expansion1_base.write(address - 20, static_cast<u8>(value)); return;
            case 24 ... 27: m_expansion1_base.write(address - 24, static_cast<u8>(value)); return;
            case 28 ... 31: m_expansion1_base.write(address - 28, static_cast<u8>(value)); return;
        }

        UNREACHABLE();
    }

    void MemControl::reset()
    {
        m_expansion1_base   = 0;
        m_expansion2_base   = 0;
        m_expansion1_config = 0;
        m_expansion2_config = 0;
        m_expansion3_config = 0;
        m_bios_config       = 0;
        m_spu_config        = 0;
        m_cdrom_config      = 0;
    }
}