/**
 * @file      Peripherals.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation of the PSX Peripherals
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

#include "Peripherals.hpp"
#include "Bus.hpp"
#include "InterruptController.hpp"
#include "PeripheralsController.hpp"
#include "PeripheralsDigitalController.hpp"
#include "PeripheralsMemoryCardController.hpp"

namespace PSX
{
    void Peripherals::execute(u32 num_steps)
    {
        // TODO: baud counter at 33MHz

        for(u32 _ = 0; _ < num_steps; _++)
        {
            if(m_meta_interrupt_request_delay > 0)
            {
                if(--m_meta_interrupt_request_delay == 0)
                {
                    m_joy_stat.interrupt_request = 1;
                    m_joy_control.acknowledge    = 0;
                }
            }
            
            if(m_joy_stat.interrupt_request)
            {
                m_interrupt_controller->trigger_interrupt(Interrupt::Peripheral);
            }
        }
    }

    u32 Peripherals::read(u32 address)
    {
        switch(address)
        {
            case 0:
            {
                u8 result = m_joy_rx_data.read(0);
                m_joy_rx_data.write(0, 0xFF);
                m_joy_stat.rx_fifo_not_empty = 0;
                return result;
            } break;
            case 4:
            case 5:
            case 6:
            case 7:
            {
                u8 result = m_joy_stat.bytes[address - 4];
                m_joy_control.acknowledge = 0;
                return result;
            } break;
            case 8:
            case 9:
            {
                return m_joy_mode.read(address - 8);
            } break;
            case 10:
            case 11:
            {
                return m_joy_control.bytes[address - 10];
            } break;
            case 14:
            case 15:
            {
                return m_joy_baud.read(address - 14);
            } break;
        }

        UNREACHABLE();
    }

    void Peripherals::write(u32 address, u32 value)
    {
        switch(address)
        {
            case 0:
            {
                send_byte_to_controller_or_memory_card(value); return;
            } break;
            case 8:
            case 9:
            {
                m_joy_mode.write(address - 8, value); return;
            } break;
            case 10:
            {
                m_joy_control.bytes[address - 10] = value;
                if(m_joy_control.acknowledge)
                {
                    m_joy_stat.interrupt_request = 0;
                }
                return;
            }
            case 11:
            {
                m_joy_control.bytes[address - 11] = value; 
                if(m_joy_control.reset)
                {
                    m_meta_currently_communicating_with = CurrentlyCommunicatingWith::None;
                    m_controllers[0]->reset();
                    m_controllers[1]->reset();
                    m_memory_cards[0]->reset();
                    m_memory_cards[1]->reset();
                }
                return;
            } break;
            case 14:
            case 15:
            {
                m_joy_baud.write(address - 14, value); return;
            } break;
        }

        UNREACHABLE();
    }

    void Peripherals::reset()
    {
        m_joy_rx_data.raw() = 0xFFFFFFFF;

        m_joy_stat.raw = 0;
        m_joy_stat.tx_ready_flag_1   = 1;
        m_joy_stat.rx_fifo_not_empty = 0;
        m_joy_stat.tx_ready_flag_2   = 1;
        m_joy_stat.rx_parity_error   = 0;
        m_joy_stat.ack_input_level   = 0;

        m_joy_mode.raw() = 0;
        m_joy_control.raw = 0;
        m_joy_baud.raw() = 0;

        m_meta_currently_communicating_with = CurrentlyCommunicatingWith::None;
        m_meta_interrupt_request_delay = 0;

        m_controllers[0] = std::make_unique<PeripheralsDigitalController>(m_input);
        m_controllers[1] = std::make_unique<PeripheralsDigitalController>(m_input);
        m_memory_cards[0] = std::make_unique<PeripheralsMemoryCardController>();
        m_memory_cards[1] = std::make_unique<PeripheralsMemoryCardController>();
    }

    /**
     * @brief communicate with peripherals
     */
    void Peripherals::send_byte_to_controller_or_memory_card(u8 value)
    {
        m_joy_stat.rx_fifo_not_empty = 1;
        
        // After sending a byte, the Kernel waits 100 cycles or so, 
        // and does THEN acknowledge any old IRQ7, and does then wait 
        // for the new IRQ7. Due to that bizarre coding, emulators 
        // can't trigger IRQ7 immediately within 0 cycles after 
        // sending the byte.

        // initiate communication
        if(m_meta_currently_communicating_with == CurrentlyCommunicatingWith::None)
        {
            if(value == 0b0000'0001)
                m_meta_currently_communicating_with = CurrentlyCommunicatingWith::Controller;
            
            if(value == 0b1000'0001)
                m_meta_currently_communicating_with = CurrentlyCommunicatingWith::MemoryCard;
        }

        if(m_meta_currently_communicating_with == CurrentlyCommunicatingWith::Controller)
        {
            auto new_data = m_controllers[m_joy_control.desired_slot_number]->send_byte(value);
            auto ack_flag = m_controllers[m_joy_control.desired_slot_number]->ack();

            m_joy_rx_data.write(0, new_data);
            m_joy_control.acknowledge = ack_flag;

            if(ack_flag)
                m_meta_interrupt_request_delay = ControllerInterruptRequestDelay;

            if(m_controllers[m_joy_control.desired_slot_number]->communication_ended())
                m_meta_currently_communicating_with = CurrentlyCommunicatingWith::None;
        }

        if(m_meta_currently_communicating_with == CurrentlyCommunicatingWith::MemoryCard)
        {
            auto new_data = m_memory_cards[m_joy_control.desired_slot_number]->send_byte(value);
            auto ack_flag = m_memory_cards[m_joy_control.desired_slot_number]->ack();

            m_joy_rx_data.write(0, new_data);
            m_joy_control.acknowledge = ack_flag;

            if(ack_flag)
                m_meta_interrupt_request_delay = MemoryCardInterruptRequestDelay;

            if(m_memory_cards[m_joy_control.desired_slot_number]->communication_ended())
                m_meta_currently_communicating_with = CurrentlyCommunicatingWith::None; 
        }

        // TODO: delay interrupt
    }
}