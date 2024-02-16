/**
 * @file      Emulator.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for the PSX Emulator Frontend
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

#include "EmulatorApp.hpp"

#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>

/**
 * @brief initialize static member 
 */
std::shared_ptr<EmulatorApp> EmulatorApp::m_singleton_instance = nullptr;

/**
 * @brief initialize the app 
 */
EmulatorApp::EmulatorApp(int argc, char* argv[])
{
    init_frontend();
    init_backend();

    //TODO: make argument handling better
    if(argc > 1) m_emulator_core->meta_load_bios(argv[1]);

    // keep main loop running
    m_run = true;
}

/**
 * @brief destroy the app 
 */
EmulatorApp::~EmulatorApp()
{
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

/**
 * @brief initialize window/surface
 */
void EmulatorApp::init_frontend()
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        ABORT_WITH_MESSAGE(fmt::format("couldn't initialize SDL2 video component: {}", SDL_GetError()));
    }

    m_window = SDL_CreateWindow
    (
        "Emulator", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        1024, 
        720, 
        SDL_WINDOW_SHOWN
    );
    if(m_window == nullptr)
    {
        ABORT_WITH_MESSAGE(fmt::format("couldn't open window: {}", SDL_GetError()));
    }

    m_surface = SDL_GetWindowSurface(m_window);
    if(m_surface == nullptr)
    {
        ABORT_WITH_MESSAGE(fmt::format("couldn't retrieve window surface: {}", SDL_GetError()));
    }

    m_renderer = SDL_CreateSoftwareRenderer(m_surface);
    if(m_renderer == nullptr)
    {
        ABORT_WITH_MESSAGE(fmt::format("couldn't create software renderer: {}", SDL_GetError()));
    }

    if(SDL_RenderSetLogicalSize(m_renderer, PSX::VRamWidth, PSX::VRamHeight) != 0)
    {
        ABORT_WITH_MESSAGE(fmt::format("couldn't set logical size to renderer: {}", SDL_GetError()));
    }
    
    if(SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0x00, 0xFF) != 0)
    {
        ABORT_WITH_MESSAGE(fmt::format("couldn't set background draw color: {}", SDL_GetError()));
    }

    m_framebuffer = SDL_CreateTexture
    (
        m_renderer, 
        SDL_PIXELFORMAT_ABGR1555,
        SDL_TEXTUREACCESS_STREAMING, 
        PSX::VRamWidth, 
        PSX::VRamHeight
    );
    if(m_framebuffer == nullptr)
    {
        ABORT_WITH_MESSAGE(fmt::format("couldn't create framebuffer texture: {}", SDL_GetError()));
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer2_Init(m_renderer);
}

/**
 * @brief initialize emulator core
 */
void EmulatorApp::init_backend()
{
    m_emulator_core = PSX::Bus::create();
}

/**
 * @brief initialize singleton instance 
 */
void EmulatorApp::init(int argc, char* argv[])
{
    if(m_singleton_instance)
    {
        ABORT_WITH_MESSAGE("trying to initialize already initialized EmulatorApp");
    }

    m_singleton_instance = std::shared_ptr<EmulatorApp>(new EmulatorApp(argc, argv));
}

/**
 * @brief get singleton instance 
 */
std::shared_ptr<EmulatorApp> EmulatorApp::the()
{
    if(!m_singleton_instance)
    {
        ABORT_WITH_MESSAGE("trying to access uninitialized EmulatorApp, call EmulatorApp::init(argc, argv) first");
    }

    return m_singleton_instance;
}

/**
 * @brief start event/rendering loop 
 */
void EmulatorApp::run()
{
    while(m_run)
    {
        SDL_Event event = {0};
        while(SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            switch(event.type)
            {
                case SDL_QUIT:
                {
                    m_run = false;
                } break;
            }
        }

        // emulate system until gpu finishes rendering a frame
        while(!m_emulator_core->meta_vblank())
        {
            m_emulator_core->execute(PSX::Bus::OptimalSimulationStep);
        }
        
        // copy gpu vram content to SDL texture
        {
            const auto& vram_buffer = m_emulator_core->meta_get_vram_buffer();

            void* framebuffer_pixels = nullptr;
            int   framebuffer_pitch = 0;
            
            if(SDL_LockTexture(m_framebuffer, NULL, &framebuffer_pixels, &framebuffer_pitch) != 0)
            {
                LOG(fmt::format("failed to lock framebuffer texture: {}", SDL_GetError()));
            }
            else
            {
                std::memcpy(framebuffer_pixels, vram_buffer.data(), framebuffer_pitch * PSX::VRamHeight);
            }
            SDL_UnlockTexture(m_framebuffer);
        }
        
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // render vram
        SDL_RenderClear(m_renderer);
        SDL_RenderCopy(m_renderer, m_framebuffer, NULL, NULL);

        // render menu
        ImGui::BeginMainMenuBar();
        if(ImGui::BeginMenu("Test 1"))
        {
            if(ImGui::MenuItem("Test 2"))
            {
                LOG("Test 3");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

        SDL_UpdateWindowSurface(m_window);
    }
}