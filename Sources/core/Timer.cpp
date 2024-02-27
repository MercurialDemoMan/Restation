/**
 * @file      Timer.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation of the PSX Timer
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

#include "Timer.hpp"
#include "InterruptController.hpp"
#include "GPUConstants.hpp"
#include "Macros.hpp"
#include "Utils.hpp"
#include <cmath>

namespace PSX
{
    template<ClockSource Source>
    void Timer<Source>::execute(u32 num_steps)
    {
        // do not proceed whe timer paused
        if(m_meta_paused)
            return;

        // update counter
        m_meta_counter += num_steps;

        // store updated result based on the clock source
        u32 result = m_current_counter_value.raw();

        if constexpr (Source == ClockSource::DotClock)
        {
            auto actual_clock_source = m_counter_mode.clock_source & 1;
            // System Clock
            if(actual_clock_source == 0)
            {
                result        += static_cast<u32>(m_meta_counter / 1.0f);
                m_meta_counter = std::fmod(m_meta_counter, 1.0f);
            }
            // Dot Clock
            else
            {
                // TODO: get true dot clock value from gpu
                //       the modulation value is an average
                //       of all dot clock timings defined in
                //       GPUConstants.hpp
                result        += static_cast<u32>(m_meta_counter / 6.8f);
                m_meta_counter = std::fmod(m_meta_counter, 6.8f);
            }
        }

        if constexpr (Source == ClockSource::HBlank)
        {
            auto actual_clock_source = m_counter_mode.clock_source & 1;
            // System Clock
            if(actual_clock_source == 0)
            {
                result        += static_cast<u32>(m_meta_counter / 1.0f);
                m_meta_counter = std::fmod(m_meta_counter, 1.0f);
            }
            // HBlank Clock
            else
            {
                // TODO: get gpu mode (PAL/NTSC) from the gpu
                //       for now assume we have NTSC mode
                result        += static_cast<u32>(m_meta_counter / NTSCScanlineCycles);
                m_meta_counter = std::fmod(m_meta_counter, NTSCScanlineCycles);
            }
        }

        if constexpr (Source == ClockSource::SystemClock)
        {
            auto actual_clock_source = (m_counter_mode.clock_source >> 1) & 1;
            // System Clock
            if(actual_clock_source == 0)
            {
                result        += static_cast<u32>(m_meta_counter / 1.0f);
                m_meta_counter = std::fmod(m_meta_counter, 1.0f);
            }
            // System Clock / 8
            else
            {
                result        += static_cast<u32>(m_meta_counter / 8.0f);
                m_meta_counter = std::fmod(m_meta_counter, 8.0f);
            }
        }

        // timer reached target
        if(result >= m_counter_target_value.raw())
        {
            m_counter_mode.reached_target_value = 1;

            // reset counter to zero when reached target
            if(m_counter_mode.reset_counter_to_zero == 1)
            {
                if(m_counter_target_value.raw() != 0)
                    result %= m_counter_target_value.raw();
                else
                    result = 0;
            }

            if(m_counter_mode.irq_when_counter_eq_target)
            {
                trigger_interrupt_request();
            }
        }

        // timer reached ffff
        if(result >= 0xFFFF)
        {
            m_counter_mode.reached_ffff_value = 1;

            // reset counter to zero when reached ffff
            if(m_counter_mode.reset_counter_to_zero == 0)
            {
                result %= 0xFFFF;
            }

            if(m_counter_mode.irq_when_counter_eq_ffff)
            {
                trigger_interrupt_request();
            }
        }

        // update counter value
        m_current_counter_value.raw() = static_cast<u32>(result) & 0xFFFF;
    }

    template<ClockSource Source>
    u32 Timer<Source>::read(u32 address)
    {
        if(in_range(address, 0u, 3u))
        {
            return m_current_counter_value.read(address - 0);
        }
        if(in_range(address, 4u, 7u))
        {
            u32 current_mode = m_counter_mode.bytes[address - 4];
            // bits 11,12 get reset after read
            if(address == 5)
            {
                m_counter_mode.reached_ffff_value   = false;
                m_counter_mode.reached_target_value = false;
            }
            return current_mode;
        }

        if(in_range(address, 8u, 11u))
        {
            return m_counter_target_value.read(address - 8);
        }

        UNREACHABLE();
    }

    template<ClockSource Source>
    void Timer<Source>::write(u32 address, u32 value)
    {
        if(in_range(address, 0u, 3u))
        {
            m_current_counter_value.write(address - 0, value); return;
        }
        if(in_range(address, 4u, 7u))
        {
            // reset counter state
            m_meta_paused = false;
            m_current_counter_value.raw() = 0;

            m_counter_mode.bytes[address - 4] = value;

            // update meta flags
            if(address == 5)
            {
                m_meta_irq_occured = false;
                m_counter_mode.interrupt_request = true;

                if(m_counter_mode.synchronization_enabled)
                {
                    if constexpr (Source == ClockSource::DotClock)
                    {
                        // pause until hblank and freerun
                        if(m_counter_mode.synchronization_mode == 3)
                        {
                            m_meta_paused = true;
                        }

                        u32 mode = m_counter_mode.synchronization_enabled;

                        LOG_DEBUG(4, fmt::format("timer0 set sync {}", mode));
                        MARK_UNUSED(mode);
                    }

                    if constexpr (Source == ClockSource::HBlank)
                    {
                        // pause until vblank and freerun
                        if(m_counter_mode.synchronization_mode == 3)
                        {
                            m_meta_paused = true;
                        }

                        u32 mode = m_counter_mode.synchronization_enabled;

                        LOG_DEBUG(4, fmt::format("timer1 set sync {}", mode));
                        MARK_UNUSED(mode);
                    }

                    if constexpr (Source == ClockSource::SystemClock)
                    {
                        // stop counter
                        if(m_counter_mode.synchronization_mode == 0 ||
                            m_counter_mode.synchronization_mode == 3)
                        {
                            m_meta_paused = true;
                        }

                        u32 mode = m_counter_mode.synchronization_enabled;

                        LOG_DEBUG(4, fmt::format("timer2 set sync {}", mode));
                        MARK_UNUSED(mode);
                    }
                }
            }

            return;
        }
        if(in_range(address, 8u, 11u))
        {
            m_counter_target_value.write(address - 8, value); return;
        }

        UNREACHABLE();
    }

    template<ClockSource Source>
    void Timer<Source>::reset()
    {
        m_current_counter_value.raw()    = 0;
        m_counter_mode.raw               = 0;
        m_counter_mode.interrupt_request = 1;
        m_counter_target_value.raw()     = 0;
        m_meta_counter                   = 0;
        m_meta_paused                    = false;
        m_meta_irq_occured               = false;
    }

    /**
     * @brief set interrupt request flag 
     */
    template<ClockSource Source>
    void Timer<Source>::trigger_interrupt_request()
    {
        // toggle interrupt request
        if(m_counter_mode.irq_toggle_mode == 1)
        {
            m_counter_mode.interrupt_request = !m_counter_mode.interrupt_request;
        }
        // short pulse
        else
        {
            m_counter_mode.interrupt_request = 0;
        }

        // one shot
        if(m_counter_mode.irq_repeat_mode == 0 && m_meta_irq_occured)
        {
            return;
        }

        // if there isn't an already pending interrupt request
        if(m_counter_mode.interrupt_request == false)
        {
            if constexpr (Source == ClockSource::DotClock)
            {
                m_interrupt_controller->trigger_interrupt(Interrupt::Timer0);
            }
            if constexpr (Source == ClockSource::HBlank)
            {
                m_interrupt_controller->trigger_interrupt(Interrupt::Timer1);
            }
            if constexpr (Source == ClockSource::SystemClock)
            {
                m_interrupt_controller->trigger_interrupt(Interrupt::Timer2);
            }

            m_meta_irq_occured = true;
        }

        m_counter_mode.interrupt_request = true;
    }

    /**
     * instantiate all timers
     */
    template class Timer<ClockSource::DotClock>;
    template class Timer<ClockSource::HBlank>;
    template class Timer<ClockSource::SystemClock>;
}