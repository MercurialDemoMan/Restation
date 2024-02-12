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
                return m_status.raw;
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
                m_status.response_fifo_empty = 0;

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
        if(m_status.index == 0 || m_status.index == 2)
            return m_interrupt_enable;
        
        if(m_status.index == 1 || m_status.index == 3)
        {
            if(!m_interrupt_fifo.empty())
            {
                return 0b1110'0000 | (m_interrupt_fifo.top().value() & 0b0111);
            }

            return 0b1110'0000;
        }
    }

    void CDROM::UNK()
    {
        TODO();
    }        
    
    void CDROM::GETSTAT()
    {
        TODO();
    }    
    
    void CDROM::SETLOC()
    {
        TODO();
    }     
    
    void CDROM::PLAY()
    {
        TODO();
    }       
    
    void CDROM::FORWARD()
    {
        TODO();
    }    
    
    void CDROM::BACKWARD()
    {
        TODO();
    }   
    
    void CDROM::READN()
    {
        TODO();
    }      
    
    void CDROM::MOTORON()
    {
        TODO();
    }    
    
    void CDROM::STOP()
    {
        TODO();
    }       
    
    void CDROM::PAUSE()
    {
        TODO();
    }      
    
    void CDROM::INIT()
    {
        TODO();
    }       
    
    void CDROM::MUTE()
    {
        TODO();
    }       
    
    void CDROM::DEMUTE()
    {
        TODO();
    }     
    
    void CDROM::SETFILTER()
    {
        TODO();
    }  
    
    void CDROM::SETMODE()
    {
        TODO();
    }    
    
    void CDROM::GETPARAM()
    {
        TODO();
    }   
    
    void CDROM::GETLOCL()
    {
        TODO();
    }    
    
    void CDROM::GETLOCP()
    {
        TODO();
    }    
    
    void CDROM::SETSESSION()
    {
        TODO();
    } 
    
    void CDROM::GETTN()
    {
        TODO();
    }      
    
    void CDROM::GETTD()
    {
        TODO();
    }      
    
    void CDROM::SEEKL()
    {
        TODO();
    }      
    
    void CDROM::SEEKP()
    {
        TODO();
    }      
    
    void CDROM::TEST()
    {
        TODO();
    }       
    
    void CDROM::GETID()
    {
        TODO();
    }      
    
    void CDROM::READS()
    {
        TODO();
    }      
    
    void CDROM::RESET()
    {
        TODO();
    }      
    
    void CDROM::GETQ()
    {
        TODO();
    }       
    
    void CDROM::READTOC()
    {
        TODO();
    }    
    
    void CDROM::VIDEOCD()
    {
        TODO();
    }    
    
    void CDROM::SECRET1()
    {
        TODO();
    }    
    
    void CDROM::SECRET2()
    {
        TODO();
    }    
    
    void CDROM::SECRET3()
    {
        TODO();
    }    
    
    void CDROM::SECRET4()
    {
        TODO();
    }    
    
    void CDROM::SECRET5()
    {
        TODO();
    }    
    
    void CDROM::SECRET6()
    {
        TODO();
    }    
    
    void CDROM::SECRET7()
    {
        TODO();
    }    
    
    void CDROM::SECRETLOCK()
    {
        TODO();
    } 
    
}