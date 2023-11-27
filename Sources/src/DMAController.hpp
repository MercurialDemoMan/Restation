/**
 * @file      DMAController.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX Direct memory access controller
 *
 * @version   0.1
 *
 * @date      26. 10. 2023, 16:51 (created)
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

#ifndef DMACONTROLLER_HPP
#define DMACONTROLLER_HPP

#include <memory>
#include "Component.hpp"
#include "Forward.hpp"
#include "DMAChannel.hpp"
#include "DMATypes.hpp"

namespace PSX
{
    /**
     * @brief PSX Direct memory access controller
     */
    class DMAController final : public Component
    {
    public:

        DMAController(const std::shared_ptr<Bus>& bus) :
            m_bus(bus)
        {
            reset();
        }
        
        virtual ~DMAController() override = default;

        virtual void execute(u32 num_steps) override;
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;

    private:

        std::shared_ptr<Bus>        m_bus;
        std::shared_ptr<DMAChannel> m_channels[ChannelType::Size];

        /**
         * @brief DMA Control register 
         */
        union Control
        {
            struct
            {
                u32 mdec_in_priority:       3;
                u32 mdec_in_master_enable:  1;
                u32 mdec_out_priority:      3;
                u32 mdec_out_master_enable: 1;
                u32 gpu_priority:           3;
                u32 gpu_master_enable:      1;
                u32 cdrom_priority:         3;
                u32 cdrom_master_enable:    1;
                u32 spu_priority:           3;
                u32 spu_master_enable:      1;
                u32 pio_priority:           3;
                u32 pio_master_enable:      1;
                u32 otc_priority:           3;
                u32 otc_master_enable:      1;
            };

            u32 raw;
        };

        /**
         * @brief DMA Interrupt register 
         */
        union Interrupt
        {
            struct
            {
                u32: 15;
                u32 force_irq:             1;
                u32 channel0_enable:       1;
                u32 channel1_enable:       1;
                u32 channel2_enable:       1;
                u32 channel3_enable:       1;
                u32 channel4_enable:       1;
                u32 channel5_enable:       1;
                u32 channel6_enable:       1;
                u32 channel_master_enable: 1;
                u32 channel0_irq:          1;
                u32 channel1_irq:          1;
                u32 channel2_irq:          1;
                u32 channel3_irq:          1;
                u32 channel4_irq:          1;
                u32 channel5_irq:          1;
                u32 channel6_irq:          1;
                u32 irq_master_enable:     1;
            };

            u32 raw;
        };

        Control   m_control;
        Interrupt m_interrupt;
    };
}

#endif // DMACONTROLLER_HPP