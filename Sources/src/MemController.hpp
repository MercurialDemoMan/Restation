/**
 * @file      MemController.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX Memory Controller
 *
 * @version   0.1
 *
 * @date      4. 11. 2023, 12:56 (created)
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

#ifndef MEMCONTROLLER_HPP
#define MEMCONTROLLER_HPP

#include "Component.hpp"
#include "Utils.hpp"

namespace PSX
{
    /**
     * @brief PSX Memory controller 
     */
    class MemController final : public Component
    {
    public:

        MemController()
        {
            reset();
        }
        
        virtual ~MemController() override = default;

        virtual void execute(u32) override {}
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;

    private:

        Register<u32> m_expansion1_base;
        Register<u32> m_expansion2_base;
        Register<u32> m_expansion1_size;
        Register<u32> m_expansion2_size;
        Register<u32> m_expansion3_size;
        Register<u32> m_bios_size;
        Register<u32> m_spu_size;
        Register<u32> m_cdrom_size;
        Register<u32> m_common_delay;
        
    };
}

#endif // MEMCONTROLLER_HPP