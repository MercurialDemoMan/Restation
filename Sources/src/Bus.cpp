/**
 * @file      Bus.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation of PSX Bus
 *
 * @version   0.1
 *
 * @date      26. 10. 2023, 15:46 (created)
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

#include "Bus.hpp"
#include "CPU.hpp"
#include "GPU.hpp"
#include "SPU.hpp"
#include "Timer.hpp"

namespace PSX
{
    std::shared_ptr<Bus> Bus::create()
    {
        auto bus = std::shared_ptr<Bus>(new Bus());
        bus->initialize_components();
        return bus;
    }

    void Bus::initialize_components()
    {
        m_cpu = std::make_shared<CPU>(shared_from_this());
        m_gpu = std::make_shared<GPU>(shared_from_this());
        m_spu = std::make_shared<SPU>(shared_from_this());
        m_timer = std::make_shared<Timer>(shared_from_this());
    }
}