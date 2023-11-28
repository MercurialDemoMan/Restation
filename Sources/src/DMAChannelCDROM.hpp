/**
 * @file      DMAChannelCDROM.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX 4th DMA Channel for reading from Disc
 *
 * @version   0.1
 *
 * @date      28. 11. 2023, 11:36 (created)
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

#ifndef DMACHANNELCDROM_HPP
#define DMACHANNELCDROM_HPP

#include <memory>
#include "Component.hpp"
#include "Forward.hpp"
#include "DMAChannel.hpp"
#include "DMATypes.hpp"

namespace PSX
{
    /**
     * @brief PSX 4th DMA Channel
     */
    class DMAChannelCDROM final : public DMAChannel
    {
    public:

        DMAChannelCDROM(const std::shared_ptr<Bus>& bus, const std::shared_ptr<CDROM>& cdrom) :
            DMAChannel(bus),
            m_cdrom(cdrom)
        {
            
        }
        
        virtual ~DMAChannelCDROM() override = default;
        virtual ChannelType type() const override { return ChannelType::CDROM; };

    private:

        std::shared_ptr<CDROM> m_cdrom;

    };
}

#endif // DMACHANNELCDROM_HPP