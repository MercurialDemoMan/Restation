/**
 * @file      DMAChannel.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX Direct memory access channel
 *
 * @version   0.1
 *
 * @date      27. 11. 2023, 17:36 (created)
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

#ifndef DMACHANNEL_HPP
#define DMACHANNEL_HPP

#include <memory>
#include "Component.hpp"
#include "Forward.hpp"
#include "DMATypes.hpp"

namespace PSX
{
    /**
     * @brief PSX Direct memory access channel
     */
    class DMAChannel : public Component
    {
    public:

        DMAChannel(const std::shared_ptr<Bus>& bus) :
            m_bus(bus)
        {
            reset();
        }
        
        virtual ~DMAChannel() override = default;

        virtual void execute(u32 num_steps) override;
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;
        virtual void serialize(std::shared_ptr<SaveState>&) override;
        virtual void deserialize(std::shared_ptr<SaveState>&) override;

        /**
         * @brief read from managed component
         */
        virtual u32 read_from_component();

        /**
         * @brief write to managed component
         */
        virtual void write_to_component(u32 value);

        /**
         * @brief return channel type 
         */
        virtual ChannelType type() const = 0;

        /**
         * @brief perform DMA in word mode 
         */
        virtual void word_copy();

        /**
         * @brief perform DMA in block mode 
         */
        virtual void block_copy();

        /**
         * @brief perform DMA in linked list mode 
         */
        virtual void linked_list_copy();

        /**
         * @brief flag for indicating DMA copy has been finished
         */
        bool& meta_interrupt_request()
        {
            return m_meta_interrupt_request;
        }

        /**
         * @brief used by sync block transfer to 
         *        check whether there is data to copy
         */
        virtual bool sync_request() const
        {
            return true;
        }

        /**
         * @brief used by sync block transfer to
         *        check whether copy can be interleaved
         */
        virtual bool interleaved_copy() const
        {
            return false;
        }

        /**
         * @brief mask channel control register when writing into it
         */
        virtual void mask_channel_control_register()
        {
            
        }

    protected:

        std::shared_ptr<Bus> m_bus;

        /**
         * @brief DMA Channel base address for initiating DMA 
         */
        union BaseAddress
        {
            struct
            {
                u32 address: 24;

                u32:         8;
            };

            u32 raw;
            u8  bytes[sizeof(u32)];
        };

        /**
         * @brief DMA Channel block control for initiating DMA
         */
        union BlockControl
        {
            union
            {
                // sync mode 0 = OTC + CDROM
                struct
                {
                    u32 num_words: 16;

                    u32:           16;
                } sync_mode_0;

                // sync mode 1 = MDEC + SPU + GPU vram
                struct
                {
                    u32 block_size: 16;
                    u32 num_blocks: 16;
                } sync_mode_1;

                // sync mode 2 = GPU command list
                struct
                {
                    u32: 32;
                } sync_mode_2;
            };
            
            u32 raw;
            u8  bytes[sizeof(u32)];
        };

        /**
         * @brief DMA Channel Control register 
         */
        union ChannelControl
        {
            struct
            {
                u32 transfer_direction:  1; // 0 to ram, 1 from ram
                u32 memory_address_step: 1; // 0 = +4, 1 = -4

                u32: 6;

                u32 chopping_enable: 1;
                u32 sync_mode: 2;

                u32: 5;

                u32 chopping_dma_window_size: 3;

                u32: 1;

                u32 chopping_cpu_window_size: 3;
                
                u32: 1;

                u32 enabled: 1;

                u32: 3;

                u32 start_trigger: 1;

                u32: 3;
            };

            u32 raw;
            u8  bytes[sizeof(u32)];
        };

        BaseAddress    m_base_address;
        BlockControl   m_block_control;
        ChannelControl m_channel_control;

        bool           m_meta_interrupt_request;
    };
}

#endif // DMACHANNEL_HPP