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

#ifndef MENU_HPP
#define MENU_HPP

#include <mutex>
#include <string>
#include "../core/PeripheralsInput.hpp"

class Menu
{
public:

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
     * @brief set emulator reset status 
     */
    void set_emulator_reset(bool);

private:

    void render_button_mapping(PSX::PeripheralsInput::DigitalButton);

    bool m_emulator_reset;
    bool m_show_controls;
    std::string m_test;
    
    std::mutex m_menu_state_mutex;
};

#endif // MENU_HPP