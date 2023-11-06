/**
 * @file      Bus.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     PSX Bus header
 *
 * @version   0.1
 *
 * @date      26. 10. 2023, 15:32 (created)
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

#ifndef BUS_HPP
#define BUS_HPP

#include <array>
#include <string>
#include <memory>
#include "Types.hpp"
#include "Macros.hpp"
#include "Constants.hpp"
#include "Component.hpp"
#include "MemoryRegion.hpp"
#include "Forward.hpp"

namespace PSX
{
    /**
     * @brief The core of the PSX. Its' main purpose is dispatching reads and writes.
     */
    class Bus : public std::enable_shared_from_this<Bus>
    {
    public:

        /**
         * @brief allocate bus and initialize all psx components
         */
        static std::shared_ptr<Bus> create();

        /**
         * @brief reads bios from file and loads it into the bios memory
         */
        void meta_load_bios(const std::string& bios_path);

        /**
         * @brief execute all components for num_steps clock cycles
         */
        void execute(u32 num_steps);

        /**
         * @brief dispatch read to component or memory region according to memory map
         */
        template<typename T>
        T dispatch_read(u32 address);

        /**
         * @brief dispatch write to component or memory region according to memory map
         */
        template<typename T>
        void dispatch_write(u32 address, T value);

    private:

        /**
         * since we are expecting the Bus class to be managed through std::shared_ptr
         * creating or copying the Bus class makes no sense so we just delete that
         * functionality
         */
        explicit Bus() {}
        DELETE_COPY_CONSTRUCTOR(Bus);
        DELETE_MOVE_CONSTRUCTOR(Bus);
        DELETE_COPY_ASSIGNMENT(Bus);
        DELETE_MOVE_ASSIGNMENT(Bus);

        /**
         * @brief allocate all components and connect them together 
         */
        void initialize_components();

        /**
         * @brief convert virtual address to physical 
         */
        template<typename T>
        u32 virtual_to_physical(u32 virtual_address)
        {
            if constexpr (sizeof(T) == sizeof(u8))  return virtual_address & 0x1FFFFFFF;
            if constexpr (sizeof(T) == sizeof(u16)) return virtual_address & 0x1FFFFFFE;
            if constexpr (sizeof(T) == sizeof(u32)) return virtual_address & 0x1FFFFFFC;

            UNREACHABLE();
        }

        /**
         * @brief dispatch read to component
         */
        template<typename T>
        T component_read(const std::shared_ptr<Component>& component, u32 address)
        {
            if constexpr (sizeof(T) == sizeof(u8))
            {
                return component->read(address);
            }

            if constexpr (sizeof(T) == sizeof(u16))
            {
                return (component->read(address + 0) << 0) |
                       (component->read(address + 1) << 8);
            }

            if constexpr (sizeof(T) == sizeof(u32))
            {
                return (component->read(address + 0) <<  0) |
                       (component->read(address + 1) <<  8) |
                       (component->read(address + 2) << 16) |
                       (component->read(address + 3) << 24);
            }

            UNREACHABLE();

            return 0;
        }

        /**
         * @brief dispatch write to component
         */
        template<typename T>
        void component_write(const std::shared_ptr<Component>& component, u32 address, T value)
        {
            if constexpr (sizeof(T) == sizeof(u8))
            {
                component->write(address, value);
                return;
            }

            if constexpr (sizeof(T) == sizeof(u16))
            {
                component->write(address + 0, static_cast<u8>((value >> 0) & 0xFF));
                component->write(address + 1, static_cast<u8>((value >> 8) & 0xFF));
                return;
            }

            if constexpr (sizeof(T) == sizeof(u32))
            {
                component->write(address + 0, static_cast<u8>((value >>  0) & 0xFF));
                component->write(address + 1, static_cast<u8>((value >>  8) & 0xFF));
                component->write(address + 2, static_cast<u8>((value >> 16) & 0xFF));
                component->write(address + 3, static_cast<u8>((value >> 23) & 0xFF));
                return;
            }

            UNREACHABLE();
        }

        static constexpr const u32 RamBase          = 0x00000000;
        static constexpr const u32 ExpansionBase    = 0x1F000000;
        static constexpr const u32 ScratchpadBase   = 0x1F800000;
        static constexpr const u32 MemControlBase   = 0x1F801000;
        static constexpr const u32 PeripheralsBase  = 0x1f801040;
        static constexpr const u32 SerialBase       = 0x1F801050;
        static constexpr const u32 RamControlBase   = 0x1F801060;
        static constexpr const u32 InterruptBase    = 0x1F801070;
        static constexpr const u32 DmaBase          = 0x1F801080;
        static constexpr const u32 Timer0Base       = 0x1F801100;
        static constexpr const u32 Timer1Base       = 0x1F801110;
        static constexpr const u32 Timer2Base       = 0x1F801120;
        static constexpr const u32 CdromBase        = 0x1F801800;
        static constexpr const u32 GpuBase          = 0x1F801810;
        static constexpr const u32 MdecBase         = 0x1F801820;
        static constexpr const u32 SpuBase          = 0x1F801C00;
        static constexpr const u32 IOPortsBase      = 0x1F802000;
        static constexpr const u32 BiosBase         = 0x1FC00000;
        static constexpr const u32 CacheControlBase = 0x1FFE0130;
        
        static constexpr const u32 RamSize          = 2 * MiB;
        static constexpr const u32 ExpansionSize    = 1 * MiB;
        static constexpr const u32 ScratchpadSize   = 1 * KiB;
        static constexpr const u32 MemControlSize   = 0x24;
        static constexpr const u32 PeripheralsSize  = 0x10;
        static constexpr const u32 SerialSize       = 0x10;
        static constexpr const u32 RamControlSize   = 0x4;
        static constexpr const u32 InterruptSize    = 0x8;
        static constexpr const u32 DmaSize          = 0x80;
        static constexpr const u32 Timer0Size       = 0x10;
        static constexpr const u32 Timer1Size       = 0x10;
        static constexpr const u32 Timer2Size       = 0x10;
        static constexpr const u32 CdromSize        = 0x4;
        static constexpr const u32 GpuSize          = 0x8;
        static constexpr const u32 MdecSize         = 0x8;
        static constexpr const u32 SpuSize          = 0x400;
        static constexpr const u32 IOPortsSize      = 0x2000;
        static constexpr const u32 BiosSize         = 512 * KiB;
        static constexpr const u32 CacheControlSize = 0x4;

        std::shared_ptr<CPU>                 m_cpu;                  /// CPU Component
        std::shared_ptr<GPU>                 m_gpu;                  /// GPU Component
        std::shared_ptr<SPU>                 m_spu;                  /// SPU Component
        std::shared_ptr<MDEC>                m_mdec;                 /// MDEC Component
        std::shared_ptr<CDROM>               m_cdrom;                /// CDROM Component
        std::shared_ptr<Timer>               m_timer;                /// Timer Component
        std::shared_ptr<IOPorts>             m_io_ports;             /// IOPorts Component
        std::shared_ptr<Peripherals>         m_peripherals;          /// Peripherals Component
        std::shared_ptr<RamController>       m_ram_controller;       /// RamController Component
        std::shared_ptr<MemController>       m_mem_controller;       /// MemController Component
        std::shared_ptr<DMAController>       m_dma_controller;       /// DMA Controller
        std::shared_ptr<CacheController>     m_cache_controller;     /// Cache Controller
        std::shared_ptr<InterruptController> m_interrupt_controller; /// InterruptController

        MemoryRegion<RamSize>        m_ram;        /// RAM memory
        MemoryRegion<BiosSize>       m_bios;       /// BIOS memory
        MemoryRegion<ScratchpadSize> m_scratchpad; /// Scratchpad memory
        MemoryRegion<ExpansionSize>  m_expansion;  /// Expansion memory
    };
}

#endif // BUS_HPP