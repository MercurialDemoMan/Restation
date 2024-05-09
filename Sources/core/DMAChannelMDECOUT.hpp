/**
 * @file      DMAChannelMDECOUT.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX 2nd DMA channel for sending MDEC output data
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

#ifndef DMACHANNELMDECOUT_HPP
#define DMACHANNELMDECOUT_HPP

#include "Forward.hpp"
#include "DMATypes.hpp"
#include "Component.hpp"
#include "DMAChannel.hpp"

#include <memory>

namespace PSX
{
    /**
     * @brief PSX 2nd DMA Channel
     */
    class DMAChannelMDECOUT final : public DMAChannel
    {
    public:

        DMAChannelMDECOUT(const std::shared_ptr<Bus>& bus, const std::shared_ptr<MDEC>& mdec) :
            DMAChannel(bus),
            m_mdec(mdec)
        {
            
        }
        
        virtual ~DMAChannelMDECOUT() override = default;
        virtual ChannelType type() const override { return ChannelType::MDECOUT; };

        /**
         * @brief read from MDEC 
         */
        virtual u32 read_from_component() override;

        /**
         * @brief wait for MDEC output
         */
        virtual bool sync_request() const override;

        /**
         * @brief interleave block sync copy 
         */
        virtual bool interleaved_copy() const override;
        
    private:

        std::shared_ptr<MDEC> m_mdec;

    };
}

#endif // DMACHANNELMDECOUT_HPP