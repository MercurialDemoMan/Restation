/**
 * @file      DMAController.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation of the PSX Direct memory access controller
 *
 * @version   0.1
 *
 * @date      26. 10. 2023, 16:20 (created)
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

#include "DMAController.hpp"
#include "Bus.hpp"
#include "DMAChannel.hpp"
#include "DMAChannelMDECIN.hpp"
#include "DMAChannelMDECOUT.hpp"

namespace PSX
{
    void DMAController::execute(u32 num_steps)
    {
        MARK_UNUSED(num_steps);
        TODO();
    }

    u32 DMAController::read(u32 address)
    {
        switch(address)
        {
            case 0 ... 111:
            {
                return m_channels[address / 16]->read(address & 0b1111);
            }
            case 112 ... 116:
            {
                return m_control.bytes[address - 112];
            }
            case 117 ... 121:
            {
                return m_control.bytes[address - 117];
            }
        }

        UNREACHABLE();
    }

    void DMAController::write(u32 address, u32 value)
    {
        switch(address)
        {
            case 0 ... 111:
            {
                m_channels[address / 16]->write(address & 0b1111, value); return;
            }
            case 112 ... 116:
            {
                m_control.bytes[address - 112] = value; return;
            }
            case 117 ... 121:
            {
                m_control.bytes[address - 117] = value; return;
            }
        }

        UNREACHABLE();
    }

    void DMAController::reset()
    {
        m_control.raw   = 0x07654321;
        m_interrupt.raw = 0;
        m_channels[static_cast<u32>(ChannelType::MDECIN)]  = std::make_shared<DMAChannelMDECIN>(m_bus, m_mdec);
        m_channels[static_cast<u32>(ChannelType::MDECOUT)] = std::make_shared<DMAChannelMDECOUT>(m_bus, m_mdec);
        m_channels[static_cast<u32>(ChannelType::GPU)]     = std::make_shared<DMAChannelMDECOUT>(m_bus, m_gpu);
        m_channels[static_cast<u32>(ChannelType::CDROM)]   = std::make_shared<DMAChannelMDECOUT>(m_bus, m_cdrom);
        m_channels[static_cast<u32>(ChannelType::SPU)]     = std::make_shared<DMAChannelMDECOUT>(m_bus, m_spu);
        m_channels[static_cast<u32>(ChannelType::PIO)]     = std::make_shared<DMAChannelMDECOUT>(m_bus);
        m_channels[static_cast<u32>(ChannelType::OTC)]     = std::make_shared<DMAChannelMDECOUT>(m_bus);
        TODO();
    }
}