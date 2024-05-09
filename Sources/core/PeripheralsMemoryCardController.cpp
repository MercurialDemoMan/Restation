/**
 * @file      PeripheralsMemoryCardController.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for a Memory Card peripheral
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

#include "PeripheralsMemoryCardController.hpp"

namespace PSX
{
    /**
     * @brief transfer and handle 1 byte in communication sequence
     */
    u8 PeripheralsMemoryCardController::send_byte(u8)
    {
        return 0xFF;
    }

    /**
     * @brief check if controller ended communication sequence 
     */
    bool PeripheralsMemoryCardController::communication_ended() const
    {
        return true;
    }

    /**
     * @brief send acknowledge flag back
     */
    bool PeripheralsMemoryCardController::ack() const
    {
        return false;
    }

    /**
     * @brief reset any communication temporaries 
     */
    void PeripheralsMemoryCardController::reset()
    {
        
    }

    void PeripheralsMemoryCardController::serialize(std::shared_ptr<SaveState>&)
    {

    }

    void PeripheralsMemoryCardController::deserialize(std::shared_ptr<SaveState>&)
    {
        
    }
}