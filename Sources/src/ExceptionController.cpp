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
    u32 ExceptionController::read(u32 address)
    {
        switch(address)
        {
            case 3:  { return m_bpc; }
            case 5:  { return m_bda; }
            case 6:  { return m_jumpdest; }
            case 7:  { return m_dcic.raw; }
            case 8:  { return m_bad_vaddr; }
            case 9:  { return m_bdam; }
            case 11: { return m_bpcm; }
            case 12: { return m_sr.raw; }
            case 13: { return m_cause.raw; }
            case 14: { return m_epc; }
            case 15: { return m_prid; }
        }

        UNREACHABLE();
    }

    void ExceptionController::write(u32 address, u32 value)
    {
        switch(address)
        {
            case 3:  { m_bpc = value;      return; }
            case 5:  { m_bda = value;      return; }
            case 7:  { m_dcic.raw = value; return; }
            case 9:  { m_bdam = value;     return; }
            case 11: { m_bdam = value;     return; }
            case 12: { m_sr.raw = value;   return; }
            // cause is read only except for bits 8-9
            case 13: 
            {
                m_cause.interrupt_pending &= 0b1111'1100; // TODO: potentially missunderstood
                m_cause.interrupt_pending |= (value & 0b0011'0000'0000) >> 8;
                break;
            }
            case 14: { m_epc = value; return; }
        }

        UNREACHABLE();
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

    /**
     * @brief update history of exceptions
     */
    void ExceptionController::return_from_exception()
    {
        m_sr.current_interrupt_enable   = m_sr.previous_interrupt_disable;
        m_sr.current_execution_mode     = m_sr.previous_execution_mode;
        m_sr.previous_interrupt_disable = m_sr.old_interrupt_disable;
        m_sr.previous_execution_mode    = m_sr.old_execution_mode;
    }
}