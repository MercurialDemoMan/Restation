/**
 * @file      PeripheralsNoneController.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for a disconnected controller peripheral
 *
 * @version   0.1
 *
 * @date      17. 2. 2024 (created)
 *
 * @section   TODO: replace with actual documentation
 * TODO: documentation text
 *
 * @section License
 * This file is part of the TODO: project \n
 *
 * Copyright (C) 2024
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

#include "PeripheralsEmptyController.hpp"
#include "Macros.hpp"

namespace PSX
{
    PeripheralsEmptyController::PeripheralsEmptyController(const std::shared_ptr<PeripheralsInput>&)
    {
    }

    /**
     * @brief transfer and handle 1 byte in communication sequence
     */
    u8 PeripheralsEmptyController::send_byte(u8 byte)
    {
        return 0xFF;
    }

    /**
     * @brief check if controller ended communication sequence 
     */
    bool PeripheralsEmptyController::communication_ended() const
    {
        return true;
    }

    /**
     * @brief send acknowledge flag back
     */
    bool PeripheralsEmptyController::ack() const
    {
        return false;
    }

    /**
     * @brief reset any communication temporaries 
     */
    void PeripheralsEmptyController::reset()
    {
    }

    void PeripheralsEmptyController::serialize(std::shared_ptr<SaveState>&)
    {
    }

    void PeripheralsEmptyController::deserialize(std::shared_ptr<SaveState>&)
    {
    }
}