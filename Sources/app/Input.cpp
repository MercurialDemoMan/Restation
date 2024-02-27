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
#include "../core/Macros.hpp"

/**
 * @brief allocate input manager and set default button mapping
 */
std::shared_ptr<Input> Input::create()
{
    auto result = std::shared_ptr<Input>(new Input());
    result->keyboard_to_button_mapping(SDLK_l,     PSX::PeripheralsInput::DigitalButton::Select);
    result->keyboard_to_button_mapping(SDLK_k,     PSX::PeripheralsInput::DigitalButton::Start);
    result->keyboard_to_button_mapping(SDLK_w,     PSX::PeripheralsInput::DigitalButton::Up);
    result->keyboard_to_button_mapping(SDLK_d,     PSX::PeripheralsInput::DigitalButton::Right);
    result->keyboard_to_button_mapping(SDLK_s,     PSX::PeripheralsInput::DigitalButton::Down);
    result->keyboard_to_button_mapping(SDLK_a,     PSX::PeripheralsInput::DigitalButton::Left);
    result->keyboard_to_button_mapping(SDLK_n,     PSX::PeripheralsInput::DigitalButton::L2);
    result->keyboard_to_button_mapping(SDLK_m,     PSX::PeripheralsInput::DigitalButton::R2);
    result->keyboard_to_button_mapping(SDLK_q,     PSX::PeripheralsInput::DigitalButton::L1);
    result->keyboard_to_button_mapping(SDLK_e,     PSX::PeripheralsInput::DigitalButton::R1);
    result->keyboard_to_button_mapping(SDLK_UP,    PSX::PeripheralsInput::DigitalButton::Triangle);
    result->keyboard_to_button_mapping(SDLK_RIGHT, PSX::PeripheralsInput::DigitalButton::Circle);
    result->keyboard_to_button_mapping(SDLK_DOWN,  PSX::PeripheralsInput::DigitalButton::Cross);
    result->keyboard_to_button_mapping(SDLK_LEFT,  PSX::PeripheralsInput::DigitalButton::Square);

    result->joystick_button_to_button_mapping(0, PSX::PeripheralsInput::DigitalButton::Cross);
    result->joystick_button_to_button_mapping(1, PSX::PeripheralsInput::DigitalButton::Circle);
    result->joystick_button_to_button_mapping(2, PSX::PeripheralsInput::DigitalButton::Square);
    result->joystick_button_to_button_mapping(3, PSX::PeripheralsInput::DigitalButton::Triangle);
    return result;
}

/**
 * @brief Close all open joystics 
 */
Input::~Input()
{
    for(auto* joystick : m_joysticks)
    {
        SDL_JoystickClose(joystick);
    }
}

/**
 * @brief Process event from SDL2 to update key state and to manage gamepads 
 */
void Input::process_event(SDL_Event* event)
{
    if(event == nullptr)
    {
        UNREACHABLE();
    }

    switch(event->type)
    {
        case SDL_KEYDOWN:
        {
            update_keyboard_key_state(event->key.keysym.sym, 1);
        } break;
        case SDL_KEYUP:
        {
            update_keyboard_key_state(event->key.keysym.sym, 0);
        } break;
        case SDL_JOYDEVICEADDED:
        {
            // TODO: potentially flawed logic, what if we have 3 controllers and the 1st one disconnects?
            if(event->jdevice.which < PSX::s32(m_joysticks.size()))
            {
                force_recheck_joysticks();
            }
            else
            {
                auto* joystick = SDL_JoystickOpen(event->jdevice.which);
                m_joysticks.push_back(joystick);
            }
        } break;
        case SDL_JOYDEVICEREMOVED:
        {
            if(event->jdevice.which >= 0 && event->jdevice.which < PSX::s32(m_joysticks.size()))
            {
                SDL_JoystickClose(m_joysticks[event->jdevice.which]);
                m_joysticks.erase(m_joysticks.begin() + event->jdevice.which);
            }
            else
            {
                force_recheck_joysticks();
            }
        } break;
        case SDL_JOYAXISMOTION:
        {
            static constexpr PSX::u16 DeadZoneLimit = 16000;
            if(std::abs(event->jaxis.value) > DeadZoneLimit)
                LOG(fmt::format("joy {} axis {} value {}", event->jdevice.which, event->jaxis.axis, event->jaxis.value));
        } break;
        case SDL_JOYBALLMOTION:
        {
            LOG(fmt::format("joy {} ball {} value {} {}", event->jdevice.which, event->jball.ball, event->jball.xrel, event->jball.yrel));
        } break;
        case SDL_JOYHATMOTION:
        {
            LOG(fmt::format("joy {} hat {} value {}", event->jdevice.which, event->jhat.hat, event->jhat.value));
        } break;
        case SDL_JOYBUTTONDOWN:
        {
            update_joystick_button_state(event->jbutton.button, 1);
            LOG(fmt::format("joy {} button {} down", event->jdevice.which, event->jbutton.button));
        } break;
        case SDL_JOYBUTTONUP:
        {
            update_joystick_button_state(event->jbutton.button, 0);
            LOG(fmt::format("joy {} button {} up", event->jdevice.which, event->jbutton.button));
        } break;
    }
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
 * @brief Setup button mapping between emulator keyboard input and host input
 */
void Input::keyboard_to_button_mapping(SDL_Keycode host_key, PSX::PeripheralsInput::DigitalButton emulator_button)
{
    m_keyboard_button_mapping[host_key] = emulator_button;
    m_button_state[emulator_button] = 0;
}

/**
 * @brief Setup button mapping between emulator joystick button input and host input
 */
void Input::joystick_button_to_button_mapping(PSX::u8 host_button, PSX::PeripheralsInput::DigitalButton emulator_button)
{
    m_joystick_button_mapping[host_button] = emulator_button;
    m_button_state[emulator_button] = 0;
}

/**
 * @brief Set pressed/unpressed state of a host key
 */
void Input::update_keyboard_key_state(SDL_Keycode key, PSX::u32 state)
{
    auto map_it = m_keyboard_button_mapping.find(key);
    if(map_it == m_keyboard_button_mapping.end())
        return;
    
    auto state_it = m_button_state.find(map_it->second);
    if(state_it == m_button_state.end())
        return;

    state_it->second = state;
}

/**
 * @brief Set pressed/unpressed state of a host joystick button 
 */
void Input::update_joystick_button_state(PSX::u8 button, PSX::u32 state)
{
    auto map_it = m_joystick_button_mapping.find(button);
    if(map_it == m_joystick_button_mapping.end())
        return;
    
    auto state_it = m_button_state.find(map_it->second);
    if(state_it == m_button_state.end())
        return;

    state_it->second = state;
}

/**
 * @brief Close all existing joystics and check current status of joystics 
 */
void Input::force_recheck_joysticks()
{
    for(auto* joystick : m_joysticks)
    {
        SDL_JoystickClose(joystick);
    }

    m_joysticks.clear();

    for(PSX::s32 i = 0; i < SDL_NumJoysticks(); i++)
    {
        auto* joystick = SDL_JoystickOpen(i);
        if(joystick == nullptr)
        {
            LOG_WARNING(fmt::format("couldn't open joystick with index {}: {}", i, SDL_GetError()));
            continue;
        }
        m_joysticks.push_back(joystick);
    }
}