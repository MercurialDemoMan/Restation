

/**
 * @file      DMAChannelMDECIN.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for the PSX 1st DMA channel for supplying MDEC with input data
 *
 * @version   0.1
 *
 * @date      27. 11. 2023, 17:36 (created)
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

#include "DMAChannelMDECIN.hpp"

#include "Bus.hpp"
#include "MDEC.hpp"

namespace PSX
{
    /**
     * @brief write to MDEC 
     */
    void DMAChannelMDECIN::write_to_component(u32 value)
    {
        m_mdec->write(0, value);
    }

    /**
     * @brief wait for MDEC 
     */
    bool DMAChannelMDECIN::sync_request() const
    {
        return m_mdec->is_input_fifo_ready();
    }
}