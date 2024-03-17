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
        virtual void serialize(std::shared_ptr<SaveState>&) override;
        virtual void deserialize(std::shared_ptr<SaveState>&) override;

        /**
         * @brief since GTE is only accessible through the cop2 instruction we need to directly execute the instruction 
         */
        void execute(const GTEInstruction&);

    private:

        /**
         * @brief multiply 3d vector by 3d vector
         */
        void multiply_and_translate(const GTEVector<s16>&, const GTEVector<s16>&, const GTEVector<s32>& translate);

        /**
         * @brief multiply 3x3 matrix a by 3d vector b and translate it
         */
        void multiply_and_translate(const GTEMatrix<s16>&, const GTEVector<s16>&, const GTEVector<s32>& translate);

        /**
         * @brief multiply 3x3 matrix by 3d vector and translate it (RTP command specialized)
         */
        s64 multiply_and_translate_rtp(const GTEMatrix<s16>&, const GTEVector<s16>&, const GTEVector<s32>& translate);

        /**
         * @brief multiply and translate 3x3 matrix by 3d vector
         */
        GTEVector<s64> multiply_and_translate_impl(const GTEMatrix<s16>&, const GTEVector<s16>&, const GTEVector<s32>& translate);

        /**
         * @brief check overflow/underflow of a value and assign it to mac and ir registers
         */
        void check_and_assign_result(s64 value, u32 accumulator_index, bool allow_negative);

        /**
         * @brief check overflow/underflow of a value and assign it to mac register
         */
        s64 check_and_assign_to_mac(s64 value, u32 accumulator_index);

        /**
         * @brief check overflow/underflow of a mac value and extend the sign to 64bit value
         */
        s64 check_and_extend_from_mac(s64 value, u32 accumulator_index);

        /**
         * @brief check overflow/underflow of a value and assign it to ir register
         */
        void check_and_assign_to_ir(s64 value, u32 accumulator_index, bool allow_negative);

        /**
         * @brief push new color to the crgb queue 
         */
        void push_to_crgb_fifo(const GTEVector<s32>& color);

        /**
         * @brief push new z coord to the screen queue 
         */
        void push_to_screen_z_fifo(s32);

        /**
         * @brief push new x and y coord to the screen queue 
         */
        void push_to_screen_xy_fifo(s32, s32);

        /**
         * @brief general implementation for the RTPS/RTPT commands 
         */
        void rtps_impl(u32 general_purpose_vector_index, bool set_mac0);

         /**
         * @brief general implementation for the NCDS/NCDT commands 
         */
        void ncds_impl(u32 general_purpose_vector_index);

        /**
         * @brief divide two values using the UNR division algorithm 
         */
        u32 unr_division(u32 numerator, u32 denominator);

        /**
         * instructions 
         */
        void UNK(const GTEInstruction&);   /// unknown opcode
        void RTPS(const GTEInstruction&);  /// perspective transformation single
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
        void AVSZ3(const GTEInstruction&); /// average of three z values
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
         * @brief status register for indicating calculation errors 
         */
        union ErrorFlags
        {
            struct
            {
                u32:                      12;
                u32 ir0_saturated:        1;
                u32 sy2_saturated:        1;
                u32 sx2_saturated:        1;
                u32 mac0_overflow:        1;
                u32 mac0_underflow:       1;
                u32 divide_overflow:      1;
                u32 sz3_or_otz_saturated: 1;
                u32 color_b_saturated:    1;
                u32 color_g_saturated:    1;
                u32 color_r_saturated:    1;
                u32 ir3_saturated:        1;
                u32 ir2_saturated:        1;
                u32 ir1_saturated:        1;
                u32 mac3_overflow:        1;
                u32 mac2_overflow:        1;
                u32 mac1_overflow:        1;
                u32 mac3_underflow:       1;
                u32 mac2_underflow:       1;
                u32 mac1_underflow:       1;
                u32 error:                1;
            };

            u32 raw;
        };

        /**
         * data registers 
         */
        GTEVector<s16> m_vxyz0;   /// General Purpose Vector 0
        GTEVector<s16> m_vxyz1;   /// General Purpose Vector 1
        GTEVector<s16> m_vxyz2;   /// General Purpose Vector 2
        Register<u32>  m_rgbc;    /// Color/Code Value
        Register<u16>  m_otz;     /// Average Z Value (For Ordering Table)
        Register<s16>  m_ir[4];   /// 16bit Accumulator For Interpolation And Vector
        GTEVector<s16> m_sxyz[4]; /// Screen XY-Coordinate 3-slot Queue and Z-Coordinate 4-slot Queue TODO: z is unsigned
        Register<u32>  m_crgb[3]; /// Color CRGM-Code/Color 3-slot Queue
        Register<u32>  m_res1;    /// Prohibited???
        Register<s32>  m_mac[4];  /// Math Accumulators
        Register<u16>  m_iorgb;   /// Convert RGB Colors (48bit vs 15bit)
        Register<s32>  m_lzcs;    ///  v
                                  ///  Count Leading-Zeroes/Ones
        Register<s32>  m_lzcr;    ///  ^

        /**
         * control registers 
         */
        GTEMatrix<s16> m_rotation_matrix;               /// Rotation Matrix     (3x3)
        GTEVector<s32> m_translation_vector;            /// Translation Vector  (X,Y,Z)
        GTEMatrix<s16> m_light_source_matrix;           /// Light Source Matrix (3x3)
        GTEVector<s32> m_background_color;              /// Background Color    (R,G,B)
        GTEMatrix<s16> m_light_color_matrix;            /// Light Color Matrix  (3x3)
        GTEVector<s32> m_far_color;                     /// Far Color           (R,G,B)
        Register<s32>  m_screen_offset[2];              /// Screen Offset       (X,Y)
        Register<u16>  m_projection_plane_distance;     /// Projection Plane Distance
        Register<s16>  m_depth_queing_parameter_coeff;  /// Depth Queing Parameter Coefficient
        Register<s32>  m_depth_queing_parameter_offset; /// Depth Queing Parameter offset
        Register<s16>  m_zsf3;                          /// v
                                                        /// Average Z Scale Factors
        Register<s16>  m_zsf4;                          /// ^
        ErrorFlags     m_error_flags;                   /// Calculation Error Container

        /**
         * meta state 
         */
        GTEInstruction m_current_instruction;
    };
}

#endif // GTE_HPP