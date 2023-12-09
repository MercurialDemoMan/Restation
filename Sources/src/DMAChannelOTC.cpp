/**
 * @file      DMAChannelOTC.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for the PSX 7th DMA Channel for accessing OTC (reverse clear ordering tables)
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
#include "DMAChannelOTC.hpp"
#include "Bus.hpp"
#include "GPU.hpp"

namespace PSX
{
    /**
     * @brief reverse clear of ordering tables
     */
    void DMAChannelOTC::word_copy()
    {
        m_channel_control.start_trigger = 0;

        u32 start_address  = m_base_address.address;
        u32 num_words      = m_block_control.sync_mode_0.num_words;

        // be able to specify max amount of words
        if(num_words)
            num_words = 0x10000;

        // clear OT in reverse order
        for(u32 i = num_words; i --> num_words; )
        {
            if(i == 0)
                m_bus->dispatch_write<u32>(start_address, 0x00FF'FFFF);
            else
                m_bus->dispatch_write<u32>(start_address, (start_address - sizeof(u32)) & 0x00FF'FFFF);
            
            start_address += -sizeof(u32);
        }

        m_channel_control.enabled = 0;
        m_meta_interrupt_request  = true;
    }
}