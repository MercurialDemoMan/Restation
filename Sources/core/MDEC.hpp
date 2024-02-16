/**
 * @file      MDEC.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX MDEC unit for decoding JPEG-like images
 *
 * @version   0.1
 *
 * @date      26. 10. 2023, 16:51 (created)
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

#ifndef MDEC_HPP
#define MDEC_HPP

#include <memory>
#include "Component.hpp"
#include "Forward.hpp"

namespace PSX
{
    /**
     * @brief PSX MDEC
     */
    class MDEC final : public Component
    {
    public:

        MDEC(const std::shared_ptr<Bus>& bus) :
            m_bus(bus)
        {
            
        }
        
        virtual ~MDEC() override = default;

        virtual void execute(u32 num_steps) override;
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;

    private:

        std::shared_ptr<Bus> m_bus;

    };
}

#endif // MDEC_HPP