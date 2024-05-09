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

#include "InterruptController.hpp"

#include "Utils.hpp"
#include "ExceptionController.hpp"

namespace PSX
{
    u32 InterruptController::read(u32 address)
    {
        if(in_range(address, 0u, 3u))
        {
            return m_status.read(address - 0);
        }
        if(in_range(address, 4u, 7u))
        {
            return m_mask.read(address - 4);
        }

        UNREACHABLE();
    }

    void InterruptController::write(u32 address, u32 value)
    {
        if(in_range(address, 0u, 1u))
        {
            m_status.write(address - 0, m_status.read(address - 0) & value);
            m_exception_controller->set_interrupt_pending(is_interrupt_pending() ? 4 : 0);
            return;
        }
        if(in_range(address, 2u, 3u))
        {
            m_exception_controller->set_interrupt_pending(is_interrupt_pending() ? 4 : 0);
            return;
        }
        if(in_range(address, 4u, 5u))
        {
            m_mask.write(address - 4, value);
            m_exception_controller->set_interrupt_pending(is_interrupt_pending() ? 4 : 0);
            return;
        }
        if(in_range(address, 6u, 7u))
        {
            m_exception_controller->set_interrupt_pending(is_interrupt_pending() ? 4 : 0);
            return;
        }

        UNREACHABLE();
    }

    void InterruptController::reset()
    {
        m_status = 0;
        m_mask   = 0;
    }

    void InterruptController::serialize(std::shared_ptr<SaveState>& save_state)
    {
        save_state->serialize_from(m_status);
        save_state->serialize_from(m_mask);
    }

    void InterruptController::deserialize(std::shared_ptr<SaveState>& save_state)
    {
        save_state->deserialize_to(m_status);
        save_state->deserialize_to(m_mask);
    }

    /**
     * @brief check for queued up interrupt 
     */
    bool InterruptController::is_interrupt_pending()
    {
        return m_status.raw() & m_mask.raw();
    }

    /**
     * @brief send exception to the cpu
     */
    void InterruptController::trigger_interrupt(Interrupt interrupt)
    {
        static constexpr const char* interrupt_name[] =
        {
            "Vblank", 
            "GPU", 
            "CDRom", 
            "DMA", 
            "Timer0", 
            "Timer1", 
            "Timer2", 
            "Controller", 
            "SIO", 
            "SPU", 
            "Lightpen"
        };

        LOG_DEBUG(4, fmt::format("interrupt trigger: {}", interrupt_name[static_cast<u32>(interrupt)]));
        MARK_UNUSED(interrupt_name);

        m_status.raw() |= (1 << static_cast<u32>(interrupt));

        m_exception_controller->set_interrupt_pending(is_interrupt_pending() ? 4 : 0);
    }
}