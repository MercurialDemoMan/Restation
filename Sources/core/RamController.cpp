/**
 * @file      RamController.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for the PSX RAM Controller
 *
 * @version   0.1
 *
 * @date      4. 11. 2023, 12:56 (created)
 *
 * @section License
 * This file is part of the Restation \n
 *
 * Copyright (C) 2023
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

#include "RamController.hpp"

namespace PSX
{
    u32 RamController::read(u32 address)
    {
        if(in_range(address, 0u, 3u)) 
        {
            return m_ram_size.read(address);
        }

        UNREACHABLE();
    }

    void RamController::write(u32 address, u32 value)
    {
        if(in_range(address, 0u, 3u)) 
        {
            m_ram_size.write(address, static_cast<u8>(value)); return;
        }

        UNREACHABLE();
    }

    void RamController::reset()
    {
        m_ram_size.raw() = 0x00000B88;
    }

    void RamController::serialize(std::shared_ptr<SaveState>& save_state)
    {
        save_state->serialize_from(m_ram_size);
    }

    void RamController::deserialize(std::shared_ptr<SaveState>& save_state)
    {
        save_state->deserialize_to(m_ram_size);
    }
}