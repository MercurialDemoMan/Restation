/**
 * @file      Input.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for sending input to the host emulator
 *
 * @version   0.1
 *
 * @date      17. 2. 2024 (created)
 *
 * @section   TODO: replace with actual documentation
 * TODO: documentation text
 *
 * @section License
 * This file is part of the TODO: project \n
 *
 * Copyright (C) 2024
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

#include "Input.hpp"

/**
 * @brief allocate input manager and set default button mapping
 */
std::shared_ptr<Input> Input::create()
{
    auto result = std::shared_ptr<Input>(new Input());
    result->button_mapping(SDLK_l,     PSX::PeripheralsInput::DigitalButton::Select);
    result->button_mapping(SDLK_k,     PSX::PeripheralsInput::DigitalButton::Start);
    result->button_mapping(SDLK_w,     PSX::PeripheralsInput::DigitalButton::Up);
    result->button_mapping(SDLK_d,     PSX::PeripheralsInput::DigitalButton::Right);
    result->button_mapping(SDLK_s,     PSX::PeripheralsInput::DigitalButton::Down);
    result->button_mapping(SDLK_a,     PSX::PeripheralsInput::DigitalButton::Left);
    result->button_mapping(SDLK_n,     PSX::PeripheralsInput::DigitalButton::L2);
    result->button_mapping(SDLK_m,     PSX::PeripheralsInput::DigitalButton::R2);
    result->button_mapping(SDLK_q,     PSX::PeripheralsInput::DigitalButton::L1);
    result->button_mapping(SDLK_e,     PSX::PeripheralsInput::DigitalButton::R1);
    result->button_mapping(SDLK_UP,    PSX::PeripheralsInput::DigitalButton::Triangle);
    result->button_mapping(SDLK_RIGHT, PSX::PeripheralsInput::DigitalButton::Circle);
    result->button_mapping(SDLK_DOWN,  PSX::PeripheralsInput::DigitalButton::Cross);
    result->button_mapping(SDLK_LEFT,  PSX::PeripheralsInput::DigitalButton::Square);
    return result;
}

/**
 * @brief Interface used by peripherals component to receive input from host system
 */
bool Input::is_digital_button_down(PSX::PeripheralsInput::DigitalButton emulator_key)
{
    auto it = m_button_state.find(emulator_key);
    if(it == m_button_state.end())
    {
        return false;
    }

    return it->second;
}

/**
 * @brief Setup button mapping between emulator input and host input
 */
void Input::button_mapping(SDL_Keycode host_key, PSX::PeripheralsInput::DigitalButton emulator_key)
{
    m_button_mapping[host_key]   = emulator_key;
    m_button_state[emulator_key] = 0;
}

/**
 * @brief Set pressed/unpressed state of a host key
 */
void Input::update_key_state(SDL_Keycode key, PSX::u32 state)
{
    auto map_it = m_button_mapping.find(key);
    if(map_it == m_button_mapping.end())
        return;
    
    auto state_it = m_button_state.find(map_it->second);
    if(state_it == m_button_state.end())
        return;

    state_it->second = state;
}