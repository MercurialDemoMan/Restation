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

#include <memory>
#include "Types.hpp"
#include "Macros.hpp"
#include "Constants.hpp"
#include "Forward.hpp"

namespace PSX
{
    /**
     * @brief The core of the PSX. Its' main purpose is dispatching reads and writes.
     */
    class Bus : public std::enable_shared_from_this<Bus>
    {
    public:

        static std::shared_ptr<Bus> create();

    private:

        explicit Bus() {}
        DELETE_COPY_CONSTRUCTOR(Bus);
        DELETE_MOVE_CONSTRUCTOR(Bus);

        void initialize_components();

        std::shared_ptr<CPU>                 m_cpu;
        std::shared_ptr<GPU>                 m_gpu;
        std::shared_ptr<SPU>                 m_spu;
        std::shared_ptr<MDEC>                m_mdec;
        std::shared_ptr<CDROM>               m_cdrom;
        std::shared_ptr<Timer>               m_timer;
        std::shared_ptr<Peripherals>         m_peripherals;
        std::shared_ptr<DMAController>       m_dma_controller;
        std::shared_ptr<InterruptController> m_interrupt_controller;

        static constexpr const u32 RamBase          = 0x00000000;
        static constexpr const u32 ExpansionBase    = 0x1F000000;
        static constexpr const u32 ScratchpadBase   = 0x1F800000;
        static constexpr const u32 IoBase           = 0x1F801000;
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
        static constexpr const u32 Expansion2Base   = 0x1F802000;
        static constexpr const u32 BiosBase         = 0x1FC00000;
        static constexpr const u32 CacheControlBase = 0x1FFE0130;
        
        static constexpr const u32 RamSize          = 2   * MiB;
        static constexpr const u32 ExpansionSize    = 1   * MiB;
        static constexpr const u32 ScratchpadSize   = 1   * KiB;
        static constexpr const u32 IoSize           = 8   * KiB;
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
        static constexpr const u32 Expansion2Size   = 0x2000;
        static constexpr const u32 BiosSize         = 512 * KiB;
        static constexpr const u32 CacheControlSize = 0x4;
    };
}

#endif // BUS_HPP