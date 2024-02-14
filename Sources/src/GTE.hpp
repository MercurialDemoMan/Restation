/**
 * @file      GTE.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX Geometry Transformation Engine
 *
 * @version   0.1
 *
 * @date      9. 11. 2023, 10:12 (created)
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

#ifndef GTE_HPP
#define GTE_HPP

#include <memory>
#include "Component.hpp"
#include "GTEInstruction.hpp"
#include "GTETypes.hpp"

namespace PSX
{
    /**
     * @brief PSX Geometry Transformation Engine
     */
    class GTE final : public Component
    {
    public:

        GTE()
        {
            reset();
        }
        
        virtual ~GTE() override = default;

        virtual void execute(u32 num_steps) override;
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;

        /**
         * @brief since GTE is only accessible through the cop2 instruction we need to directly execute the instruction 
         */
        void execute(const GTEInstruction&);

    private:

        /**
         * instructions 
         */
        void UNK(const GTEInstruction&);   /// unknown opcode
        void RTPS(const GTEInstruction&);  /// perspective transofrmation single
        void NCLIP(const GTEInstruction&); /// normal clipping
        void OP(const GTEInstruction&);    /// outer product of 2 vectors
        void DPCS(const GTEInstruction&);  /// depth cueing single
        void INTPL(const GTEInstruction&); /// interpolation of a vector and far color vector
        void MVMVA(const GTEInstruction&); /// multiply vector by matrix and add vector
        void NCDS(const GTEInstruction&);  /// normal color depth cue single vector
        void CDP(const GTEInstruction&);   /// color depth que
        void NCDT(const GTEInstruction&);  /// normal color depth cue triple vectors
        void NCCS(const GTEInstruction&);  /// normal color color single vector
        void CC(const GTEInstruction&);    /// color color
        void NCS(const GTEInstruction&);   /// normal color single
        void NCT(const GTEInstruction&);   /// normal color triple
        void SQR(const GTEInstruction&);   /// square of vector IR
        void DCPL(const GTEInstruction&);  /// depth cue color light
        void DPCT(const GTEInstruction&);  /// depth cueing triple
        void AVSZ3(const GTEInstruction&); /// average of three Z values
        void AVSZ4(const GTEInstruction&); /// average of four z values
        void RTPT(const GTEInstruction&);  /// perspective transformation triple
        void GPF(const GTEInstruction&);   /// general purpose interpolation
        void GPL(const GTEInstruction&);   /// general purpose interpolation with base
        void NCCT(const GTEInstruction&);  /// normal color color triple vector

        /**
         * instruction handler map
         */
        typedef void(GTE::*OpcodeHandler)(const GTEInstruction&);
        OpcodeHandler m_handlers[64] =
        {
            &GTE::UNK,  &GTE::RTPS,  &GTE::UNK,   &GTE::UNK,  &GTE::UNK,  &GTE::UNK,   &GTE::NCLIP, &GTE::UNK,
            &GTE::UNK,  &GTE::UNK,   &GTE::UNK,   &GTE::UNK,  &GTE::OP,   &GTE::UNK,   &GTE::UNK,   &GTE::UNK,
            &GTE::DPCS, &GTE::INTPL, &GTE::MVMVA, &GTE::NCDS, &GTE::CDP,  &GTE::UNK,   &GTE::NCDT,  &GTE::UNK,
            &GTE::UNK,  &GTE::UNK,   &GTE::UNK,   &GTE::NCCS, &GTE::CC,   &GTE::UNK,   &GTE::NCS,   &GTE::UNK,
            &GTE::NCT,  &GTE::UNK,   &GTE::UNK,   &GTE::UNK,  &GTE::UNK,  &GTE::UNK,   &GTE::UNK,   &GTE::UNK,
            &GTE::SQR,  &GTE::DCPL,  &GTE::DPCT,  &GTE::UNK,  &GTE::UNK,  &GTE::AVSZ3, &GTE::AVSZ4, &GTE::UNK,
            &GTE::RTPT, &GTE::UNK,   &GTE::UNK,   &GTE::UNK,  &GTE::UNK,  &GTE::UNK,   &GTE::UNK,   &GTE::UNK,
            &GTE::UNK,  &GTE::UNK,   &GTE::UNK,   &GTE::UNK,  &GTE::UNK,  &GTE::GPF,   &GTE::GPL,   &GTE::NCCT,
        };

        /**
         * data registers 
         */
        GTEVector<s16> m_vxyz0;
        GTEVector<s16> m_vxyz1;
        GTEVector<s16> m_vxyz2;
        Register<u32>  m_rgbc;
        Register<u16>  m_otz;
        Register<s16>  m_ir[4];
        GTEVector<s16> m_sxyz; // TODO: z is unsigned
        Register<u32>  m_crgb[3];
        Register<u32>  m_res1;
        Register<s32>  m_mac[4];
        Register<u16>  m_iorgb;
        Register<s32>  m_lzcs;
        Register<s32>  m_lzcr;

        /**
         * control registers 
         */
        GTEMatrix<s16> m_rotation_matrix;
        GTEVector<s32> m_translation_vector;
        GTEMatrix<s16> m_light_source_matrix;
        GTEVector<s32> m_background_color;
        GTEMatrix<s16> m_light_color_matrix;
        GTEVector<s32> m_far_color;
        Register<s32>  m_screen_offset;
        Register<u16>  m_projection_plane_distance;
        Register<s16>  m_depth_queing_parameter_coeff;
        Register<s32>  m_depth_queing_parameter_offset;
        Register<s16>  m_zsf3;
        Register<s16>  m_zsf4;
        Register<u32>  m_flag;
    };
}

#endif // GTE_HPP