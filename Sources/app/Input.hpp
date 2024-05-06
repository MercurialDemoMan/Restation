/**
 * @file      Input.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for sending input to the host emulator
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

#ifndef INPUT_HPP
#define INPUT_HPP

#include "../core/Types.hpp"
#include "../core/PeripheralsInput.hpp"

#include <tuple>
#include <memory>
#include <vector>
#include <optional>
#include <unordered_map>

#include <SDL.h>

/**
 * @brief Emulator input manager for the SDL2 library 
 */
class Input final : public PSX::PeripheralsInput
{
public:

    /**
     * @brief Allocate input manager and set default button mapping
     */
    static std::shared_ptr<Input> create();

    /**
     * @brief Close all open joystics 
     */
    ~Input();

    /**
     * @brief Interface used by peripherals component to receive input from host system
     */
    virtual bool is_digital_button_down(PSX::PeripheralsInput::DigitalButton) override;

    /**
     * @brief Setup button mapping between emulator keyboard input and host input
     */
    void keyboard_button_to_button_mapping(SDL_Keycode, PSX::PeripheralsInput::DigitalButton);

    /**
     * @brief Setup button mapping between emulator joystick button input and host input
     */
    void joystick_button_to_button_mapping(PSX::u8, PSX::PeripheralsInput::DigitalButton);

    /**
     * @brief obtain host key name, which maps onto a specific digital button
     */
    std::tuple<SDL_Keycode, std::string> get_keyboard_button_mapping_key(PSX::PeripheralsInput::DigitalButton);

    /**
     * @brief Process event from SDL2 to update key state and to manage gamepads 
     */
    void process_event(SDL_Event*);

private:

    /**
     * @brief Set pressed/unpressed state of a host keyboard key
     */
    void update_keyboard_key_state(SDL_Keycode, PSX::u32 state);

    /**
     * @brief Set pressed/unpressed state of a host joystick button 
     */
    void update_joystick_button_state(PSX::u8 button, PSX::u32 state);

    /**
     * @brief Close all existing joystics and check current status of joystics 
     */
    void force_recheck_joysticks();

    explicit Input() {}

    std::vector<SDL_Joystick*> m_joysticks; /// Keep track of all connected joysticks/gamepads

    std::unordered_map<PSX::u8, PSX::PeripheralsInput::DigitalButton>     m_joystick_button_mapping;
    std::unordered_map<SDL_Keycode, PSX::PeripheralsInput::DigitalButton> m_keyboard_button_mapping;
    std::unordered_map<PSX::PeripheralsInput::DigitalButton, PSX::u32>    m_button_state;
};

#endif // INPUT_HPP