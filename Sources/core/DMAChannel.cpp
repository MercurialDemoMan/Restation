/**
 * @file      DMAChannel.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for the PSX Direct memory access channel
 *
 * @version   0.1
 *
 * @date      27. 11. 2023, 17:36 (created)
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

#include "DMAChannel.hpp"

#include "Bus.hpp"
#include "Utils.hpp"
#include "Macros.hpp"

namespace PSX
{
    void DMAChannel::execute(u32 num_steps)
    {
        MARK_UNUSED(num_steps);

        if(!m_channel_control.enabled)
            return;

        if(m_channel_control.sync_mode     == 0 && // block mode
           m_channel_control.start_trigger != 1)   // manual
            return;

        switch(m_channel_control.sync_mode)
        {
            case 0: { word_copy();        break; }
            case 1: { block_copy();       break; }
            case 2: { linked_list_copy(); break; }
            case 3: { UNREACHABLE();      break; }
        }
    }

    u32 DMAChannel::read(u32 address)
    {
        if(in_range(address, 0u, 3u))
        {
            return m_base_address.bytes[address - 0];
        }
        if(in_range(address, 4u, 7u))
        {
            return m_block_control.bytes[address - 4];
        }
        if(in_range(address, 8u, 11u))
        {
            return m_channel_control.bytes[address - 8];
        }

        UNREACHABLE();
    }

    void DMAChannel::write(u32 address, u32 value)
    {
        if(in_range(address, 0u, 3u))
        {
            m_base_address.bytes[address - 0] = value; return;
        }
        if(in_range(address, 4u, 7u))
        {
            m_block_control.bytes[address - 4] = value; return;
        }
        if(in_range(address, 8u, 11u))
        {
            m_channel_control.bytes[address - 8] = value;

            mask_channel_control_register();

            if(m_channel_control.enabled)
            {
                execute(1);
            }

            return;
        }

        UNREACHABLE();
    }

    void DMAChannel::reset()
    {
        m_base_address.raw    = 0;
        m_block_control.raw   = 0;
        m_channel_control.raw = 0;
        m_meta_interrupt_request = false;
    }

    void DMAChannel::serialize(std::shared_ptr<SaveState>& save_state)
    {
        save_state->serialize_from(m_base_address.raw);
        save_state->serialize_from(m_block_control.raw);
        save_state->serialize_from(m_channel_control.raw);
        save_state->serialize_from(m_meta_interrupt_request);
    }

    void DMAChannel::deserialize(std::shared_ptr<SaveState>& save_state)
    {
        save_state->deserialize_to(m_base_address.raw);
        save_state->deserialize_to(m_block_control.raw);
        save_state->deserialize_to(m_channel_control.raw);
        save_state->deserialize_to(m_meta_interrupt_request);
    }
    
    /**
     * @brief read from managed component
     */
    u32 DMAChannel::read_from_component()
    {
        TODO(); return 0;
    }

    /**
     * @brief write to managed component
     */
    void DMAChannel::write_to_component(u32 value)
    {
        MARK_UNUSED(value);
        TODO();
    }

    /**
     * @brief perform DMA in word mode 
     */
    void DMAChannel::word_copy()
    {
        m_channel_control.start_trigger = 0;

        u32 start_address  = m_base_address.address;
        u32 num_words      = m_block_control.sync_mode_0.num_words;
        s32 step_direction = m_channel_control.memory_address_step ? -s32(sizeof(u32)) : s32(sizeof(u32));

        // be able to specify max amount of words
        if(num_words == 0)
            num_words = 0x10000;

        switch(m_channel_control.transfer_direction)
        {
            // to RAM
            case 0:
            {
                for(u32 i = 0; i < num_words; i++)
                {
                    m_bus->dispatch_write<u32>(start_address, read_from_component());
                    start_address += step_direction;
                }
            } break;

            // from RAM
            case 1:
            {
                for(u32 i = 0; i < num_words; i++)
                {
                    write_to_component(m_bus->dispatch_read<u32>(start_address));
                    start_address += step_direction;
                }
            } break;
        }

        m_channel_control.enabled = 0;
        m_meta_interrupt_request  = true;
    }

    /**
     * @brief perform DMA in block mode 
     */
    void DMAChannel::block_copy()
    {
        //TODO: remove this?
        m_channel_control.start_trigger = 0;

        if(!sync_request())
            return;

        u32 start_address = m_base_address.address;
        u32 block_size    = m_block_control.sync_mode_1.block_size;
        u32 num_blocks    = m_block_control.sync_mode_1.num_blocks;
        s32 step_direction = m_channel_control.memory_address_step ? -s32(sizeof(u32)) : s32(sizeof(u32));
        
        do
        {
            switch(m_channel_control.transfer_direction)
            {
                // to RAM
                case 0:
                {
                    for(u32 i = 0; i < block_size; i++)
                    {
                        m_bus->dispatch_write<u32>(start_address, read_from_component());
                        start_address += step_direction;
                    }
                } break;

                // from RAM
                case 1:
                {
                    for(u32 i = 0; i < block_size; i++)
                    {
                        write_to_component(m_bus->dispatch_read<u32>(start_address));
                        start_address += step_direction;
                    }
                } break;
            }

            // update address
            m_base_address.address = start_address;

            // end copy
            if(--num_blocks == 0)
            {
                m_channel_control.enabled = 0;
                m_meta_interrupt_request  = true;
            }

        } while(!interleaved_copy() && num_blocks > 0);

        m_block_control.sync_mode_1.num_blocks = num_blocks;
    }

    /**
     * @brief perform DMA in linked list mode 
     */
    void DMAChannel::linked_list_copy()
    {
        //TODO: remove this?
        m_channel_control.start_trigger = 0;

        u32 start_address  = m_base_address.address;
        s32 step_direction = m_channel_control.memory_address_step ? -s32(sizeof(u32)) : s32(sizeof(u32));

        while(true)
        {
            // parse node header
            u32 node_header = m_bus->dispatch_read<u32>(start_address);
            u32 num_words   = node_header >> 24;
            u32 next_node   = node_header & 0x00FF'FFFF;

            // copy data
            start_address  += step_direction;

            for(u32 i = 0; i < num_words; i++)
            {
                write_to_component(m_bus->dispatch_read<u32>(start_address));
                start_address += step_direction;
            }

            // move onto the next node
            start_address = next_node;

            // end copy
            if((start_address & 0x0080'0000) || (start_address == 0))
            {
                break;
            }
        }

        m_base_address.address    = start_address;
        m_channel_control.enabled = 0;
        m_meta_interrupt_request  = true;
    }
}