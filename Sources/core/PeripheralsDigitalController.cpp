/**
 * @file      PeripheralsDigitalController.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for a digital controller peripheral
 *
 * @version   0.1
 *
 * @date      17. 2. 2024 (created)
 *
 * @section License
 * This file is part of the Restation \n
 *
 * Copyright (C) 2024
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

#include "PeripheralsDigitalController.hpp"

#include "Macros.hpp"

namespace PSX
{
    PeripheralsDigitalController::PeripheralsDigitalController(const std::shared_ptr<PeripheralsInput>& input) :
        m_input(input)
    {
        m_buttons_state.raw = 0xFFFF;
        m_communication_counter = 0;
    }

    /**
     * @brief transfer and handle 1 byte in communication sequence
     */
    u8 PeripheralsDigitalController::send_byte(u8 byte)
    {
        m_buttons_state.select   = !m_input->is_digital_button_down(PeripheralsInput::DigitalButton::Select);
        m_buttons_state.l3       = !m_input->is_digital_button_down(PeripheralsInput::DigitalButton::L3);
        m_buttons_state.r3       = !m_input->is_digital_button_down(PeripheralsInput::DigitalButton::R3);
        m_buttons_state.start    = !m_input->is_digital_button_down(PeripheralsInput::DigitalButton::Start);
        m_buttons_state.up       = !m_input->is_digital_button_down(PeripheralsInput::DigitalButton::Up);
        m_buttons_state.right    = !m_input->is_digital_button_down(PeripheralsInput::DigitalButton::Right);
        m_buttons_state.down     = !m_input->is_digital_button_down(PeripheralsInput::DigitalButton::Down);
        m_buttons_state.left     = !m_input->is_digital_button_down(PeripheralsInput::DigitalButton::Left);
        m_buttons_state.l2       = !m_input->is_digital_button_down(PeripheralsInput::DigitalButton::L2);
        m_buttons_state.r2       = !m_input->is_digital_button_down(PeripheralsInput::DigitalButton::R2);
        m_buttons_state.l1       = !m_input->is_digital_button_down(PeripheralsInput::DigitalButton::L1);
        m_buttons_state.r1       = !m_input->is_digital_button_down(PeripheralsInput::DigitalButton::R1);
        m_buttons_state.triangle = !m_input->is_digital_button_down(PeripheralsInput::DigitalButton::Triangle);
        m_buttons_state.circle   = !m_input->is_digital_button_down(PeripheralsInput::DigitalButton::Circle);
        m_buttons_state.cross    = !m_input->is_digital_button_down(PeripheralsInput::DigitalButton::Cross);
        m_buttons_state.square   = !m_input->is_digital_button_down(PeripheralsInput::DigitalButton::Square);
    
        switch(m_communication_counter)
        {
            case 0:
            {
                if(byte == 0x01)
                {
                    m_communication_counter++;
                }
                return 0xFF;
            } break;
            case 1:
            {
                if(byte == 0x42)
                {
                    m_communication_counter++;
                    return 0x41;
                }
                else
                {
                    m_communication_counter = 0;
                    return 0xFF;
                }
            } break;
            case 2:
            {
                m_communication_counter++;
                return 0x5A;
            } break;
            case 3:
            {
                m_communication_counter++;
                return m_buttons_state.bytes[0];
            } break;
            case 4:
            {
                m_communication_counter = 0;
                return m_buttons_state.bytes[1];
            } break;
        }

        return 0xFF;
    }

    /**
     * @brief check if controller ended communication sequence 
     */
    bool PeripheralsDigitalController::communication_ended() const
    {
        return m_communication_counter == 0;
    }

    /**
     * @brief send acknowledge flag back
     */
    bool PeripheralsDigitalController::ack() const
    {
        return m_communication_counter != 0;
    }

    /**
     * @brief reset any communication temporaries 
     */
    void PeripheralsDigitalController::reset()
    {
        m_communication_counter = 0;
    }

    void PeripheralsDigitalController::serialize(std::shared_ptr<SaveState>& save_state)
    {
        save_state->serialize_from(m_buttons_state.raw);
        save_state->serialize_from(m_communication_counter);
    }

    void PeripheralsDigitalController::deserialize(std::shared_ptr<SaveState>& save_state)
    {
        save_state->deserialize_to(m_buttons_state.raw);
        save_state->deserialize_to(m_communication_counter);
    }
}