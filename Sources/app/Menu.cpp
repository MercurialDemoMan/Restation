/**
 * @file      Menu.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for the PSX Emulator Frontend Menu
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

#include "Menu.hpp"
#include "../core/Macros.hpp"
#include <imgui.h>

/**
 * @brief reset the configuration 
 */
void Menu::reset()
{
    std::scoped_lock lock(m_menu_state_mutex);
    m_emulator_reset = false;
}

/**
 * @brief draw the menu and manage the logic 
 */
void Menu::render_and_update()
{
    ImGui::BeginMainMenuBar();
    if(ImGui::BeginMenu("File..."))
    {
        if(ImGui::MenuItem("Load BIOS"))
        {
            LOG("TODO...");
        }
        if(ImGui::MenuItem("Load Game"))
        {
            LOG("TODO...");
        }
        ImGui::EndMenu();
    }
    if(ImGui::BeginMenu("State..."))
    {
        if(ImGui::MenuItem("Reset"))
        {
            set_emulator_reset(true);
        }
        ImGui::EndMenu();
    }
    if(ImGui::MenuItem("Controls"))
    {
        LOG("TODO...");
    }
    if(ImGui::MenuItem("Hide"))
    {
        LOG("TODO...");
    }
    ImGui::EndMainMenuBar();
}

/**
 * @brief check if user wants to reset the emulator
 */
bool Menu::emulator_reset()
{
    std::scoped_lock lock(m_menu_state_mutex);
    return m_emulator_reset;
}

/**
 * @brief set emulator reset status 
 */
void Menu::set_emulator_reset(bool value)
{
    std::scoped_lock lock(m_menu_state_mutex);
    m_emulator_reset = value;
}