/**
 * @file      DMAChannelGPU.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for the PSX 3rd DMA Channel for accessing GPU
 *
 * @version   0.1
 *
 * @date      28. 11. 2023, 11:36 (created)
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
#include "DMAChannelGPU.hpp"

#include "Bus.hpp"
#include "GPU.hpp"

namespace PSX
{
    /**
     * @brief read from GPU
     */
    u32 DMAChannelGPU::read_from_component()
    {
        return m_gpu->read(0);
    }

    /**
     * @brief write to GPU
     */
    void DMAChannelGPU::write_to_component(u32 value)
    {
        m_gpu->write(0, value);
    }
}