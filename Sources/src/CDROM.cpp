/**
 * @file      CDROM.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation of the PSX CDROM
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

#include "CDROM.hpp"
#include "Bus.hpp"

namespace PSX
{
    void CDROM::execute(u32 num_steps)
    {
        MARK_UNUSED(num_steps);
        TODO();
    }

    u32 CDROM::read(u32 address)
    {
        switch(address)
        {
            case 0:
            {
                return m_index.raw;
            }
            case 1:
            {
                return read_response();
            }
            case 2:
            {
                return read_data();
            }
            case 3:
            {
                return read_interrupt();
            }
        }

        UNREACHABLE();
    }

    void CDROM::write(u32 address, u32 value)
    {
        MARK_UNUSED(address);
        MARK_UNUSED(value);
        TODO();
    }

    void CDROM::reset()
    {
        TODO();
    }

    /**
     * @brief load disc from the host filesystem 
     */
    void CDROM::meta_load_disc(const std::string& meta_file_path)
    {
        m_disc = Disc::create(meta_file_path);
        TODO();
    }

    /**
     * @brief extract response byte from response fifo 
     */
    u8 CDROM::read_response()
    {
        auto response_or_error = m_response_fifo.pop();

        if(response_or_error)
        {
            auto response = response_or_error.value();

            if(m_response_fifo.empty())
                m_index.response_fifo_empty = 0;

            return response;
        }
        else
        {
            return 0;
        }
    }

    /**
     * @brief extract data byte from data fifo 
     */
    u8 CDROM::read_data()
    {
        TODO();
    }

    /**
     * @brief extract interrupt byte from interrupt fifo 
     */
    u8 CDROM::read_interrupt()
    {
        if(m_index.index == 0 || m_index.index == 2)
            return m_interrupt_enable;
        
        if(m_index.index == 1 || m_index.index == 3)
        {
            //note: top 3 bits' function are unknown but they are always set to 1
            if(!m_interrupt_fifo.empty())
            {
                return 0b1110'0000 | (m_interrupt_fifo.top().value() & 0b0111);
            }

            return 0b1110'0000;
        }
    }

    /**
     * @brief push byte into response fifo
     */
    void CDROM::push_to_response_fifo(u8 value)
    {
        if(!m_response_fifo.push(value))
            LOG_DEBUG(6, fmt::format("trying to push response {} into a full response queue", value));

        m_index.response_fifo_empty = 1;
    }

    /**
     * @brief push byte into interrupt fifo
     */
    void CDROM::push_to_interrupt_fifo(u8 value)
    {
        if(value > 0b111)
            ABORT_WITH_MESSAGE(fmt::format("trying to push invalid interrupt {}", value));
        
        if(!m_interrupt_fifo.push(value))
            LOG_DEBUG(6, fmt::format("trying to push interrupt {} into a full interrupt queue", value));
    }

    /**
     * @brief pop byte from parameter fifo
     */
    u8 CDROM::pop_from_parameter_fifo()
    {
        auto parameter_or_error = m_parameter_fifo.pop();

        if(!parameter_or_error)
            ABORT_WITH_MESSAGE(fmt::format("trying to pop parameter from empty parameter queue"));

        m_index.parameter_fifo_empty = m_parameter_fifo.empty();
        m_index.parameter_fifo_full  = 1;

        return parameter_or_error.value();
    }

    /**
     * @brief update status register with new mode
     */
    void CDROM::set_status_mode(Status::Mode mode)
    {
        m_status.error         = 0;
        m_status.spindle_motor = 1;
        m_status.seek_error    = 0;
        m_status.id_error      = 0;
        m_status.shell_open    = 0;
        m_status.read          = mode == Status::Mode::Read;
        m_status.seek          = mode == Status::Mode::Seek;
        m_status.play          = mode == Status::Mode::Play;
    }

    /**
     * @brief Unknown Command
     */
    void CDROM::UNK()
    {
        push_to_interrupt_fifo(5);
        push_to_response_fifo(0x11);
        push_to_response_fifo(0x40);
    }        
    
    /**
     * @brief Get Status Register
     */
    void CDROM::GETSTAT()
    {
        push_to_interrupt_fifo(3);
        push_to_response_fifo(m_status.raw);
    }    
    
    /**
     * Set Seek Target
     */
    void CDROM::SETLOC()
    {
        u8 minutes = bcd_to_binary(pop_from_parameter_fifo());
        u8 seconds = bcd_to_binary(pop_from_parameter_fifo());
        u8 sector  = bcd_to_binary(pop_from_parameter_fifo());

        TODO();

        push_to_interrupt_fifo(3);
        push_to_response_fifo(m_status.raw);
    }     
    
    /**
     * 
     */
    void CDROM::PLAY()
    {
        TODO();
    }       
    
    /**
     * 
     */
    void CDROM::FORWARD()
    {
        TODO();
    }    
    
    /**
     * 
     */
    void CDROM::BACKWARD()
    {
        TODO();
    }   
    
    /**
     * 
     */
    void CDROM::READN()
    {
        TODO();
    }      
    
    /**
     * @brief Activate Drive Motor
     */
    void CDROM::MOTORON()
    {
        m_status.spindle_motor = 1;

        push_to_interrupt_fifo(3);
        push_to_response_fifo(m_status.raw);
        push_to_interrupt_fifo(2);
        push_to_response_fifo(m_status.raw);
    }    
    
    /**
     * @brief Stop Motor With Magnetic Brakes
     */
    void CDROM::STOP()
    {
        set_status_mode(Status::Mode::None);
        m_status.spindle_motor = 0;

        push_to_interrupt_fifo(3);
        push_to_response_fifo(m_status.raw);
        push_to_interrupt_fifo(2);
        push_to_response_fifo(m_status.raw);
    }       
    
    /**
     * 
     */
    void CDROM::PAUSE()
    {
        TODO();
    }      
    
    /**
     * 
     */
    void CDROM::INIT()
    {
        TODO();
    }       
    
    /**
     * 
     */
    void CDROM::MUTE()
    {
        TODO();
    }       
    
    /**
     * 
     */
    void CDROM::DEMUTE()
    {
        TODO();
    }     
    
    /**
     * 
     */
    void CDROM::SETFILTER()
    {
        TODO();
    }  
    
    /**
     * 
     */
    void CDROM::SETMODE()
    {
        TODO();
    }    
    
    /**
     * 
     */
    void CDROM::GETPARAM()
    {
        TODO();
    }   
    
    /**
     * 
     */
    void CDROM::GETLOCL()
    {
        TODO();
    }    
    
    /**
     * 
     */
    void CDROM::GETLOCP()
    {
        TODO();
    }    
    
    /**
     * 
     */
    void CDROM::SETSESSION()
    {
        TODO();
    } 
    
    /**
     * 
     */
    void CDROM::GETTN()
    {
        TODO();
    }      
    
    /**
     * 
     */
    void CDROM::GETTD()
    {
        TODO();
    }      
    
    /**
     * 
     */
    void CDROM::SEEKL()
    {
        TODO();
    }      
    
    /**
     * 
     */
    void CDROM::SEEKP()
    {
        TODO();
    }      
    
    /**
     * 
     */
    void CDROM::TEST()
    {
        TODO();
    }       
    
    /**
     * 
     */
    void CDROM::GETID()
    {
        TODO();
    }      
    
    /**
     * 
     */
    void CDROM::READS()
    {
        TODO();
    }      
    
    /**
     * 
     */
    void CDROM::RESET()
    {
        TODO();
    }      
    
    /**
     * 
     */
    void CDROM::GETQ()
    {
        TODO();
    }       
    
    /**
     * 
     */
    void CDROM::READTOC()
    {
        TODO();
    }    
    
    /**
     * 
     */
    void CDROM::VIDEOCD()
    {
        TODO();
    }    
    
    /**
     * 
     */
    void CDROM::SECRET1()
    {
        TODO();
    }    
    
    /**
     * 
     */
    void CDROM::SECRET2()
    {
        TODO();
    }    
    
    /**
     * 
     */
    void CDROM::SECRET3()
    {
        TODO();
    }    
    
    /**
     * 
     */
    void CDROM::SECRET4()
    {
        TODO();
    }    
    
    /**
     * 
     */
    void CDROM::SECRET5()
    {
        TODO();
    }    
    
    /**
     * 
     */
    void CDROM::SECRET6()
    {
        TODO();
    }    
    
    /**
     * 
     */
    void CDROM::SECRET7()
    {
        TODO();
    }    
    
    /**
     * 
     */
    void CDROM::SECRETLOCK()
    {
        TODO();
    } 
    
}