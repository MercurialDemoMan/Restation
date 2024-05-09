/**
 * @file      SPU.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation of the PSX Sound processing unit
 *
 * @version   0.1
 *
 * @date      10. 11. 2023, 20:01 (created)
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

#include "SerialPort.hpp"

namespace PSX
{
    void SerialPort::execute(u32 num_steps)
    {
        MARK_UNUSED(num_steps);
        TODO();
    }

    u32 SerialPort::read(u32 address)
    {
        MARK_UNUSED(address);
        TODO();
    }

    void SerialPort::write(u32 address, u32 value)
    {
        MARK_UNUSED(address);
        MARK_UNUSED(value);
        TODO();
    }

    void SerialPort::reset()
    {
        
    }

    void SerialPort::serialize(std::shared_ptr<SaveState>&)
    {

    }
    
    void SerialPort::deserialize(std::shared_ptr<SaveState>&)
    {

    }
}