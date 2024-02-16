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

/**
 * @brief initialize static member 
 */
std::shared_ptr<EmulatorApp> EmulatorApp::m_singleton_instance = nullptr;

/**
 * @brief initialize the app 
 */
EmulatorApp::EmulatorApp(int argc, char* argv[])
{
    MARK_UNUSED(argc);
    MARK_UNUSED(argv);

    init_frontend();
    init_backend();

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
    SDL_Init(SDL_INIT_VIDEO);
    m_window = SDL_CreateWindow
    (
        "Emulator", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        1280, 
        720, 
        SDL_WINDOW_SHOWN
    );
    m_surface = SDL_GetWindowSurface(m_window);
    m_renderer = SDL_CreateSoftwareRenderer(m_surface);
    SDL_SetRenderDrawColor(m_renderer, 0xFF, 0x00, 0x00, 0xFF);
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
            switch(event.type)
            {
                case SDL_QUIT:
                {
                    m_run = false;
                } break;
            }
        }

        SDL_RenderClear(m_renderer);

        //SDL_FillRect(m_surface, NULL, SDL_MapRGB(m_surface->format, 0xFF, 0x00, 0x00));
        SDL_UpdateWindowSurface(m_window);
    }
}