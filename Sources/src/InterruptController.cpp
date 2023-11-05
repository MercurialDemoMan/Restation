/**
 * @file      InterruptController.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation of the PSX Interrupt controller
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

#include "InterruptController.hpp"
#include "ExceptionController.hpp"
#include <fmt/core.h>

namespace PSX
{
    void InterruptController::execute(u32 num_steps)
    {
        m_exception_controller->set_interrupt_pending(is_interrupt_pending() ? 0b1111 : 0);
    }

    u32 InterruptController::read(u32 address)
    {
        switch(address)
        {
            case 0 ... 1:
            {
                return m_status.read(address - 0);
            }
            case 4 ... 5:
            {
                return m_mask.read(address - 4);
            }
        }

        UNREACHABLE();
    }

    void InterruptController::write(u32 address, u32 value)
    {
        switch(address)
        {
            case 0 ... 3:
            {
                m_status.write(address - 0, m_status.read(address - 0) & value);
                execute(1);
                return;
            }

            case 4 ... 7:
            {
                m_mask.write(address - 4, value);
                execute(1);
                return;
            }
        }

        UNREACHABLE();
    }

    void InterruptController::reset()
    {
        m_status = 0;
        m_mask   = 0;
    }
    
    /**
     * @brief check for queued up interrupt 
     */
    bool InterruptController::is_interrupt_pending()
    {
        return m_status.raw() & m_mask.raw();
    }
}