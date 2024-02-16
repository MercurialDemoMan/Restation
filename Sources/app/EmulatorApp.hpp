/**
 * @file      Emulator.hpp
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

#ifndef EMULATORAPP_HPP
#define EMULATORAPP_HPP

#include <memory>
#include <SDL.h>
#include "../core/Bus.hpp"
#include "../core/Macros.hpp"

/**
 * @brief Frontend for the PSX Emulator
 */
class EmulatorApp
{
public:

    /**
     * @brief delete copy/move to comfort to singleton pattern 
     */
    DELETE_COPY_ASSIGNMENT(EmulatorApp);
    DELETE_COPY_CONSTRUCTOR(EmulatorApp);
    DELETE_MOVE_ASSIGNMENT(EmulatorApp);
    DELETE_MOVE_CONSTRUCTOR(EmulatorApp);

    /**
     * @brief initialize singleton instance 
     */
    static void init(int argc, char* argv[]);

    /**
     * @brief get singleton instance 
     */
    static std::shared_ptr<EmulatorApp> the();

    /**
     * @brief start event/rendering loop 
     */
    void run();

    /**
     * @brief destroy the app
     */
    ~EmulatorApp();

private:

    /**
     * @brief initialize the app 
     */
    explicit EmulatorApp(int argc, char* argv[]);

    /**
     * @brief initialize window/surface
     */
    void init_frontend();

    /**
     * @brief initialize emulator core
     */
    void init_backend();

    static std::shared_ptr<EmulatorApp> m_singleton_instance; /// Manage singleton instance

    SDL_Window*   m_window;      /// Manage Window
    SDL_Surface*  m_surface;     /// Manage Window Surface
    SDL_Renderer* m_renderer;    /// Manage Window Surface Renderer
    SDL_Texture*  m_framebuffer; /// Manage Window Framebuffer
    bool          m_run;         /// Manage Main App Loop

    std::shared_ptr<PSX::Bus> m_emulator_core; /// Store Actual Emulator State
};

#endif // EMULATORAPP_HPP