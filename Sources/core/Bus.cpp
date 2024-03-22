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
#include "Macros.hpp"
#include "IOPorts.hpp"
#include "SerialPort.hpp"
#include "Peripherals.hpp"
#include "RamController.hpp"
#include "MemController.hpp"
#include "DMAController.hpp"
#include "ExecutableFile.hpp"
#include "CacheController.hpp"
#include "PeripheralsInput.hpp"
#include "InterruptController.hpp"
#include "PeripheralsDigitalController.hpp"

#include <vector>
#include <fstream>
#include <cstring>
#include <iterator>

namespace PSX
{
    /**
     * @brief The core of the PSX. Its' main purpose is dispatching reads and writes.
     */
    std::shared_ptr<Bus> Bus::create(const std::shared_ptr<PeripheralsInput>& input)
    {
        auto bus = std::shared_ptr<Bus>(new Bus());
        bus->initialize_components(input);
        return bus;
    }

    /**
     * @brief allocate all components and connect them together 
     */
    void Bus::initialize_components(const std::shared_ptr<PeripheralsInput>& input)
    {
        LOG("initializing all hardware components");

        m_cpu                  = std::make_shared<CPU>(shared_from_this());
        m_spu                  = std::make_shared<SPU>(shared_from_this());
        m_mdec                 = std::make_shared<MDEC>(shared_from_this());
        m_io_ports             = std::make_shared<IOPorts>();
        m_serial_port          = std::make_shared<SerialPort>();
        m_ram_controller       = std::make_shared<RamController>();
        m_mem_controller       = std::make_shared<MemController>();
        m_cache_controller     = std::make_shared<CacheController>();
        m_interrupt_controller = std::make_shared<InterruptController>(m_cpu->exception_controller());
        m_peripherals          = std::make_shared<Peripherals>(shared_from_this(), input, m_interrupt_controller);
        m_cdrom                = std::make_shared<CDROM>(shared_from_this(), m_interrupt_controller);
        m_gpu                  = std::make_shared<GPU>(shared_from_this(), m_interrupt_controller);
        m_dma_controller       = std::make_shared<DMAController>(shared_from_this(), m_mdec, m_gpu, m_spu, m_cdrom, m_interrupt_controller);
        m_timer_dotclock       = std::make_shared<Timer<ClockSource::DotClock>>(m_interrupt_controller);   
        m_timer_hblank         = std::make_shared<Timer<ClockSource::HBlank>>(m_interrupt_controller);     
        m_timer_systemclock    = std::make_shared<Timer<ClockSource::SystemClock>>(m_interrupt_controller);
        
        LOG("initialized all hardware components");
    }

    /**
     * @brief reset the whole console 
     */
    void Bus::reset()
    {
        m_cpu->reset();
        m_spu->reset();
        m_mdec->reset();
        m_io_ports->reset();
        m_serial_port->reset();
        m_peripherals->reset();
        m_ram_controller->reset();
        m_mem_controller->reset();
        m_cache_controller->reset();
        m_interrupt_controller->reset();
        m_cdrom->reset();
        m_gpu->reset();
        m_dma_controller->reset();
        m_timer_dotclock->reset();
        m_timer_hblank->reset();
        m_timer_systemclock->reset();

        m_meta_vblank_flag = false;
    }

