/**
 * @file      PeripheralsController.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Interface class for representing a controller peripheral
 *
 * @version   0.1
 *
 * @date      17. 2. 2024 (created)
 *
 * @section License
 * This file is part of the Restation \n
 *
 * Copyright (C) 2024
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

#ifndef PERIPHERALSCONTROLLER_HPP
#define PERIPHERALSCONTROLLER_HPP

#include "Types.hpp"
#include "SaveState.hpp"
#include "PeripheralsInput.hpp"

#include <memory>

namespace PSX
{
    /**
     * @brief interface for psx peripheral controller
     */
    class PeripheralsController
    {
    public:

        /**
         * @brief transfer and handle 1 byte in communication sequence
         */
        virtual u8 send_byte(u8) = 0;

        /**
         * @brief check if controller ended communication sequence 
         */
        virtual bool communication_ended() const = 0;

        /**
         * @brief send acknowledge flag back
         */
        virtual bool ack() const = 0;

        /**
         * @brief reset any communication temporaries 
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

#endif // PERIPHERALSCONTROLLER_HPP