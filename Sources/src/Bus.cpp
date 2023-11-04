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
#include "MDEC.hpp"
#include "CDROM.hpp"
#include "Timer.hpp"
#include "RamControl.hpp"
#include "MemControl.hpp"
#include "Peripherals.hpp"
#include "DMAController.hpp"
#include "InterruptController.hpp"
#include "Macros.hpp"

#include <vector>
#include <fstream>
#include <iterator>
#include <cstring>

#include <fmt/core.h>

namespace PSX
{
    /**
     * @brief The core of the PSX. Its' main purpose is dispatching reads and writes.
     */
    std::shared_ptr<Bus> Bus::create()
    {
        auto bus = std::shared_ptr<Bus>(new Bus());
        bus->initialize_components();
        return bus;
    }

    /**
     * @brief allocate all components and connect them together 
     */
    void Bus::initialize_components()
    {
        LOG("initializing all hardware components");

        m_cpu                  = std::make_shared<CPU>(shared_from_this());
        m_gpu                  = std::make_shared<GPU>(shared_from_this());
        m_spu                  = std::make_shared<SPU>(shared_from_this());
        m_mdec                 = std::make_shared<MDEC>(shared_from_this());
        m_cdrom                = std::make_shared<CDROM>(shared_from_this());
        m_timer                = std::make_shared<Timer>(shared_from_this());
        m_ram_control          = std::make_shared<RamControl>();
        m_mem_control          = std::make_shared<MemControl>();
        m_peripherals          = std::make_shared<Peripherals>(shared_from_this());
        m_dma_controller       = std::make_shared<DMAController>(shared_from_this());
        m_interrupt_controller = std::make_shared<InterruptController>(shared_from_this());

        LOG("initialized all hardware components");
    }

    /**
     * @brief dispatch read to component or memory region according to memory map
     */
    template<typename T>
    T Bus::dispatch_read(u32 address)
    {
        // convert virtual address to physical
        u32 physical_address = virtual_to_physical<T>(address);

        switch(physical_address)
        {
            // access RAM
            case (RamBase) ... (RamBase + RamSize * 4 - 1):
            {
                return m_ram.read<T>((physical_address - RamBase) % RamSize);
            }
            // access BIOS
            case (BiosBase) ... (BiosBase + BiosSize - 1):
            {
                return m_bios.read<T>(physical_address - BiosBase);
            }
            default:
            {
                TODO();
            }
        }
    }

    /**
     * @brief dispatch write to component or memory region according to memory map
     */
    template<typename T>
    void Bus::dispatch_write(u32 address, T value)
    {
        // convert virtual address to physical
        u32 physical_address = virtual_to_physical<T>(address);

        switch(physical_address)
        {
            // access RAM
            case (RamBase) ... (RamBase + RamSize * 4 - 1):
            {
                m_ram.write<T>((physical_address - RamBase) % RamSize, value); return;
            }
            // access BIOS
            case (BiosBase) ... (BiosBase + BiosSize - 1):
            {
                m_bios.write<T>(physical_address - BiosBase, value); return;
            }
            // access MemControl
            case (MemControlBase) ... (MemControlBase + MemControlSize - 1):
            {
                component_write<T>(m_mem_control, physical_address - MemControlBase, value); return;
            }
            // access RamControl
            case (RamControlBase) ... (RamControlBase + RamControlSize - 1):
            {
                component_write<T>(m_ram_control, physical_address - RamControlBase, value); return;
            }
            default:
            {
                LOG_ERROR(fmt::format("unknown bus address: 0x{:08x}", physical_address));
                TODO();
            }
        }
    }

    /**
     * @brief execute all components for 1 clock cycle 
     */
    void Bus::execute(u32 num_steps)
    {
        m_cpu->execute(num_steps);
    }

    /**
     * @brief reads bios from file and loads it into the bios memory
     */
    void Bus::meta_load_bios(const std::string& bios_path)
    {
        LOG(fmt::format("loading bios from {}", bios_path));

        // open bios file
        std::ifstream bios_file(bios_path, std::ios::binary);

        if(!bios_file.is_open())
        {
            LOG_ERROR("bios file could not be opened");
            return;
        }

        // read file contents
        std::vector<u8> bios_file_contents((std::istreambuf_iterator<char>(bios_file)),
                                            std::istreambuf_iterator<char>());
        
        // do basic check TODO: checksum?
        if(bios_file_contents.size() != BiosSize)
        {
            LOG_ERROR(fmt::format("bios file content does not have the correct size (correct size: {} Bytes)", BiosSize));
            return;
        }
        
        // initialize bios
        std::memcpy(m_bios.data(), bios_file_contents.data(), BiosSize);

        LOG("bios loaded");
    }

    /**
     * instantiate templated arguments 
     */
    template u8 Bus::dispatch_read<u8>(u32 address);
    template u16 Bus::dispatch_read<u16>(u32 address);
    template u32 Bus::dispatch_read<u32>(u32 address);
    template void Bus::dispatch_write<u8>(u32 address, u8 value);
    template void Bus::dispatch_write<u16>(u32 address, u16 value);
    template void Bus::dispatch_write<u32>(u32 address, u32 value);
}