    /**
     * @brief dispatch read to component or memory region according to memory map
     */
    template<typename T>
    T Bus::dispatch_read(u32 address)
    {
        // convert virtual address to physical
        u32 physical_address = virtual_to_physical<T>(address);

        // access RAM
        if(in_range(physical_address, (RamBase), (RamBase + RamSize * 4 - 1)))
        {
            return m_ram.read<T>((physical_address - RamBase) % RamSize);
        }
        // access Expansion
        if(in_range(physical_address, (ExpansionBase), (ExpansionBase + ExpansionSize - 1)))
        {
            return m_expansion.read<T>(physical_address - ExpansionBase);
        }
        // access Scratchpad
        if(in_range(physical_address, (ScratchpadBase), (ScratchpadBase + ScratchpadSize - 1)))
        {
            return m_scratchpad.read<T>(physical_address - ScratchpadBase);
        }
        // access MemController
        if(in_range(physical_address, (MemControlBase), (MemControlBase + MemControlSize - 1)))
        {
            return component_read<T>(m_mem_controller, physical_address - MemControlBase);
        }
        // access Peripherals
        if(in_range(physical_address, (PeripheralsBase), (PeripheralsBase + PeripheralsSize - 1)))
        {
            return component_read<T>(m_peripherals, physical_address - PeripheralsBase);
        }
        // access SerialPort
        if(in_range(physical_address, (SerialBase), (SerialBase + SerialSize - 1)))
        {
            return component_read<T>(m_serial_port, physical_address - SerialBase);
        }
        // access RamController
        if(in_range(physical_address, (RamControlBase), (RamControlBase + RamControlSize - 1)))
        {
            return component_read<T>(m_ram_controller, physical_address - RamControlBase);
        }
        // access InterruptController
        if(in_range(physical_address, (InterruptBase), (InterruptBase + InterruptSize - 1)))
        {
            return component_read<T>(m_interrupt_controller, physical_address - InterruptBase);
        }
        // access DMA
        if(in_range(physical_address, (DmaBase), (DmaBase + DmaSize - 1)))
        {
            return component_read<T>(m_dma_controller, physical_address - DmaBase);
        }
        // access Timer0
        if(in_range(physical_address, (Timer0Base), (Timer0Base + Timer0Size - 1)))
        {
            return component_read<T>(m_timer_dotclock, physical_address - Timer0Base);
        }
        // access Timer1
        if(in_range(physical_address, (Timer1Base), (Timer1Base + Timer1Size - 1)))
        {
            return component_read<T>(m_timer_hblank, physical_address - Timer1Base);
        }
        // access Timer2
        if(in_range(physical_address, (Timer2Base), (Timer2Base + Timer2Size - 1)))
        {
            return component_read<T>(m_timer_systemclock, physical_address - Timer2Base);
        }
        // access CDROM
        if(in_range(physical_address, (CdromBase), (CdromBase + CdromSize - 1)))
        {
            return component_read<T>(m_cdrom, physical_address - CdromBase);
        }
        // access GPU
        if(in_range(physical_address, (GpuBase), (GpuBase + GpuSize - 1)))
        {
            return m_gpu->read(physical_address - GpuBase);
        }
        // access MDEC
        if(in_range(physical_address, (MdecBase), (MdecBase + MdecSize - 1)))
        {
            return m_mdec->read(physical_address - MdecBase);
        }
        // access SPU
        if(in_range(physical_address, (SpuBase), (SpuBase + SpuSize - 1)))
        {
            return component_read<T>(m_spu, physical_address - SpuBase);
        }
        // access IOPorts
        if(in_range(physical_address, (IOPortsBase), (IOPortsBase + IOPortsSize - 1)))
        {
            return component_read<T>(m_io_ports, physical_address - IOPortsBase);
        }
        // access BIOS
        if(in_range(physical_address, (BiosBase), (BiosBase + BiosSize - 1)))
        {
            return m_bios.read<T>(physical_address - BiosBase);
        }
        // access CacheController
        if(in_range(physical_address, (CacheControlBase), (CacheControlBase + CacheControlSize - 1)))
        {
            return component_read<T>(m_cache_controller, physical_address - CacheControlBase);
        }

        ABORT_WITH_MESSAGE(fmt::format("unknown bus address while dispatching read: 0x{:08x}", physical_address));
    }

