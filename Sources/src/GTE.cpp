/**
 * @file      GTE.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation of PSX Geometry Transformation Engine
 *
 * @version   0.1
 *
 * @date      9. 11. 2023, 10:16 (created)
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

#include "GTE.hpp"

namespace PSX
{
    void GTE::execute(u32 num_steps)
    {
        MARK_UNUSED(num_steps);
    }

    u32 GTE::read(u32 address)
    {
        MARK_UNUSED(address);
        TODO();
    }

    void GTE::write(u32 address, u32 value)
    {
        MARK_UNUSED(address);
        MARK_UNUSED(value);
        TODO();
    }

    void GTE::reset()
    {
        m_vxyz0 = {0};
        m_vxyz1 = {0};
        m_vxyz2 = {0};
        m_rgbc.raw() = 0;
        m_otz.raw() = 0;
        m_ir[0] = 0;
        m_ir[1] = 0;
        m_ir[2] = 0;
        m_ir[3] = 0;
        m_sxyz = {0};
        m_crgb[0].raw() = 0;
        m_crgb[1].raw() = 0;
        m_crgb[2].raw() = 0;
        m_res1.raw() = 0;
        m_mac[0].raw() = 0;
        m_mac[1].raw() = 0;
        m_mac[2].raw() = 0;
        m_iorgb.raw() = 0;
        m_lzcs.raw() = 0;
        m_lzcr.raw() = 0;

        m_rotation_matrix = {0};
        m_translation_vector = {0};
        m_light_source_matrix = {0};
        m_background_color = {0};
        m_light_color_matrix = {0};
        m_far_color = {0};
        m_screen_offset.raw() = 0;
        m_projection_plane_distance.raw() = 0;
        m_depth_queing_parameter_coeff.raw() = 0;
        m_depth_queing_parameter_offset.raw() = 0;
        m_zsf3.raw() = 0;
        m_zsf4.raw() = 0;
        m_flag.raw() = 0;
    }

    /**
     * @brief since GTE is only accessible through the cop2 instruction we need to directly execute the instruction 
     */
    void GTE::execute(const GTEInstruction& ins)
    {
        (this->*m_handlers[ins.opcode])(ins);
    }

    /**
     * @brief 
     */
    void GTE::UNK(const GTEInstruction&)
    {
        TODO();
    }

    /**
     * @brief 
     */
    void GTE::RTPS(const GTEInstruction&)
    {
        TODO();
    } 

    /**
     * @brief 
     */
    void GTE::NCLIP(const GTEInstruction&)
    {
        TODO();
    }

    /**
     * @brief 
     */
    void GTE::OP(const GTEInstruction&)
    {
        TODO();
    }  

    /**
     *  @brief
     */
    void GTE::DPCS(const GTEInstruction&)
    {
        TODO();
    }  

    /**
     * @brief 
     */
    void GTE::INTPL(const GTEInstruction&)
    {
        TODO();
    } 

    /**
     * @brief 
     */
    void GTE::MVMVA(const GTEInstruction&)
    {
        TODO();
    } 

    /**
     * @brief 
     */
    void GTE::NCDS(const GTEInstruction&)
    {
        TODO();
    }  

    /**
     * @brief 
     */
    void GTE::CDP(const GTEInstruction&)
    {
        TODO();
    }   

    /**
     * @brief 
     */
    void GTE::NCDT(const GTEInstruction&)
    {
        TODO();
    }  

    /**
     * @brief 
     */
    void GTE::NCCS(const GTEInstruction&)
    {
        TODO();
    }  

    /**
     * @brief 
     */
    void GTE::CC(const GTEInstruction&)
    {
        TODO();
    }   

    /**
     * @brief 
     */
    void GTE::NCS(const GTEInstruction&)
    {
        TODO();
    }   

    /**
     * @brief 
     */
    void GTE::NCT(const GTEInstruction&)
    {
        TODO();
    }   

    /**
     * @brief 
     */
    void GTE::SQR(const GTEInstruction&)
    {
        TODO();
    }   

    /**
     * @brief 
     */
    void GTE::DCPL(const GTEInstruction&)
    {
        TODO();
    }  

    /**
     * @brief 
     */
    void GTE::DPCT(const GTEInstruction&)
    {
        TODO();
    }  

    /**
     * @brief 
     */
    void GTE::AVSZ3(const GTEInstruction&)
    {
        TODO();
    } 

    /**
     * @brief 
     */
    void GTE::AVSZ4(const GTEInstruction&)
    {
        TODO();
    } 

    /**
     * @brief 
     */
    void GTE::RTPT(const GTEInstruction&)
    {
        TODO();
    }  

    /**
     * @brief 
     */
    void GTE::GPF(const GTEInstruction&)
    {
        TODO();
    }   

    /**
     * @brief 
     */
    void GTE::GPL(const GTEInstruction&)
    {
        TODO();
    }   

    /**
     * @brief 
     */
    void GTE::NCCT(const GTEInstruction&)
    {
        TODO();
    }  
}