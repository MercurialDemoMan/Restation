/**
 * @file      App.cpp
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

#include "App.hpp"

#include <chrono>
#include <thread>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>

/**
 * @brief initialize static member 
 */
std::shared_ptr<App> App::m_singleton_instance = nullptr;

/**
 * @brief initialize the app 
 */
App::App(int argc, char* argv[])
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
App::~App()
{
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

/**
 * @brief initialize window/surface
 */
void App::init_frontend()
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
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if(m_window == nullptr)
    {
        ABORT_WITH_MESSAGE(fmt::format("couldn't open window: {}", SDL_GetError()));
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if(m_renderer == nullptr)
    {
        ABORT_WITH_MESSAGE(fmt::format("couldn't create software renderer: {}", SDL_GetError()));
    }
    
    if(SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0x00, 0xFF) != 0)
    {
        ABORT_WITH_MESSAGE(fmt::format("couldn't set background draw color: {}", SDL_GetError()));
    }

    m_framebuffer = SDL_CreateTexture
    (
        m_renderer, 
        // TODO: The alpha component could be problematic, since psx uses it for masking,
        //       but for now it will save us manual conversion hurdle. Any form of blending 
        //       should be disabled by the SDL_SetRenderDrawBlendMode function.
        SDL_PIXELFORMAT_ABGR1555,
        SDL_TEXTUREACCESS_STREAMING, 
        PSX::VRamWidth, 
        PSX::VRamHeight
    );
    if(m_framebuffer == nullptr)
    {
        ABORT_WITH_MESSAGE(fmt::format("couldn't create framebuffer texture: {}", SDL_GetError()));
    }

    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer2_Init(m_renderer);

    m_menu.reset();
    m_input = Input::create();
}

/**
 * @brief initialize emulator core
 */
void App::init_backend()
{
    m_emulator_core = PSX::Bus::create(m_input);
}

/**
 * @brief initialize singleton instance 
 */
void App::init(int argc, char* argv[])
{
    if(m_singleton_instance)
    {
        ABORT_WITH_MESSAGE("trying to initialize already initialized App");
    }

    m_singleton_instance = std::shared_ptr<App>(new App(argc, argv));
}

/**
 * @brief get singleton instance 
 */
std::shared_ptr<App> App::the()
{
    if(!m_singleton_instance)
    {
        ABORT_WITH_MESSAGE("trying to access uninitialized App, call App::init(argc, argv) first");
    }

    return m_singleton_instance;
}

/**
 * @brief start event/rendering/emulator loop 
 */
void App::run()
{
    // launch emulator thread
    m_emulator_thread = std::thread(&App::emulator_thread, this);

    // launch event/rendering loop
    while(m_run)
    {
        // handle events
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
                    case SDL_KEYDOWN:
                    {
                        PSX::u32 scancode = event.key.keysym.scancode;
                        PSX::u32 keycode  = event.key.keysym.sym;
                        MARK_UNUSED(scancode);
                        m_input->update_key_state(keycode, 1);
                    } break;
                    case SDL_KEYUP:
                    {
                        PSX::u32 scancode = event.key.keysym.scancode;
                        PSX::u32 keycode  = event.key.keysym.sym;
                        MARK_UNUSED(scancode);
                        m_input->update_key_state(keycode, 0);
                    } break;
                }
            }
        }

        // emulate system until gpu finishes rendering a frame
        // and wait until the emulator thread sends the vram content into m_emulator_vram
        {
            std::unique_lock lock(m_vram_mutex);
            using namespace std::chrono_literals;
            m_vblank_notifier.wait_for(lock, 20ms);
        }
        
        // copy gpu vram content to SDL texture
        {
            void* framebuffer_pixels = nullptr;
            int   framebuffer_pitch  = 0;
            
            if(SDL_LockTexture(m_framebuffer, NULL, &framebuffer_pixels, &framebuffer_pitch) != 0)
            {
                LOG(fmt::format("failed to lock framebuffer texture: {}", SDL_GetError()));
            }
            else
            {
                if(framebuffer_pitch * PSX::VRamHeight != PSX::VRamWidth * PSX::VRamHeight * sizeof(PSX::u16))
                {
                    ABORT_WITH_MESSAGE
                    (
                        fmt::format
                        (
                            "the framebuffer texture format doesn't match with the internal size of vram [{} * {}] != [{} * {} * {}]", 
                            framebuffer_pitch, 
                            PSX::VRamHeight,
                            PSX::VRamWidth,
                            PSX::VRamHeight,
                            sizeof(PSX::u16)
                        )
                    );
                }
                std::lock_guard lock(m_vram_mutex);
                std::memcpy(framebuffer_pixels, m_emulator_vram.data(), framebuffer_pitch * PSX::VRamHeight);
            }
            SDL_UnlockTexture(m_framebuffer);
        }
        
        // clear framebuffers
        {
            ImGui_ImplSDLRenderer2_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();
            SDL_RenderClear(m_renderer);
        }

        // render vram
        {
            SDL_RenderCopy(m_renderer, m_framebuffer, NULL, NULL);
        }

        // render menu
        {
            m_menu.render_and_update();
        }

        // send to screen
        {
            ImGui::Render();
            ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
            SDL_RenderPresent(m_renderer);
        }
    }

    m_emulator_thread.join();
}

/**
 * @brief start emulator logic loop
 *        needs to be called from separate thread
 */
void App::emulator_thread()
{
    while(m_run)
    {
        if(m_menu.emulator_reset())
        {
            m_emulator_core->reset();
            m_menu.set_emulator_reset(false);
        }

        // start timing frame
        auto start_timestamp = std::chrono::high_resolution_clock::now();

        // emulate system until gpu finishes rendering a frame
        while(!m_emulator_core->meta_vblank())
        {
            m_emulator_core->execute(PSX::Bus::OptimalSimulationStep);
        }

        // end timing frame
        auto end_timestamp  = std::chrono::high_resolution_clock::now();
        PSX::s64 frame_time = std::chrono::duration_cast<std::chrono::microseconds>(end_timestamp - start_timestamp).count();
        PSX::s64 desired_frame_time = 1'000'000.0f / m_emulator_core->meta_refresh_rate(); 

        // limit frame time based on the gpu mode
        if(frame_time < desired_frame_time)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(desired_frame_time - frame_time));
        }
            
        // copy vram to rendering thread
        {
            std::lock_guard lock(m_vram_mutex);
            m_emulator_vram = m_emulator_core->meta_get_vram_buffer();
        }

        // inform rendering thread about finished frame
        m_vblank_notifier.notify_one();
    }
}