    /**
     * @brief dispatch write to component or memory region according to memory map
     */
    template<typename T>
    void Bus::dispatch_write(u32 address, T value)
    {
        // if cpu has isolated cache, it means writes should go into the cpu cache,
        // but since we do not implement the cache, we let writes just fall through
        if(m_cpu->is_cache_isolated())
            return;
        
        // convert virtual address to physical
        u32 physical_address = virtual_to_physical<T>(address);

        // access RAM
        if(in_range(physical_address, (RamBase), (RamBase + RamSize * 4 - 1)))
        {
            m_ram.write<T>((physical_address - RamBase) % RamSize, value); return;
        }
        // access Expansion
        if(in_range(physical_address, (ExpansionBase), (ExpansionBase + ExpansionSize - 1)))
        {
            m_expansion.write<T>(physical_address - ExpansionBase, value); return;
        }
        // access Scratchpad
        if(in_range(physical_address, (ScratchpadBase), (ScratchpadBase + ScratchpadSize - 1)))
        {
            m_scratchpad.write<T>(physical_address - ScratchpadBase, value); return;
        }
        // access MemController
        if(in_range(physical_address, (MemControlBase), (MemControlBase + MemControlSize - 1)))
        {
            component_write<T>(m_mem_controller, physical_address - MemControlBase, value); return;
        }
        // access Peripherals
        if(in_range(physical_address, (PeripheralsBase), (PeripheralsBase + PeripheralsSize - 1)))
        {
            component_write<T>(m_peripherals, physical_address - PeripheralsBase, value); return;
        }
        // access SerialPort
        if(in_range(physical_address, (SerialBase), (SerialBase + SerialSize - 1)))
        {
            component_write<T>(m_serial_port, physical_address - SerialBase, value); return;
        }
        // access RamController
        if(in_range(physical_address, (RamControlBase), (RamControlBase + RamControlSize - 1)))
        {
            component_write<T>(m_ram_controller, physical_address - RamControlBase, value); return;
        }
        // access InterruptController
        if(in_range(physical_address, (InterruptBase), (InterruptBase + InterruptSize - 1)))
        {
            component_write<T>(m_interrupt_controller, physical_address - InterruptBase, value); return;
        }
        // access DMA
        if(in_range(physical_address, (DmaBase), (DmaBase + DmaSize - 1)))
        {
            component_write<T>(m_dma_controller, physical_address - DmaBase, value); return;
        }
        // access Timer0
        if(in_range(physical_address, (Timer0Base), (Timer0Base + Timer0Size - 1)))
        {
            component_write<T>(m_timer_dotclock, physical_address - Timer0Base, value); return;
        }
        // access Timer1
        if(in_range(physical_address, (Timer1Base), (Timer1Base + Timer1Size - 1)))
        {
            component_write<T>(m_timer_hblank, physical_address - Timer1Base, value); return;
        }
        // access Timer2
        if(in_range(physical_address, (Timer2Base), (Timer2Base + Timer2Size - 1)))
        {
            component_write<T>(m_timer_systemclock, physical_address - Timer2Base, value); return;
        }
        // access CDROM
        if(in_range(physical_address, (CdromBase), (CdromBase + CdromSize - 1)))
        {
            component_write<T>(m_cdrom, physical_address - CdromBase, value); return;
        }
        // access GPU
        if(in_range(physical_address, (GpuBase), (GpuBase + GpuSize - 1)))
        {
            m_gpu->write(physical_address - GpuBase, value); return;
        }
        // access MDEC
        if(in_range(physical_address, (MdecBase), (MdecBase + MdecSize - 1)))
        {
            m_mdec->write(physical_address - MdecBase, value); return;
        }
        // access SPU
        if(in_range(physical_address, (SpuBase), (SpuBase + SpuSize - 1)))
        {
            component_write<T>(m_spu, physical_address - SpuBase, value); return;
        }
        // access IO Ports
        if(in_range(physical_address, (IOPortsBase), (IOPortsBase + IOPortsSize - 1)))
        {
            component_write<T>(m_io_ports, physical_address - IOPortsBase, value); return;
        }
        // access BIOS
        if(in_range(physical_address, (BiosBase), (BiosBase + BiosSize - 1)))
        {
            m_bios.write<T>(physical_address - BiosBase, value); return;
        }
        // access CacheController
        if(in_range(physical_address, (CacheControlBase), (CacheControlBase + CacheControlSize - 1)))
        {
            component_write<T>(m_cache_controller, physical_address - CacheControlBase, value); return;
        }

        ABORT_WITH_MESSAGE(fmt::format("unknown bus address while dispatching write: 0x{:08x} 0x{:08x}", address, physical_address));
    }
    
