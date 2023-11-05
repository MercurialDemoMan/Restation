/**
 * @file      ExceptionController.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation of the PSX CPU Exception Controller
 *
 * @version   0.1
 *
 * @date      5. 11. 2023, 14:01 (created)
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

#include "ExceptionController.hpp"

namespace PSX
{
    void ExceptionController::execute(u32 num_steps)
    {
        MARK_UNUSED(num_steps);
        TODO();
    }

    u32 ExceptionController::read(u32 address)
    {
        MARK_UNUSED(address);
        TODO();
    }

    void ExceptionController::write(u32 address, u32 value)
    {
        MARK_UNUSED(address);
        MARK_UNUSED(value);
        TODO();
    }

    void ExceptionController::reset()
    {
        m_bpc       = 0;
        m_bda       = 0;
        m_jumpdest  = 0;
        m_dcic.raw  = 0;
        m_bad_vaddr = 0;
        m_bdam      = 0;
        m_bpcm      = 0;
        m_sr.raw    = 0;
        m_cause.raw = 0;
        m_epc       = 0;
        m_prid      = 2;
    }
}