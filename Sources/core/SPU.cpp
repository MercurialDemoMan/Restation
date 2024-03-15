/**
 * @file      SPU.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation of the PSX Sound processing unit
 *
 * @version   0.1
 *
 * @date      26. 10. 2023, 16:20 (created)
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

#include "SPU.hpp"
#include "Bus.hpp"
#include "Macros.hpp"

namespace PSX
{
    void SPU::execute(u32 num_steps)
    {
        MARK_UNUSED(num_steps);
        TODO();
    }

    u32 SPU::read(u32 address)
    {
        MARK_UNUSED(address);
        //return 0;

        if(in_range<u32>(address, 0, 16 * 24 - 1))
        {
            return 0; //TODO(); // read from voice
        }
        if(in_range<u32>(address, 0x0180, 0x0181))
        {
            TODO(); // main volume left
        }
        if(in_range<u32>(address, 0x0182, 0x0183))
        {
            TODO(); // main volume right
        }
        if(in_range<u32>(address, 0x0184, 0x0185))
        {
            TODO(); // reverb volume left
        }
        if(in_range<u32>(address, 0x0186, 0x0187))
        {
            TODO(); // reverb volume right
        }
        if(in_range<u32>(address, 0x0188, 0x018B))
        {
            return 0; //TODO(); // voice key on
        }
        if(in_range<u32>(address, 0x018C, 0x018F))
        {
            return 0; //TODO(); // voice key off
        }
        if(in_range<u32>(address, 0x0190, 0x0193))
        {
            return 0; //TODO(); // voice pitch modulation enabled
        }
        if(in_range<u32>(address, 0x0194, 0x0197))
        {
            return 0; //TODO(); // voice noise mode enabled
        }
        if(in_range<u32>(address, 0x0198, 0x019B))
        {
            return 0; //TODO(); // voice reverb enabled
        }
        if(in_range<u32>(address, 0x019C, 0x019F))
        {
            TODO(); // voice on/off
        }
        if(in_range<u32>(address, 0x01A2, 0x01A3))
        {
            return m_reverb_work_area_start_address.read(address - 0x01A2);
        }
        if(in_range<u32>(address, 0x01A4, 0x01A5))
        {
            TODO(); // SRAM irq address
        }
        if(in_range<u32>(address, 0x01A6, 0x01A7))
        {
            return m_sram_data_transfer_address.read(address - 0x01A6); 
        }
        if(in_range<u32>(address, 0x01A8, 0x01A9))
        {
            TODO(); // SRAM data transfer fifo
        }
        if(in_range<u32>(address, 0x01AA, 0x01AB))
        {
            return m_control.bytes[address - 0x01AA];
        }
        if(in_range<u32>(address, 0x01AC, 0x01AD))
        {
            return m_data_transfer_control.bytes[address - 0x01AC];
        }
        if(in_range<u32>(address, 0x01AE, 0x01AF))
        {
            return m_status.bytes[address - 0x01AE];
        }
        if(in_range<u32>(address, 0x01B0, 0x01B1))
        {
            TODO(); // cd volume left
        }
        if(in_range<u32>(address, 0x01B2, 0x01B3))
        {
            TODO(); // cd volume right
        }
        if(in_range<u32>(address, 0x01B4, 0x01B5))
        {
            TODO(); // external audio input volume left
        }
        if(in_range<u32>(address, 0x01B6, 0x01B7))
        {
            TODO(); // external audio input volume left
        }
        if(in_range<u32>(address, 0x01B8, 0x01B9))
        {
            return m_main_volume_left.read(address - 0x01B8);
        }
        if(in_range<u32>(address, 0x01BA, 0x01BB))
        {
            return m_main_volume_right.read(address - 0x01BA);
        }
        if(in_range<u32>(address, 0x0200, 0x0200 + 4 * 24 - 1))
        {
            TODO(); // voice current volume left/right
        }

        UNREACHABLE();
        return 0;
    }

    void SPU::write(u32 address, u32 value)
    {
        if(in_range<u32>(address, 0, 16 * 24 - 1))
        {
            return; //TODO(); // read from voice
        }
        if(in_range<u32>(address, 0x0180, 0x0181))
        {
            m_main_volume_left.write(address - 0x0180, value); return;
        }
        if(in_range<u32>(address, 0x0182, 0x0183))
        {
            m_main_volume_right.write(address - 0x0182, value); return;
        }
        if(in_range<u32>(address, 0x0184, 0x0185))
        {
            m_reverb_volume_left.write(address - 0x0184, value); return;
        }
        if(in_range<u32>(address, 0x0186, 0x0187))
        {
            m_reverb_volume_right.write(address - 0x0186, value); return;
        }
        if(in_range<u32>(address, 0x0188, 0x018B))
        {
            return; //TODO(); // voice key on
        }
        if(in_range<u32>(address, 0x018C, 0x018F))
        {
            return; //TODO(); // voice key off
        }
        if(in_range<u32>(address, 0x0190, 0x0193))
        {
            return; // TODO(); // voice pitch modulation enabled
        }
        if(in_range<u32>(address, 0x0194, 0x0197))
        {
            return; // TODO(); // voice noise mode enabled
        }
        if(in_range<u32>(address, 0x0198, 0x019B))
        {
            return; // TODO(); // voice reverb enabled
        }
        if(in_range<u32>(address, 0x019C, 0x019F))
        {
            TODO(); // voice on/off
        }
        if(in_range<u32>(address, 0x01A2, 0x01A3))
        {
            m_reverb_work_area_start_address.write(address - 0x01A2, value); return;
        }
        if(in_range<u32>(address, 0x01A4, 0x01A5))
        {
            TODO(); // SRAM irq address
        }
        if(in_range<u32>(address, 0x01A6, 0x01A7))
        {
            m_sram_data_transfer_address.write(address - 0x01A6, value); 
            m_sram_data_transfer_current_address = m_sram_data_transfer_address.raw() * 8;
            return;
        }
        if(in_range<u32>(address, 0x01A8, 0x01A9)) // SRAM data transfer fifo
        {
            // NOTE: ignore the fifo, and just write into the SRAM directly?
            m_sram.write<u8>(m_sram_data_transfer_current_address, value);
            m_sram_data_transfer_current_address = (m_sram_data_transfer_current_address + 1) % SRAMSize;
            return;
        }
        if(in_range<u32>(address, 0x01AA, 0x01AB))
        {
            m_control.bytes[address - 0x01AA]     = value;
            m_status.cd_audio_enable              = m_control.cd_audio_enable;
            m_status.external_audio_enable        = m_control.external_audio_enable;
            m_status.cd_audio_reverb_enable       = m_control.cd_audio_reverb_enable;
            m_status.external_audio_reverb_enable = m_control.external_audio_reverb_enable;
            m_status.sound_ram_transfer_mode      = m_control.sound_ram_transfer_mode;
            m_status.irq9_flag                    = m_control.irq9_enable;
            return;
        }
        if(in_range<u32>(address, 0x01AC, 0x01AD))
        {
            m_data_transfer_control.bytes[address - 0x01AC] = value; return;
        }
        if(in_range<u32>(address, 0x01AE, 0x01AF))
        {
            TODO(); // write status register?
        }
        if(in_range<u32>(address, 0x01B0, 0x01B1))
        {
            m_cd_volume_left.write(address - 0x01B0, value); return;
        }
        if(in_range<u32>(address, 0x01B2, 0x01B3))
        {
            m_cd_volume_right.write(address - 0x01B2, value); return;
        }
        if(in_range<u32>(address, 0x01B4, 0x01B5))
        {
            m_external_volume_left.write(address - 0x01B4, value); return;
        }
        if(in_range<u32>(address, 0x01B6, 0x01B7))
        {
            m_external_volume_right.write(address - 0x01B6, value); return;
        }
        if(in_range<u32>(address, 0x01B8, 0x01B9))
        {
            TODO(); // current main volume left?
        }
        if(in_range<u32>(address, 0x01BA, 0x01BB))
        {
            TODO(); // current main volume right?
        }
        if(in_range<u32>(address, 0x01C0, 0x01FF))
        {
            return; //TODO(); // reverb configuration area
        }
        if(in_range<u32>(address, 0x0200, 0x0200 + 4 * 24 - 1))
        {
            TODO(); // voice current volume left/right?
        }

        UNREACHABLE();
    }

    void SPU::reset()
    {
        m_main_volume_left.raw()      = 0;
        m_main_volume_right.raw()     = 0;
        m_reverb_volume_left.raw()    = 0;
        m_reverb_volume_right.raw()   = 0;
        m_cd_volume_left.raw()        = 0;
        m_cd_volume_right.raw()       = 0;
        m_external_volume_left.raw()  = 0;
        m_external_volume_right.raw() = 0;
        m_sram_data_transfer_address.raw()     = 0;
        m_reverb_work_area_start_address.raw() = 0;
        m_sram_data_transfer_current_address   = 0;

        m_control.raw = 0;
        m_status.raw  = 0;
        m_data_transfer_control.raw   = 0x0004;
    }
}