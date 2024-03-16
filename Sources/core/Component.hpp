/**
 * @file      Component.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Common interface for PSX hardware components
 *
 * @version   0.1
 *
 * @date      25. 10. 2023, 20:39 (created)
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

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <memory>
#include "Types.hpp"
#include "Macros.hpp"
#include "SaveState.hpp"

namespace PSX
{
    /**
     * @brief interface for psx hardware component connected to the Bus 
     */
    class Component
    {
    public:
        virtual ~Component() = default;

        /**
         * @brief execute the device for num_steps amount of clock cycles 
         */
        virtual void execute(u32 num_steps) = 0;

        /**
         * @brief read data from the device
         * 
         * @arg address relative address in the device
         * 
         * @returns read value
         */
        virtual u32  read(u32 address) = 0;

        /**
         * @brief write data itno the device
         * 
         * @arg address relative address in the device 
         * @arg value to be written
         */
        virtual void write(u32 address, u32 value) = 0;

        /**
         * @brief reset device into its' initial state
         */
        virtual void reset() = 0;

        /**
         * @brief serialize component 
         */
        virtual void serialize(std::shared_ptr<SaveState>&) = 0;

        /**
         * @brief de-serialize component 
         */
        virtual void deserialize(std::shared_ptr<SaveState>&) = 0;
    };
}

#endif // COMPONENT_HPP