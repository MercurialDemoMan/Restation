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

namespace PSX
{
    /**
     * @brief PSX CDROM
     */
    class CDROM final : public Component
    {
    public:

        CDROM(const std::shared_ptr<Bus>& bus) :
            m_bus(bus)
        {
            
        }
        
        virtual ~CDROM() override = default;

        virtual void execute(u32 num_steps) override;
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;

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

        std::shared_ptr<Bus> m_bus;

    };
}

#endif // CDROM_HPP