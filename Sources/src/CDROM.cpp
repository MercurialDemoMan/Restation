/**
 * @file      CDROM.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation of the PSX CDROM
 *
 * @version   0.1
 *
 * @date      26. 10. 2023, 16:20 (created)
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

#include "CDROM.hpp"
#include "Bus.hpp"

namespace PSX
{
    void CDROM::execute(u32 num_steps)
    {
        MARK_UNUSED(num_steps);
        TODO();
    }

    u32 CDROM::read(u32 address)
    {
        MARK_UNUSED(address);
        TODO();
    }

    void CDROM::write(u32 address, u32 value)
    {
        MARK_UNUSED(address);
        MARK_UNUSED(value);
        TODO();
    }

    void CDROM::reset()
    {
        TODO();
    }
}