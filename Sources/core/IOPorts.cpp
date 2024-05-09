/**
 * @file      IOPorts.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation of the PSX IO Ports
 *
 * @version   0.1
 *
 * @date      5. 11. 2023, 19:34 (created)
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

#include "IOPorts.hpp"

namespace PSX
{
    u32 IOPorts::read(u32)
    {
        return 0;
    }

    void IOPorts::write(u32, u32)
    {

    }

    void IOPorts::reset()
    {
        
    }

    void IOPorts::serialize(std::shared_ptr<SaveState>&)
    {

    }

    void IOPorts::deserialize(std::shared_ptr<SaveState>&)
    {
        
    }
}