    void Bus::meta_run_until_vblank()
    {
        while(!meta_vblank() && !m_cpu->meta_did_hit_breakpoint())
        {
            execute(PSX::Bus::OptimalSimulationStep);
        }
    }

    /**
     * @brief execute all components for num_steps clock cycles (relatively to CPU speed)
     */
    void Bus::execute(u32 num_steps)
    {
        m_cpu->execute(num_steps);
        if(m_cpu->meta_did_hit_breakpoint())
        {
            return;
        }
        m_dma_controller->execute(num_steps / OptimalSimulationStep);
        m_timer_dotclock->execute(num_steps);
        m_timer_hblank->execute(num_steps);
        m_timer_systemclock->execute(num_steps);
        m_cdrom->execute(num_steps / OptimalSimulationStep);
        m_gpu->execute(num_steps * (11.0/7.0) * 2);
        m_peripherals->execute(num_steps);
    }

    /**
     * @brief reads bios from file and loads it into the bios memory
     */
    void Bus::meta_load_bios(const std::string& bios_path)
    {
        LOG_DEBUG(1, fmt::format("loading bios from {}", bios_path));

        // open bios file
        std::ifstream bios_file(bios_path, std::ios::binary);

        if(!bios_file.is_open())
        {
            LOG_ERROR(fmt::format("bios file {} could not be opened", bios_path));
            return;
        }

        // read file contents
        std::vector<u8> bios_file_contents((std::istreambuf_iterator<char>(bios_file)),
                                            std::istreambuf_iterator<char>());
        
        // do basic size check
        if(bios_file_contents.size() != BiosSize)
        {
            LOG_ERROR(fmt::format("bios file content does not have the correct size (correct size: {} Bytes)", BiosSize));
            return;
        }
        
        // initialize bios
        std::memcpy(m_bios.data(), bios_file_contents.data(), BiosSize);

        bios_file.close();

        LOG_DEBUG(1, "bios loaded");
    }

    /**
     * @brief reads game and initializes/readies the cdrom
     */
    void Bus::meta_load_game(const std::string& game_path)
    {
        auto game_path_lower = game_path;
        std::transform(game_path_lower.begin(), game_path_lower.end(), game_path_lower.begin(), [](u8 ascii_char)
        {
            return std::tolower(ascii_char);
        });

        if(game_path_lower.ends_with(".bin"))
        {
            m_cdrom->meta_load_disc(game_path);
        }
        else if(game_path_lower.ends_with(".exe"))
        {
            static constexpr const u32 ConsoleInitializedAddress = 0x8003'0000;
            auto executable = ExecutableFile::create(game_path);
            meta_run_until(ConsoleInitializedAddress);
            m_cpu->meta_load_executable(executable);
            meta_copy_from_host_to_emulator(executable->text(), executable->text_base());
        }
        else
        {
            ABORT_WITH_MESSAGE(fmt::format("trying to load unsupported game format {}", game_path));
        }
    }

    /**
     * @brief set console region, which will be checked against the region of the disc
     */
    void Bus::meta_set_console_region(ConsoleRegion region)
    {
        m_cdrom->meta_set_console_region(region);
    }

    /**
     * @brief obtain the state of vram from gpu
     */
    const std::array<u16, VRamWidth * VRamHeight>& Bus::meta_get_vram_buffer() const
    {
        return m_gpu->meta_get_vram_buffer();
    }

    /**
     * @brief check whether GPU finished rendering a frame 
     */
    bool Bus::meta_vblank()
    {
        auto result = m_meta_vblank_flag;
        m_meta_vblank_flag = false;
        return result;
    }

    /**
     * @brief set vblank flag 
     */
    void Bus::meta_set_vblank()
    {
        m_meta_vblank_flag = true;
    }

