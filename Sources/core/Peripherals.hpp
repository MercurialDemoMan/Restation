/**
 * @file      Peripherals.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX Interrupt controller
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

#ifndef PERIPHERALS_HPP
#define PERIPHERALS_HPP

#include <memory>
#include <array>
#include "Component.hpp"
#include "Forward.hpp"
#include "PeripheralsInput.hpp"
#include "PeripheralsController.hpp"
#include "PeripheralsTypes.hpp"

namespace PSX
{
    /**
     * @brief PSX Direct memory access controller
     */
    class Peripherals final : public Component
    {
    public:

        static constexpr const u32 ControllerInterruptRequestDelay = 376; 
        static constexpr const u32 MemoryCardInterruptRequestDelay = 125;

        Peripherals(const std::shared_ptr<Bus>& bus, 
                    const std::shared_ptr<PeripheralsInput>& input,
                    const std::shared_ptr<InterruptController>& interrupt_controller) :
            m_bus(bus),
            m_input(input),
            m_interrupt_controller(interrupt_controller)
        {
            reset();
        }
        
        virtual ~Peripherals() override = default;

        virtual void execute(u32 num_steps) override;
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;
        virtual void serialize(std::shared_ptr<SaveState>&) override;
        virtual void deserialize(std::shared_ptr<SaveState>&) override;

    private:

        /**
         * @brief communicate with peripherals
         */
        void send_byte_to_controller_or_memory_card(u8);

        /**
         * @brief 0x1F80'1044 JOY_STAT Register 
         */
        union JoyStat
        {
            struct
            {
                u32 tx_ready_flag_1:   1;
                u32 rx_fifo_not_empty: 1;
                u32 tx_ready_flag_2:   1;
                u32 rx_parity_error:   1;
                u32: 3;
                u32 ack_input_level:   1;
                u32: 1;
                u32 interrupt_request: 1;
                u32: 1;
                u32 baudrate_timer:   21;
            };

            u32 raw;
            u8  bytes[sizeof(u32)];
        };

        /**
         * @brief 0x1F80'104A JOY_CTRL Register 
         */
        union JoyCtrl
        {
            struct
            {
                u16 tx_enable:   1;
                u16 joyn_output: 1;
                u16 rx_enable:   1;
                u16: 1;
                u16 acknowledge: 1;
                u16: 1;
                u16 reset:       1;
                u16: 1;
                u16 rx_interrupt_mode:    2;
                u16 tx_interrupt_enable:  1;
                u16 rx_interrupt_enable:  1;
                u16 ack_interrupt_enable: 1;
                u16 desired_slot_number:  1;
                u16: 2;
            };

            u16 raw;
            u8  bytes[sizeof(u16)];
        };

        std::shared_ptr<Bus> m_bus;
        std::shared_ptr<PeripheralsInput> m_input;
        std::shared_ptr<InterruptController> m_interrupt_controller;

        Register<u32> m_joy_rx_data; 
        JoyStat       m_joy_stat;
        Register<u16> m_joy_mode;
        JoyCtrl       m_joy_control;
        Register<u16> m_joy_baud;

        CurrentlyCommunicatingWith m_meta_currently_communicating_with;
        u32 m_meta_interrupt_request_delay;
        std::array<std::unique_ptr<PeripheralsController>, 2> m_controllers;
        std::array<std::unique_ptr<PeripheralsController>, 2> m_memory_cards;
    };
}

#endif // PERIPHERALS_HPP