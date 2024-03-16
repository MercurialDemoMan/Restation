/**
 * @file      Timer.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX Timer clock
 *
 * @version   0.1
 *
 * @date      26. 10. 2023, 16:35 (created)
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

#ifndef TIMER_HPP
#define TIMER_HPP

#include <memory>
#include "Component.hpp"
#include "Forward.hpp"
#include "TimerTypes.hpp"

namespace PSX
{
    /**
     * @brief PSX Timer
     */
    template<ClockSource Source>
    class Timer final : public Component
    {
    public:

        Timer(const std::shared_ptr<InterruptController>& interrupt_controller) :
            m_interrupt_controller(interrupt_controller)
        {
            reset();
        }
        
        virtual ~Timer() override = default;

        virtual void execute(u32 num_steps) override;
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;
        virtual void serialize(std::shared_ptr<SaveState>&) override;
        virtual void deserialize(std::shared_ptr<SaveState>&) override;

    private:

        /**
         * @brief set interrupt request flag 
         */
        void trigger_interrupt_request();

        /**
         * @brief bit-fields accessors for timer counter mode
         */
        union CounterMode
        {
            struct
            {
                u32 synchronization_enabled: 1;
                u32 synchronization_mode: 2;
                u32 reset_counter_to_zero: 1;
                u32 irq_when_counter_eq_target: 1;
                u32 irq_when_counter_eq_ffff: 1;
                u32 irq_repeat_mode: 1;
                u32 irq_toggle_mode: 1;
                u32 clock_source: 2;
                u32 interrupt_request: 1;
                u32 reached_target_value: 1;
                u32 reached_ffff_value: 1;
                u32: 19;
            };
            
            u32 raw;
            u8  bytes[sizeof(u32)];
        };

        std::shared_ptr<InterruptController> m_interrupt_controller; /// connection to interrupt controller

        Register<u32> m_current_counter_value;
        CounterMode   m_counter_mode;
        Register<u32> m_counter_target_value;

        float m_meta_counter;     /// helper counter to calculate correct register values
        bool  m_meta_paused;      /// helper state bit to pause counting
        bool  m_meta_irq_occured; /// helper state bit to keep track of one shot irq
    };

}

#endif // TIMER_HPP