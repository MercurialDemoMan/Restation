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
#include "../core/Types.hpp"
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

/**
 * @brief reset the configuration 
 */
void Menu::reset()
{
    std::scoped_lock lock(m_menu_state_mutex);
    m_emulator_reset = false;
    m_show_controls = false;
    m_show_vram = true;
}

/**
 * @brief draw the menu and manage the logic 
 */
void Menu::render_and_update()
{
    ImGui::BeginMainMenuBar();
    if(ImGui::BeginMenu("State..."))
    {
        if(ImGui::MenuItem("Reset"))
        {
            set_emulator_reset(true);
        }
        ImGui::EndMenu();
    }
    if(ImGui::MenuItem("Show/Hide Controls"))
    {
        m_show_controls = !m_show_controls;
    }
    if(ImGui::MenuItem("Toggle VRAM"))
    {
        m_show_vram = !m_show_vram;
    }
    if(ImGui::MenuItem("Hide"))
    {
        LOG("TODO...");
    }
    ImGui::EndMainMenuBar();
    
    if(m_show_controls)
    {
        ImGui::Begin("Controls");
        if(ImGui::CollapsingHeader("Buttons", NULL, ImGuiTreeNodeFlags_DefaultOpen))
        {
            render_button_mapping(PSX::PeripheralsInput::DigitalButton::Triangle);
            render_button_mapping(PSX::PeripheralsInput::DigitalButton::Square);
            render_button_mapping(PSX::PeripheralsInput::DigitalButton::Cross);
            render_button_mapping(PSX::PeripheralsInput::DigitalButton::Circle);
            render_button_mapping(PSX::PeripheralsInput::DigitalButton::Start);
            render_button_mapping(PSX::PeripheralsInput::DigitalButton::Select);
        }
        if(ImGui::CollapsingHeader("D-Pad", NULL, ImGuiTreeNodeFlags_DefaultOpen))
        {
            render_button_mapping(PSX::PeripheralsInput::DigitalButton::Up);
            render_button_mapping(PSX::PeripheralsInput::DigitalButton::Left);
            render_button_mapping(PSX::PeripheralsInput::DigitalButton::Down);
            render_button_mapping(PSX::PeripheralsInput::DigitalButton::Right);
        }
        if(ImGui::CollapsingHeader("Shoulder Buttons", NULL, ImGuiTreeNodeFlags_DefaultOpen))
        {
            render_button_mapping(PSX::PeripheralsInput::DigitalButton::L1);
            render_button_mapping(PSX::PeripheralsInput::DigitalButton::R1);
            render_button_mapping(PSX::PeripheralsInput::DigitalButton::L2);
            render_button_mapping(PSX::PeripheralsInput::DigitalButton::R2);
            render_button_mapping(PSX::PeripheralsInput::DigitalButton::L3);
            render_button_mapping(PSX::PeripheralsInput::DigitalButton::R3);
        }
        ImGui::End();
    }
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


/**
 * @brief on/off switch between showing whole vram or just the correct portion
 */
bool Menu::show_vram()
{
    return m_show_vram;
}

/**
 * @brief  
 */
void Menu::render_button_mapping(PSX::PeripheralsInput::DigitalButton button)
{
    ImGui::NewLine();
    ImGui::SameLine(0.0f);
    ImGui::BeginDisabled();
    ImGui::Button
    (
        fmt::format("{:>8}:", PSX::PeripheralsInput::DigitalButtonName[PSX::u32(button)]).c_str()
    );
    ImGui::EndDisabled();

    ImGui::SameLine(0.0f);
    if(ImGui::Button(fmt::format("TODO: show host mapping of {}", PSX::PeripheralsInput::DigitalButtonName[PSX::u32(button)]).c_str()))
    {
        LOG("TODO: remap buttons");
    }
}