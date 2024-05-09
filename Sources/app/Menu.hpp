/**
 * @file      Menu.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX Emulator Frontend Menu
 *
 * @version   0.1
 *
 * @date      16. 2. 2024 (created)
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

#ifndef MENU_HPP
#define MENU_HPP

#include "MenuTypes.hpp"

#include "../core/PeripheralsInput.hpp"
#include "../core/GPUTypes.hpp"
#include "Input.hpp"

#include <atomic>
#include <string>
#include <memory>

#include <SDL.h>

class Menu
{
public:

    /**
     * @brief Allocate menu manager and set default configuration
     */
    static std::shared_ptr<Menu> create(const std::shared_ptr<Input>&);

    /**
     * @brief reset the configuration 
     */
    void reset();

    /**
     * @brief draw the menu and manage the logic 
     */
    void render_and_update();

    /**
     * @brief check if user wants to reset the emulator
     */
    bool emulator_reset();

    /**
     * @brief check if user wants to save state of the emulator
     */
    bool emulator_save_state();

    /**
     * @brief check if user wants to load state of the emulator
     */
    bool emulator_load_state();

    /**
     * @brief on/off switch between showing whole vram or just the correct portion
     */
    bool show_vram();

    /**
     * @brief obtain current desired emulator speed scale
     */
    EmulatorSpeed emulator_speed();

    /**
     * @brief obtain current GPU rendering resolution 
     */
    PSX::RenderTarget emulator_resolution();

    /**
     * @brief set emulator reset status 
     */
    void set_emulator_reset(bool);

    /**
     * @brief set emulator save state status 
     */
    void set_emulator_save_state(bool);

    /**
     * @brief set emulator load state status 
     */
    void set_emulator_load_state(bool);

    /**
     * @brief set current desired emulator speed scale
     */
    void set_emulator_speed(EmulatorSpeed);

    /**
     * @brief set current GPU rendering resolution 
     */
    void set_emulator_resolution(PSX::RenderTarget);

    /**
     * @brief Process event from SDL2 to update key state and to manage menu
     */
    void process_event(SDL_Event*);

private:

    explicit Menu(const std::shared_ptr<Input>& input):
        m_input(input)
    {

    }

    /**
     * @brief  
     */
    void render_button_mapping(PSX::PeripheralsInput::DigitalButton);

    std::shared_ptr<Input>     m_input;
    std::atomic<bool>          m_emulator_reset;
    std::atomic<bool>          m_emulator_save_state;
    std::atomic<bool>          m_emulator_load_state;
    std::atomic<EmulatorSpeed> m_emulator_speed;
    std::atomic<PSX::RenderTarget> m_emulator_resolution;
    bool                       m_show_controls;
    bool                       m_show_vram;
    bool                       m_show_menu;
    std::optional<SDL_Keycode> m_last_pressed_keyboard_button;
};

#endif // MENU_HPP