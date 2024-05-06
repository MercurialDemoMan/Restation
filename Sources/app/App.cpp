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

#include "../core/GPU.hpp"

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
    if(argc > 2) m_emulator_core->meta_load_game(argv[2]);

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
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
    {
        ABORT_WITH_MESSAGE(fmt::format("couldn't initialize SDL2 video component: {}", SDL_GetError()));
    }

    m_window = SDL_CreateWindow
    (
        "Restation", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        PSX::VRamWidth, 
        PSX::VRamHeight, 
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

    m_framebuffer_15bit = SDL_CreateTexture
    (
        m_renderer, 
        // TODO: The alpha component could be problematic, since psx uses it for masking,
        //       but for now it will save us manual conversion hurdle. Any form of blending 
        //       should be disabled by the SDL_SetRenderDrawBlendMode function a bit lower.
        SDL_PIXELFORMAT_ABGR1555,
        SDL_TEXTUREACCESS_STREAMING, 
        PSX::VRamWidth, 
        PSX::VRamHeight
    );
    if(m_framebuffer_15bit == nullptr)
    {
        ABORT_WITH_MESSAGE(fmt::format("couldn't create 15bit framebuffer texture: {}", SDL_GetError()));
    }

    m_framebuffer_24bit = SDL_CreateTexture
    (
        m_renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING, 
        // TODO: a bit of SDL magic is going here... 
        //       we have 1MiB of VRAM and we need to map it onto a 24bit packed texture
        //       however, this is not possible by only specifying width and height 
        //       represented as an integer, because the fractional part gets thrown out:
        //       
        //       vram size:           (1024 * 2 * 512)                = 1048576 = 1MiB
        //       24 bit texture size: int(1024 * 2.0 / 3.0) * 3 * 512 = 1047552
        //       
        //       Luckily for us however, when specifying RGB texture as SDL_TEXTUREACCESS_STREAMING,
        //       the pitch (byte width of the texture) has to be 4byte aligned, which means
        //       the whole texture will be padded by 2 bytes for every scanline.
        //       This means the padding will precisely makes this texture aligned with the VRAM:
        //
        //       real 24 bit texture size: (int(1024 * 2.0 / 3.0) * 3 + 2) * 512 = 1048576 = 1MiB
        //
        //       a better solution would be to manually manage a buffer and convert the formats
        //       but this will suffice for now
        PSX::VRamWidth * (2.0 / 3.0),
        PSX::VRamHeight
    );
    if(m_framebuffer_24bit == nullptr)
    {
        ABORT_WITH_MESSAGE(fmt::format("couldn't create 24bit framebuffer texture: {}", SDL_GetError()));
    }

    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer2_Init(m_renderer);

    m_input = Input::create();
    m_menu = Menu::create(m_input);
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
                m_input->process_event(&event);
                m_menu->process_event(&event);

                switch(event.type)
                {
                    case SDL_QUIT:
                    {
                        m_run = false;
                    } break;
                }
            }
        }

        // emulate system until gpu finishes rendering a frame
        // and wait until the emulator thread sends the vram content into m_emulator_vram
        {
            std::unique_lock lock(m_vram_mutex);
            static constexpr auto VBlankMaxWaitTime = std::chrono::duration_cast<std::chrono::milliseconds>
            (
                std::chrono::duration<float>
                (
                    (1.0f / PSX::PALWithoutInterlaceRefreshRate)
                )
            );
            m_vblank_notifier.wait_for(lock, VBlankMaxWaitTime);
        }

        // obtain current display area color depth
        PSX::DisplayInfo current_display_info;
        {
            std::lock_guard lock(m_vram_mutex);
            current_display_info = m_emulator_display_info;
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
            if(current_display_info.enabled)
            {
                SDL_Texture* current_framebuffer = nullptr;

                switch(current_display_info.color_depth)
                {
                    case PSX::DisplayAreaColorDepth::Depth15Bit:
                    {
                        current_framebuffer = m_framebuffer_15bit;
                    } break;
                    case PSX::DisplayAreaColorDepth::Depth24Bit:
                    {
                        current_framebuffer = m_framebuffer_24bit;
                    } break;
                    default:
                    {
                        UNREACHABLE();
                    } break;
                }

                update_framebuffer(current_framebuffer);
                if(m_menu->show_vram())
                {
                    SDL_RenderCopy(m_renderer, current_framebuffer, NULL, NULL);
                }
                else
                {
                    SDL_RenderCopy(m_renderer, current_framebuffer, &m_framebuffer_view, NULL);
                }
            }
        }

        // render menu
        {
            m_menu->render_and_update();
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
        if(m_menu->emulator_reset())
        {
            m_emulator_core->reset();
            m_menu->set_emulator_reset(false);
        }

        // TODO: make game specific save states
        if(m_menu->emulator_save_state())
        {
            auto save_state = PSX::SaveState::create();
            m_emulator_core->serialize(save_state);
            save_state->write_to_file("global_state.bin");
            m_menu->set_emulator_save_state(false);
        }

        // TODO: make game specific save states
        if(m_menu->emulator_load_state())
        {
            auto save_state = PSX::SaveState::create();
            save_state->read_from_file("global_state.bin");
            m_emulator_core->deserialize(save_state);
            m_menu->set_emulator_load_state(false);
        }

        // start timing frame
        auto start_timestamp = std::chrono::high_resolution_clock::now();

        // emulate system until gpu finishes rendering a frame
        m_emulator_core->meta_run_until_vblank();

        // end timing frame
        auto end_timestamp  = std::chrono::high_resolution_clock::now();
        PSX::s64 frame_time = std::chrono::duration_cast<std::chrono::microseconds>(end_timestamp - start_timestamp).count();

        PSX::s64 desired_frame_time = 1'000'000.0 / (m_emulator_core->meta_refresh_rate());
        
        switch(m_menu->emulator_speed())
        {
            case EmulatorSpeed::_25Percent:  { desired_frame_time *= 4; } break;
            case EmulatorSpeed::_50Percent:  { desired_frame_time *= 2; } break;
            case EmulatorSpeed::_100Percent: { desired_frame_time *= 1; } break;
            case EmulatorSpeed::Unlimited:   { desired_frame_time *= 0; } break;
        }

        // limit frame time based on the gpu mode
        if(frame_time < desired_frame_time)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(desired_frame_time - frame_time));
        }
            
        // copy vram to rendering thread
        {
            std::lock_guard lock(m_vram_mutex);
            m_emulator_vram         = m_emulator_core->meta_get_vram_buffer();
            m_emulator_display_info = m_emulator_core->meta_get_display_info();
        }

        // inform rendering thread about finished frame
        m_vblank_notifier.notify_one();
    }
}

/**
 * @brief copy the vram contents into a framebuffer  
 */
void App::update_framebuffer(SDL_Texture* framebuffer)
{
    void* framebuffer_pixels = nullptr;
    int   framebuffer_pitch  = 0;
    
    if(SDL_LockTexture(framebuffer, NULL, &framebuffer_pixels, &framebuffer_pitch) != 0)
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
                    "framebuffer texture format doesn't match with the internal size of vram [{} * {}] != [{} * {} * {}]", 
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
        m_framebuffer_view.x = m_emulator_display_info.start_x;
        m_framebuffer_view.y = m_emulator_display_info.start_y;
        m_framebuffer_view.w = m_emulator_display_info.width;
        m_framebuffer_view.h = m_emulator_display_info.height;
    }

    SDL_UnlockTexture(framebuffer);
}