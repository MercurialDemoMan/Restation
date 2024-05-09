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

#include "DMAController.hpp"

#include "Bus.hpp"
#include "Utils.hpp"
#include "Macros.hpp"
#include "DMAChannel.hpp"
#include "DMAChannelGPU.hpp"
#include "DMAChannelSPU.hpp"
#include "DMAChannelOTC.hpp"
#include "DMAChannelPIO.hpp"
#include "DMAChannelCDROM.hpp"
#include "DMAChannelMDECIN.hpp"
#include "DMAChannelMDECOUT.hpp"
#include "InterruptController.hpp"

#include <queue>

namespace PSX
{
    void DMAController::execute(u32 num_steps)
    {
        static auto channel_compare = [this](const std::shared_ptr<DMAChannel>& x, const std::shared_ptr<DMAChannel>& y) -> bool
        {
            u32 x_type     = static_cast<u32>(x->type());
            u32 y_type     = static_cast<u32>(y->type());
            u32 x_priority = (this->m_control.raw >> (x_type * 4)) & 0b111;
            u32 y_priority = (this->m_control.raw >> (y_type * 4)) & 0b111;
            return x_priority < y_priority;
        };

        std::priority_queue<
            std::shared_ptr<DMAChannel>, 
            std::vector<std::shared_ptr<DMAChannel>>,
            decltype(channel_compare)> 
            channel_queue(channel_compare);

        // reorder the channels by their priority in the control register
        for(auto& channel: m_channels)
            channel_queue.push(channel);

        while(!channel_queue.empty())
        {
            // get the top priority channel
            auto channel = channel_queue.top(); 
            u32 channel_index = static_cast<u32>(channel->type());

            // if current channel is enabled
            if((m_control.raw & (0b1000 << (channel_index * 4))) != 0)
            {
                channel->execute(num_steps);
            }
            
            if(channel->meta_interrupt_request())
            {
                channel->meta_interrupt_request() = false;

                // if current channel can interrupt
                if(m_interrupt.raw & (1 << (16 + channel_index)))
                {
                    // set interrupt request flag of the appropriate channel
                    m_interrupt.raw |= (1 << (24 + channel_index));

                    // can we interrupt?
                    u32 dma_enabled   = (m_interrupt.raw & 0x007F'0000) >> 16;
                    u32 irq_requested = (m_interrupt.raw & 0x7F00'0000) >> 24;
                    m_meta_interrupt_request = m_interrupt.force_irq || (m_interrupt.channel_master_enable && (dma_enabled & irq_requested));
                }
            }

            channel_queue.pop();
        }

        // trigger interrupt based on the interrupt register
        if(m_meta_interrupt_request)
        {
            m_meta_interrupt_request = false;
            m_interrupt_controller->trigger_interrupt(Interrupt::DMA);
        }
    }

    u32 DMAController::read(u32 address)
    {
        LOG_DEBUG(5, fmt::format("DMA read 0x{:08x}", address));

        if(in_range(address, 0u, 111u))
        {
            return m_channels[address / 16]->read(address & 0b1111);
        }
        if(in_range(address, 112u, 115u))
        {
            return m_control.bytes[address - 112];
        }
        if(in_range(address, 116u, 119u))
        {
            return m_interrupt.bytes[address - 116];
        }

        UNREACHABLE();
    }

    void DMAController::write(u32 address, u32 value)
    {
        LOG_DEBUG(5, fmt::format("DMA write 0x{:08x}, 0x{:08x}", address, value));

        if(in_range(address, 0u, 111u))
        {
            m_channels[address / 16]->write(address & 0b1111, value); return;
        }
        if(in_range(address, 112u, 115u))
        {
            m_control.bytes[address - 112] = value; return;
        }
        if(in_range(address, 116u, 119u))
        {
            u32 interrupt_address = address - 116;

            if(interrupt_address <= 2) 
                m_interrupt.bytes[interrupt_address] = value; 
            
            if(interrupt_address == 3)
                m_interrupt.bytes[interrupt_address] &= ~value;

            u32 dma_enabled   = (m_interrupt.raw & 0x007F'0000) >> 16;
            u32 irq_requested = (m_interrupt.raw & 0x7F00'0000) >> 24;
            m_interrupt.irq_master_enable = m_interrupt.force_irq || (m_interrupt.channel_master_enable && (dma_enabled & irq_requested));
            
            return;
        }

        UNREACHABLE();
    }

    void DMAController::reset()
    {
        m_control.raw   = 0x07654321;
        m_interrupt.raw = 0;
        m_channels[static_cast<u32>(ChannelType::MDECIN)]  = std::make_shared<DMAChannelMDECIN>(m_bus, m_mdec);
        m_channels[static_cast<u32>(ChannelType::MDECOUT)] = std::make_shared<DMAChannelMDECOUT>(m_bus, m_mdec);
        m_channels[static_cast<u32>(ChannelType::GPU)]     = std::make_shared<DMAChannelGPU>(m_bus, m_gpu);
        m_channels[static_cast<u32>(ChannelType::CDROM)]   = std::make_shared<DMAChannelCDROM>(m_bus, m_cdrom);
        m_channels[static_cast<u32>(ChannelType::SPU)]     = std::make_shared<DMAChannelSPU>(m_bus, m_spu);
        m_channels[static_cast<u32>(ChannelType::PIO)]     = std::make_shared<DMAChannelPIO>(m_bus);
        m_channels[static_cast<u32>(ChannelType::OTC)]     = std::make_shared<DMAChannelOTC>(m_bus);
    
        m_meta_interrupt_request = false;
    }

    void DMAController::serialize(std::shared_ptr<SaveState>& save_state)
    {
        save_state->serialize_from(m_control.raw);
        save_state->serialize_from(m_interrupt.raw);
        save_state->serialize_from(m_meta_interrupt_request);
    }

    void DMAController::deserialize(std::shared_ptr<SaveState>& save_state)
    {
        save_state->deserialize_to(m_control.raw);
        save_state->deserialize_to(m_interrupt.raw);
        save_state->deserialize_to(m_meta_interrupt_request);
    }
}