/**
 * @file      App.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX Emulator Frontend
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

#ifndef APP_HPP
#define APP_HPP

#include "Menu.hpp"
#include "Input.hpp"

#include "../core/Bus.hpp"
#include "../core/Macros.hpp"
#include "../core/GPUTypes.hpp"

#include <mutex>
#include <memory>
#include <thread>
#include <condition_variable>

#include <SDL.h>

/**
 * @brief Frontend for the PSX Emulator
 */
class App
{
public:

    /**
     * @brief initialize singleton instance 
     */
    static void init(int argc, char* argv[]);

    /**
     * @brief get singleton instance 
     */
    static std::shared_ptr<App> the();

    /**
     * @brief start event/rendering/emulator loop 
     */
    void run();

    /**
     * @brief destroy the app
     */
    ~App();

private:

    /**
     * @brief initialize the app 
     */
    explicit App(int argc, char* argv[]);
    
    /**
     * @brief delete copy/move to comfort to singleton pattern 
     */
    DELETE_COPY_ASSIGNMENT(App);
    DELETE_COPY_CONSTRUCTOR(App);
    DELETE_MOVE_ASSIGNMENT(App);
    DELETE_MOVE_CONSTRUCTOR(App);

    /**
     * @brief initialize window/surface
     */
    void init_frontend();

    /**
     * @brief initialize emulator core
     */
    void init_backend();

    /**
     * @brief start emulator logic loop
     *        needs to be called from separate thread
     */
    void emulator_thread();

    /**
     * @brief copy the vram contents into a framebuffer  
     */
    void update_framebuffer(SDL_Texture* framebuffer, PSX::RenderTarget resolution);

    static std::shared_ptr<App> m_singleton_instance; /// Manage singleton instance

    SDL_Window*   m_window;            /// Manage Window
    SDL_Renderer* m_renderer;          /// Manage Window Surface Renderer
    SDL_Texture*  m_framebuffer_15bit; /// Manage Window Framebuffer (15bit)
    SDL_Texture*  m_framebuffer_15bit_hires; /// Manage Window Framebuffer (15bit) 2x
    SDL_Texture*  m_framebuffer_24bit; /// Manage Window Framebuffer (24bit)
    std::atomic<bool> m_run;           /// Manage Main App Emulator/Rendering Loop
    SDL_Rect      m_framebuffer_view;  /// Cutout of VRAM

    std::shared_ptr<Input>     m_input;                                     /// SDL2 Input Manager
    std::shared_ptr<PSX::Bus>  m_emulator_core;                             /// Store Actual Emulator State
    std::array<PSX::u16, PSX::VRamWidth * PSX::VRamHeight> m_emulator_vram; /// Store Copy of Emulator VRAM
    std::array<PSX::u16, PSX::VRamWidth * PSX::VRamHiresScale * PSX::VRamHeight * PSX::VRamHiresScale> m_emulator_vram_hires; /// Store Copy of Emulator hi-res VRAM
    PSX::DisplayInfo           m_emulator_display_info;                     /// Cutout of VRAM

    std::thread             m_emulator_thread;      /// Manage Emulator Thread
    std::mutex              m_vram_mutex;           /// v
                                                    /// Manage Copying of Emulator VRAM
    std::condition_variable m_vblank_notifier;      /// ^

    std::shared_ptr<Menu> m_menu; /// Manage Menu/Emulator State
};

#endif // APP_HPP