    /**
     * @brief calculate current refresh rate 
     */
    float Bus::meta_refresh_rate() const
    {
        return m_gpu->meta_refresh_rate();
    }

    /**
     * @brief get the current rendering cutout of the vram
     */
    DisplayInfo Bus::meta_get_display_info() const
    {
        return m_gpu->meta_get_display_info();
    }

    /**
     * @brief run the emulator until CPU hits a particular address with program counter 
     */
    void Bus::meta_run_until(u32 program_counter)
    {
        m_cpu->meta_add_breakpoint(program_counter);
        while(!m_cpu->meta_did_hit_breakpoint())
        {
            meta_run_until_vblank();
        }
        m_cpu->meta_acknowledge_breakpoint();
    }

    /**
     * @brief append breakpoint to the list of breakpoints
     *        bus will halt execution of all components and hit breakpoint 
     *        can be checked using the `m_cpu->meta_did_hit_breakpoint`
     */
    void Bus::meta_add_breakpoint(u32 address)
    {
        m_cpu->meta_add_breakpoint(address);
    }

    /**
     * @brief copy portion of the PSX address space to host
     */
    std::vector<u8> Bus::meta_copy_from_emulator_to_host(u32 address, u32 size)
    {
        std::vector<u8> result(size);
        for(u32 i = 0; i < size; i++)
        {
            result[i] = dispatch_read<u8>(address + i);
        }
        return result;
    }

    /**
     * @brief copy host memory to PSX address space
     */
    void Bus::meta_copy_from_host_to_emulator(const std::vector<u8>& from, u32 to)
    {
        for(auto value: from)
        {
            dispatch_write<u8>(to++, value);
        }
    }

    /**
     * @brief accumulate the state of the emulator and save it to the file 
     */
    void Bus::serialize(std::shared_ptr<SaveState>& save_state)
    {
        m_cpu->serialize(save_state);
        m_gpu->serialize(save_state);
        m_spu->serialize(save_state);
        m_mdec->serialize(save_state);
        m_cdrom->serialize(save_state);
        m_io_ports->serialize(save_state);
        m_serial_port->serialize(save_state);
        m_peripherals->serialize(save_state);
        m_ram_controller->serialize(save_state);
        m_mem_controller->serialize(save_state);
        m_dma_controller->serialize(save_state);
        m_cache_controller->serialize(save_state);
        m_interrupt_controller->serialize(save_state);
        m_timer_dotclock->serialize(save_state);
        m_timer_hblank->serialize(save_state);
        m_timer_systemclock->serialize(save_state);
        save_state->serialize_from(m_ram);
        save_state->serialize_from(m_bios);
        save_state->serialize_from(m_scratchpad);
        save_state->serialize_from(m_expansion);
        save_state->serialize_from(m_meta_vblank_flag);
    }

    /**
     * @brief load state from a file and restore all components as they were
     */
    void Bus::deserialize(std::shared_ptr<SaveState>& save_state)
    {
        m_cpu->deserialize(save_state);
        m_gpu->deserialize(save_state);
        m_spu->deserialize(save_state);
        m_mdec->deserialize(save_state);
        m_cdrom->deserialize(save_state);
        m_io_ports->deserialize(save_state);
        m_serial_port->deserialize(save_state);
        m_peripherals->deserialize(save_state);
        m_ram_controller->deserialize(save_state);
        m_mem_controller->deserialize(save_state);
        m_dma_controller->deserialize(save_state);
        m_cache_controller->deserialize(save_state);
        m_interrupt_controller->deserialize(save_state);
        m_timer_dotclock->deserialize(save_state);
        m_timer_hblank->deserialize(save_state);
        m_timer_systemclock->deserialize(save_state);
        save_state->deserialize_to(m_ram);
        save_state->deserialize_to(m_bios);
        save_state->deserialize_to(m_scratchpad);
        save_state->deserialize_to(m_expansion);
        save_state->deserialize_to(m_meta_vblank_flag);
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