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

#include "Menu.hpp"

#include "../core/Macros.hpp"
#include "../core/Types.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

std::shared_ptr<Menu> Menu::create(const std::shared_ptr<Input>& input)
{
    auto menu = std::shared_ptr<Menu>(new Menu(input));
    menu->reset();
    return menu;
}

/**
 * @brief reset the configuration 
 */
void Menu::reset()
{
    m_emulator_reset = false;
    m_emulator_save_state = false;
    m_emulator_load_state = false;
    m_show_controls = false;
    m_show_vram = false;
    m_show_menu = true;
    m_emulator_speed = EmulatorSpeed::_100Percent;
    m_emulator_resolution = PSX::RenderTarget::VRam1x;
}

/**
 * @brief draw the menu and manage the logic 
 */
void Menu::render_and_update()
{
    bool show_reset_popup = false;

    if(m_show_menu)
    {
        ImGui::BeginMainMenuBar();
        if(ImGui::BeginMenu("State..."))
        {
            if(ImGui::MenuItem("Save State (F1)"))
            {
                set_emulator_save_state(true);
            }
            if(ImGui::MenuItem("Load State (F2)"))
            {
                set_emulator_load_state(true);
            }
            ImGui::Separator();
            if(ImGui::MenuItem("Reset"))
            {
                show_reset_popup = true;
            }
            ImGui::EndMenu();
        }
        if(ImGui::MenuItem("Controls..."))
        {
            m_show_controls = !m_show_controls;
        }
        if(ImGui::BeginMenu("Debug..."))
        {
            if(ImGui::MenuItem("Toggle VRAM"))
            {
                m_show_vram = !m_show_vram;
            }
            switch(m_emulator_speed)
            {
                case EmulatorSpeed::_25Percent:  { ImGui::SeparatorText("Curr. speed: 25%"); } break;
                case EmulatorSpeed::_50Percent:  { ImGui::SeparatorText("Curr. speed: 50%"); } break;
                case EmulatorSpeed::_100Percent: { ImGui::SeparatorText("Curr. speed: 100%"); } break;
                case EmulatorSpeed::Unlimited:   { ImGui::SeparatorText("Curr. speed: Unlimited"); } break;
            }
            if(ImGui::MenuItem("25% Speed"))
            {
                m_emulator_speed = EmulatorSpeed::_25Percent;
            }
            if(ImGui::MenuItem("50% Speed"))
            {
                m_emulator_speed = EmulatorSpeed::_50Percent;
            }
            if(ImGui::MenuItem("100% Speed"))
            {
                m_emulator_speed = EmulatorSpeed::_100Percent;
            }
            if(ImGui::MenuItem("Unlimited Speed"))
            {
                m_emulator_speed = EmulatorSpeed::Unlimited;
            }
            switch(m_emulator_resolution)
            {
                case PSX::RenderTarget::VRam1x: { ImGui::SeparatorText("Curr. res: 1x"); } break;
                case PSX::RenderTarget::VRam2x: { ImGui::SeparatorText("Curr. res: 2x"); } break;
            }
            if(ImGui::MenuItem("1x resolution"))
            {
                m_emulator_resolution = PSX::RenderTarget::VRam1x;
            }
            if(ImGui::MenuItem("2x resolution"))
            {
                m_emulator_resolution = PSX::RenderTarget::VRam2x;
            }
            ImGui::EndMenu();
        }
        if(ImGui::MenuItem("Hide (esc)"))
        {
            m_show_menu = false;
        }
        ImGui::EndMainMenuBar();
    }

    if(show_reset_popup)
        ImGui::OpenPopup("Reset Console?");

    if(ImGui::BeginPopupModal("Reset Console?"))
    {
        ImGui::Text("Are you really want to reset?");
        ImGui::Text("All unsaved progress will be lost");
        if(ImGui::Button("Yes"))
        {
            set_emulator_reset(true);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine(250);
        if(ImGui::Button("No"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    
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
    return m_emulator_reset;
}

/**
 * @brief check if user wants to save state of the emulator
 */
bool Menu::emulator_save_state()
{
    return m_emulator_save_state;
}

/**
 * @brief check if user wants to load state of the emulator
 */
bool Menu::emulator_load_state()
{
    return m_emulator_load_state;
}

/**
 * @brief on/off switch between showing whole vram or just the correct portion
 */
bool Menu::show_vram()
{
    return m_show_vram;
}

/**
 * @brief obtain current GPU rendering resolution 
 */
PSX::RenderTarget Menu::emulator_resolution()
{
    return m_emulator_resolution;
}

/**
 * @brief obtain current desired emulator speed scale
 */
EmulatorSpeed Menu::emulator_speed()
{
    return m_emulator_speed;
}

/**
 * @brief set emulator reset status 
 */
void Menu::set_emulator_reset(bool value)
{
    m_emulator_reset = value;
}

/**
 * @brief set emulator save state status 
 */
void Menu::set_emulator_save_state(bool value)
{
    m_emulator_save_state = value;
}

/**
 * @brief set emulator load state status 
 */
void Menu::set_emulator_load_state(bool value)
{
    m_emulator_load_state = value;
}

/**
 * @brief set current desired emulator speed scale
 */
void Menu::set_emulator_speed(EmulatorSpeed value)
{
    m_emulator_speed = value;
}

/**
 * @brief set current GPU rendering resolution 
 */
void Menu::set_emulator_resolution(PSX::RenderTarget value)
{
    m_emulator_resolution = value;
}

/**
 * @brief Process event from SDL2 to update key state and to manage menu
 */
void Menu::process_event(SDL_Event* event)
{
    if(event == nullptr)
    {
        UNREACHABLE();
    }

    switch(event->type)
    {
        case SDL_KEYDOWN:
        {
            m_last_pressed_keyboard_button = event->key.keysym.sym;
            if(event->key.keysym.sym == SDLK_ESCAPE)
            {
                m_show_menu = !m_show_menu;
            }
            if(event->key.keysym.sym == SDLK_F1)
            {
                set_emulator_save_state(true);
            }
            if(event->key.keysym.sym == SDLK_F2)
            {
                set_emulator_load_state(true);
            }
        } break;
        case SDL_KEYUP:
        {
            m_last_pressed_keyboard_button = {};
        } break;
    }
}

/**
 * @brief  
 */
void Menu::render_button_mapping(PSX::PeripheralsInput::DigitalButton button)
{
    auto [host_key, host_key_name] = m_input->get_keyboard_button_mapping_key(button);
    bool remap_popup               = false;
    auto remap_popup_name          = fmt::format("Remap {}", PSX::PeripheralsInput::DigitalButtonName[PSX::u32(button)]); 

    ImGui::NewLine();
    ImGui::SameLine(0.0f);
    ImGui::BeginDisabled();
    ImGui::Button
    (
        fmt::format("{:>8}:", PSX::PeripheralsInput::DigitalButtonName[PSX::u32(button)]).c_str()
    );
    ImGui::EndDisabled();

    ImGui::SameLine(0.0f);
    if(ImGui::Button(fmt::format("keyboard: {}", host_key_name).c_str()))
    {
        remap_popup = true;
    }

    if(remap_popup)
        ImGui::OpenPopup(remap_popup_name.c_str());

    if(ImGui::BeginPopupModal(remap_popup_name.c_str()))
    {
        ImGui::Text("Press any key...");
        if(ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        if(m_last_pressed_keyboard_button.has_value())
        {
            m_input->keyboard_button_to_button_mapping(m_last_pressed_keyboard_button.value(), button);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}