/**
 * @file      DMAChannelGPU.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX 3rd DMA Channel for accessing GPU
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

#ifndef DMACHANNELGPU_HPP
#define DMACHANNELGPU_HPP

#include "Forward.hpp"
#include "DMATypes.hpp"
#include "Component.hpp"
#include "DMAChannel.hpp"

#include <memory>

namespace PSX
{
    /**
     * @brief PSX 3rd DMA Channel
     */
    class DMAChannelGPU final : public DMAChannel
    {
    public:

        DMAChannelGPU(const std::shared_ptr<Bus>& bus, const std::shared_ptr<GPU>& gpu) :
            DMAChannel(bus),
            m_gpu(gpu)
        {
            
        }
        
        virtual ~DMAChannelGPU() override = default;
        virtual ChannelType type() const override { return ChannelType::GPU; };

        /**
         * @brief read from GPU
         */
        virtual u32 read_from_component() override;

        /**
         * @brief write to GPU
         */
        virtual void write_to_component(u32 value) override;

    private:

        std::shared_ptr<GPU> m_gpu;

    };
}

#endif // DMACHANNELGPU_HPP