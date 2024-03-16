/**
 * @file      SPU.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX Sound processing unit
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

#ifndef SPU_HPP
#define SPU_HPP

#include <memory>
#include "Component.hpp"
#include "Forward.hpp"
#include "Constants.hpp"
#include "MemoryRegion.hpp"
#include "SPUConstants.hpp"

namespace PSX
{
    /**
     * @brief PSX Sound processing unit
     */
    class SPU final : public Component
    {
    public:

        SPU(const std::shared_ptr<Bus>& bus) :
            m_bus(bus)
        {
            reset();
        }
        
        virtual ~SPU() override = default;

        virtual void execute(u32 num_steps) override;
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;
        virtual void serialize(std::shared_ptr<SaveState>&) override;
        virtual void deserialize(std::shared_ptr<SaveState>&) override;

    private:

        std::shared_ptr<Bus> m_bus;

        /**
         * @brief 0x1F801DAA - SPU Control Register (SPUCNT)
         */
        union Control
        {
            struct
            {
                u16 cd_audio_enable:              1;
                u16 external_audio_enable:        1;
                u16 cd_audio_reverb_enable:       1;
                u16 external_audio_reverb_enable: 1;
                u16 sound_ram_transfer_mode:      2; // 0 - stop, 1 - manual write, 2 - dma write, 3 - dma read
                u16 irq9_enable:                  1;
                u16 reverb_master_enable:         1;
                u16 noise_frequency_step:         2; // 0 - step 4, 1 - step 5, 2 - step 6, 3 - step 7
                u16 noise_frequency_shift:        4; // 0 - low frequency ... 15 - high frequency
                u16 mute_spu:                     1;
                u16 spu_enable:                   1;
            };

            u16 raw;
            u8  bytes[sizeof(u16)];
        };

        /**
         * @brief 0x1F801DAE - SPU Status Register (SPUSTAT) (R)
         */
        union Status
        {
            struct
            {
                u16 cd_audio_enable:              1;
                u16 external_audio_enable:        1;
                u16 cd_audio_reverb_enable:       1;
                u16 external_audio_reverb_enable: 1;
                u16 sound_ram_transfer_mode:      2; // 0 - stop, 1 - manual write, 2 - dma write, 3 - dma read
                u16 irq9_flag:                    1;
                u16 data_transfer_dma_rw_request: 1;
                u16 data_transfer_dma_w_request:  1;
                u16 data_transfer_dma_r_request:  1;
                u16 data_transfer_busy_flag:      1;
                u16 writing_to_first_or_second_half_of_capture_buffer: 1; // 0 - first, 1 - second

                u16: 4;
            };

            u16 raw;
            u8  bytes[sizeof(u16)];
        };

        static_assert(sizeof(Control) == sizeof(u16));

        /**
         * @brief 0x1F801DAC - Sound RAM Data Transfer Control (should be 0x0004)
         */
        union DataTransferControl
        {
            struct
            {
                u16: 1;

                u16 sound_ram_data_transfer_type: 3; // should be 2

                u16: 12;
            };

            u16 raw;
            u8  bytes[sizeof(u16)];
        };

        static_assert(sizeof(DataTransferControl) == sizeof(u16));

        /**
         * SPU registers 
         */
        Register<u16> m_main_volume_left;
        Register<u16> m_main_volume_right;
        Register<u16> m_reverb_volume_left;
        Register<u16> m_reverb_volume_right;
        Register<u16> m_cd_volume_left;
        Register<u16> m_cd_volume_right;
        Register<u16> m_external_volume_left;
        Register<u16> m_external_volume_right;
        Register<u16> m_sram_data_transfer_address;
        Register<u16> m_reverb_work_area_start_address;
        u16           m_sram_data_transfer_current_address;
        Control       m_control;
        Status        m_status;
        DataTransferControl m_data_transfer_control;

        /**
         * SPU memory regions
         */
        MemoryRegion<SRAMSize> m_sram;
    };
}

#endif // SPU_HPP