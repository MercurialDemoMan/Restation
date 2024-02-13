/**
 * @file      CDROM.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX CDROM
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

#ifndef CDROM_HPP
#define CDROM_HPP

#include <memory>
#include "Component.hpp"
#include "Forward.hpp"
#include "Disc.hpp"
#include "CDROMConstants.hpp"
#include "CDROMInstruction.hpp"
#include "Utils.hpp"

namespace PSX
{
    /**
     * @brief PSX CDROM
     */
    class CDROM final : public Component
    {
    public:

        CDROM(const std::shared_ptr<Bus>& bus, const std::shared_ptr<InterruptController>& interrupt_controller) :
            m_bus(bus),
            m_interrupt_controller(interrupt_controller)
        {
            reset();
        }
        
        virtual ~CDROM() override = default;

        virtual void execute(u32 num_steps) override;
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;

        /**
         * @brief load disc from the host filesystem 
         */
        void meta_load_disc(const std::string& meta_file_path);

    private:

        /**
         * base commands
         */
        void UNK();        /// Unknown Command
        void GETSTAT();    /// Get Status Register
        void SETLOC();     /// Set Seek Target 
        void PLAY();       /// Start Audio Playback
        void FORWARD();    /// Skip Sectors Forwards
        void BACKWARD();   /// Skip Sectors Backwards
        void READN();      /// Read With Retry
        void MOTORON();    /// Activate Drive Motor
        void STOP();       /// Stop Motor With Magnetic Brakes
        void PAUSE();      /// Abort Reading And Playing
        void INIT();       /// Initialize CD-ROM
        void MUTE();       /// Turn Off Audio Streaming To SPU
        void DEMUTE();     /// Turn On Audio Streaming To SPU
        void SETFILTER();  /// ADPCM Filter
        void SETMODE();    /// Set Mode Register
        void GETPARAM();   /// Get Status, Mode, Filter and Null
        void GETLOCL();    /// Get Sector Header And Subheader
        void GETLOCP();    /// Get Position From Subchannel Q
        void SETSESSION(); /// Seek To Session
        void GETTN();      /// Get First Track Number
        void GETTD();      /// Get Track Address
        void SEEKL();      /// Seek To SETLOC In Data Mode
        void SEEKP();      /// Seek To SETLOC In Audio Mode
        void TEST();       /// Test Of Hardware
        void GETID();      /// Get Disk Identificator
        void READS();      /// Read Without Retry
        void RESET();      /// Reset CD-ROM
        void GETQ();       /// Get Position From Subchannel Q (mm:ss:ff)
        void READTOC();    /// Reread Table Of Contents Of Current Session
        void VIDEOCD();    /// Firmware Specific Command
        void SECRET1();    /// Backdoor For Checking Region Code
        void SECRET2();    /// Backdoor For Checking Region Code
        void SECRET3();    /// Backdoor For Checking Region Code
        void SECRET4();    /// Backdoor For Checking Region Code
        void SECRET5();    /// Backdoor For Checking Region Code
        void SECRET6();    /// Backdoor For Checking Region Code
        void SECRET7();    /// Backdoor For Checking Region Code
        void SECRETLOCK(); /// Backdoor For Checking Region Code

        /**
         * command handler map
         */
        typedef void(CDROM::*CommandHandler)();
        CommandHandler m_handlers[128] =
        {
            &CDROM::UNK,     &CDROM::GETSTAT, &CDROM::SETLOC,     &CDROM::PLAY,    &CDROM::FORWARD, &CDROM::BACKWARD,  &CDROM::READN,   &CDROM::MOTORON,
            &CDROM::STOP,    &CDROM::PAUSE,   &CDROM::INIT,       &CDROM::MUTE,    &CDROM::DEMUTE,  &CDROM::SETFILTER, &CDROM::SETMODE, &CDROM::GETPARAM,
            &CDROM::GETLOCL, &CDROM::GETLOCP, &CDROM::SETSESSION, &CDROM::GETTN,   &CDROM::GETTD,   &CDROM::SEEKL,     &CDROM::SEEKP,   &CDROM::UNK,
            &CDROM::UNK,     &CDROM::TEST,    &CDROM::GETID,      &CDROM::READS,   &CDROM::RESET,   &CDROM::GETQ,      &CDROM::READTOC, &CDROM::VIDEOCD, 
            &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,        &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,       &CDROM::UNK,     &CDROM::UNK,     
            &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,        &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,       &CDROM::UNK,     &CDROM::UNK,     
            &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,        &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,       &CDROM::UNK,     &CDROM::UNK,     
            &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,        &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,       &CDROM::UNK,     &CDROM::UNK,     
            &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,        &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,       &CDROM::UNK,     &CDROM::UNK,     
            &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,        &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,       &CDROM::UNK,     &CDROM::UNK,  
            &CDROM::SECRET1, &CDROM::SECRET2, &CDROM::SECRET3,    &CDROM::SECRET4, &CDROM::SECRET5, &CDROM::SECRET6,   &CDROM::SECRET7, &CDROM::SECRETLOCK,
            &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,        &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,       &CDROM::UNK,     &CDROM::UNK,     
            &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,        &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,       &CDROM::UNK,     &CDROM::UNK,     
            &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,        &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,       &CDROM::UNK,     &CDROM::UNK,     
            &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,        &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,       &CDROM::UNK,     &CDROM::UNK,     
            &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,        &CDROM::UNK,     &CDROM::UNK,     &CDROM::UNK,       &CDROM::UNK,     &CDROM::UNK
        };

        /**
         * @brief 0x1F801800 - Index/Status Register (R only)
         */
        union Index
        {
            struct
            {
                u8 index:                2;
                u8 xa_adpcm_fifo_empty:  1;
                u8 parameter_fifo_empty: 1;
                u8 parameter_fifo_full:  1;
                u8 response_fifo_empty:  1;
                u8 data_fifo_empty:      1;
                u8 transmission_busy:    1;
            };

            u8 raw;
        };

        /**
         * @brief CDROM Status Register
         */
        union Status
        {
            enum class Mode
            {
                None = 0,
                Read = 1,
                Seek = 2,
                Play = 3
            };

            struct
            {
                u8 error:         1;
                u8 spindle_motor: 1;
                u8 seek_error:    1;
                u8 id_error:      1;
                u8 shell_open:    1;
                u8 read:          1;
                u8 seek:          1;
                u8 play:          1;
            };

            u8 raw;
        };

        /**
         * @brief 0x0E SETMODE command mode 
         */
        union Mode
        {
            struct
            {
                u8 cdda:        1;
                u8 auto_pause:  1;
                u8 report:      1;
                u8 xa_filter:   1;
                u8 ignore_bit:  1;
                u8 sector_size: 1;
                u8 xa_adpcm:    1;
                u8 speed:       1;
            };

            u8 raw;
        };

        /**
         * @brief extract response byte from response fifo 
         */
        u8 read_response();

        /**
         * @brief extract data byte from data fifo 
         */
        u8 read_data();

        /**
         * @brief extract interrupt byte from interrupt fifo 
         */
        u8 read_interrupt();

        /**
         * @brief write into the parameter fifo
         */
        void write_parameter(u32);

        /**
         * @brief write into the request register
         */
        void write_request(u32);

        /**
         * @brief push byte into response fifo
         */
        void push_to_response_fifo(u8);

        /**
         * @brief push byte into interrupt fifo
         */
        void push_to_interrupt_fifo(u8);

        /**
         * @brief pop byte from parameter fifo
         */
        u8 pop_from_parameter_fifo();

        /**
         * @brief update status register with new mode
         */
        void set_status_mode(Status::Mode);

        /**
         * @brief execute instruction 
         */
        void execute(const CDROMInstruction&);

        std::shared_ptr<Bus> m_bus;
        std::shared_ptr<InterruptController> m_interrupt_controller;
        std::shared_ptr<Disc> m_disc;

        u32    m_cycles;             /// total read bytes from a sector
        Index  m_index;              /// Index/Status register
        Status m_status;             /// Status register
        Mode   m_mode;               /// Mode register
        Sector m_current_sector;     /// currently loaded disc sector
        u32    m_sector_head;        /// reading head
        u32    m_sector_seek_target; /// disc seeking offset
        fixed_queue<u8, ParameterFIFOSize> m_parameter_fifo; /// queue for command arguments
        fixed_queue<u8, ResponseFIFOSize>  m_response_fifo;  /// queue for command results
        fixed_queue<u8, InterruptFIFOSize> m_interrupt_fifo; /// queue for signaling finished command
        u8 m_interrupt_enable; /// can we send interrupts?
        u8 m_mute;             /// turn on/off audio streaming
    };
}

#endif // CDROM_HPP