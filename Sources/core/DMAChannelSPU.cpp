/**
 * @file      DMAChannelSPU.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for the PSX 5th DMA Channel for accessing SPU
 *
 * @version   0.1
 *
 * @date      28. 11. 2023, 11:36 (created)
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
#include "DMAChannelSPU.hpp"
#include "Bus.hpp"
#include "SPU.hpp"

namespace PSX
{
    /**
     * @brief read from SPU
     */
    u32 DMAChannelSPU::read_from_component()
    {
        constexpr const u32 SRAMAccessCommand = 0x01A8;
        return (m_spu->read(SRAMAccessCommand) <<  0) |
               (m_spu->read(SRAMAccessCommand) <<  8) |
               (m_spu->read(SRAMAccessCommand) << 16) |
               (m_spu->read(SRAMAccessCommand) << 24);
    }

    /**
     * @brief write to SPU
     */
    void DMAChannelSPU::write_to_component(u32 value)
    {
        constexpr const u32 SRAMAccessCommand = 0x01A8;
        m_spu->write(SRAMAccessCommand, (value >>  0) & 0xFF);
        m_spu->write(SRAMAccessCommand, (value >>  8) & 0xFF); 
        m_spu->write(SRAMAccessCommand, (value >> 16) & 0xFF);
        m_spu->write(SRAMAccessCommand, (value >> 24) & 0xFF);
    }
}