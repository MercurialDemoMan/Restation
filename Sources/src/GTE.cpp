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
#include "Utils.hpp"
#include <bit>
#include <algorithm>

namespace PSX
{
    void GTE::execute(u32 num_steps)
    {
        MARK_UNUSED(num_steps);
    }

    u32 GTE::read(u32 address)
    {
        switch(address)
        {
            case  0: return (u16(m_vxyz0.y) << 16) | u16(m_vxyz0.x); // prevent sign extension
            case  1: return (s32(m_vxyz0.z));                        // extend sign
            case  2: return (u16(m_vxyz1.y) << 16) | u16(m_vxyz1.x); // prevent sign extension
            case  3: return (s32(m_vxyz1.z));                        // extend sign
            case  4: return (u16(m_vxyz2.y) << 16) | u16(m_vxyz2.x); // prevent sign extension
            case  5: return (s32(m_vxyz2.z));                        // extend sign
            case  6: return m_rgbc.raw();
            case  7: return m_otz.raw();
            case  8: return s32(m_ir[0].raw()); // extend sign
            case  9: return s32(m_ir[1].raw()); // extend sign
            case 10: return s32(m_ir[2].raw()); // extend sign
            case 11: return s32(m_ir[3].raw()); // extend sign
            case 12: return (u16(m_sxyz[0].y) << 16) | u16(m_sxyz[0].x); // prevent sign extension
            case 13: return (u16(m_sxyz[1].y) << 16) | u16(m_sxyz[1].x); // prevent sign extension
            case 14: 
            case 15: return (u16(m_sxyz[2].y) << 16) | u16(m_sxyz[2].x); // prevent sign extension
            case 16: return u16(m_sxyz[0].z); // prevent sign extesion
            case 17: return u16(m_sxyz[1].z); // prevent sign extesion
            case 18: return u16(m_sxyz[2].z); // prevent sign extesion
            case 19: return u16(m_sxyz[3].z); // prevent sign extesion
            case 20: return m_crgb[0].raw();
            case 21: return m_crgb[1].raw();
            case 22: return m_crgb[2].raw();
            case 23: return m_res1.raw(); // prohibited???
            case 24: return m_mac[0].raw();
            case 25: return m_mac[1].raw();
            case 26: return m_mac[2].raw();
            case 27: return m_mac[3].raw();
            case 28:
            case 29:
            {
                TODO();
            }
            case 30: return m_lzcs.raw();
            case 31: return m_lzcr.raw();
            case 32: return (u16(m_rotation_matrix.at(1, 0)) << 16) | u16(m_rotation_matrix.at(0, 0));
            case 33: return (u16(m_rotation_matrix.at(0, 1)) << 16) | u16(m_rotation_matrix.at(2, 0));
            case 34: return (u16(m_rotation_matrix.at(2, 1)) << 16) | u16(m_rotation_matrix.at(1, 1));
            case 35: return (u16(m_rotation_matrix.at(1, 2)) << 16) | u16(m_rotation_matrix.at(0, 2));
            case 36: return s32(m_rotation_matrix.at(2, 2));
            case 37: return m_translation_vector.x;
            case 38: return m_translation_vector.y;
            case 39: return m_translation_vector.z;
            case 40: return (u16(m_light_source_matrix.at(1, 0)) << 16) | u16(m_light_source_matrix.at(0, 0));
            case 41: return (u16(m_light_source_matrix.at(0, 1)) << 16) | u16(m_light_source_matrix.at(2, 0));
            case 42: return (u16(m_light_source_matrix.at(2, 1)) << 16) | u16(m_light_source_matrix.at(1, 1));
            case 43: return (u16(m_light_source_matrix.at(1, 2)) << 16) | u16(m_light_source_matrix.at(0, 2));
            case 44: return s32(m_light_source_matrix.at(2, 2));
            case 45: return m_background_color.r;
            case 46: return m_background_color.g;
            case 47: return m_background_color.b;
            case 48: return (u16(m_light_color_matrix.at(1, 0)) << 16) | u16(m_light_color_matrix.at(0, 0));
            case 49: return (u16(m_light_color_matrix.at(0, 1)) << 16) | u16(m_light_color_matrix.at(2, 0));
            case 50: return (u16(m_light_color_matrix.at(2, 1)) << 16) | u16(m_light_color_matrix.at(1, 1));
            case 51: return (u16(m_light_color_matrix.at(1, 2)) << 16) | u16(m_light_color_matrix.at(0, 2));
            case 52: return s32(m_light_color_matrix.at(2, 2));
            case 53: return m_far_color.r;
            case 54: return m_far_color.g;
            case 55: return m_far_color.b;
            case 56: return m_screen_offset[0].raw();
            case 57: return m_screen_offset[1].raw();
            case 58: return s32(s16(m_projection_plane_distance.raw())); // sign extend
            case 59: return s32(m_depth_queing_parameter_coeff.raw());
            case 60: return s32(m_depth_queing_parameter_offset.raw());
            case 61: return s32(s16(m_zsf3.raw())); // sign extend
            case 62: return s32(s16(m_zsf4.raw())); // sign extend
            case 63:
            {
                TODO();
            }
            default:
            {
                UNREACHABLE();
            }
        }
    }

    void GTE::write(u32 address, u32 value)
    {
        switch(address)
        {
            case  0: 
            { 
                m_vxyz0.x = value; 
                m_vxyz0.y = value >> 16; 
            } break;
            case  1: { m_vxyz0.z = value; } break;
            case  2:
            { 
                m_vxyz1.x = value; 
                m_vxyz1.y = value >> 16; 
            } break;
            case  3: { m_vxyz1.z = value; } break;
            case  4:
            { 
                m_vxyz2.x = value; 
                m_vxyz2.y = value >> 16; 
            } break;
            case  5: { m_vxyz2.z = value; } break;
            case  6: { m_rgbc.raw() = value; } break;
            case  7: { m_otz.raw() = value; } break;
            case  8: { m_ir[0].raw() = value; } break;
            case  9: { m_ir[1].raw() = value; } break;
            case 10: { m_ir[2].raw() = value; } break;
            case 11: { m_ir[3].raw() = value; } break;
            case 12:
            { 
                m_sxyz[0].x = value; 
                m_sxyz[0].y = value >> 16; 
            } break;
            case 13:
            { 
                m_sxyz[1].x = value; 
                m_sxyz[1].y = value >> 16; 
            } break;
            case 14: 
            { 
                m_sxyz[2].x = value; 
                m_sxyz[2].y = value >> 16; 
            } break;
            case 15:
            {
                //TODO: replace by queue? but then it will always be of size 3...
                m_sxyz[0].x = m_sxyz[1].x;
                m_sxyz[0].y = m_sxyz[1].y;

                m_sxyz[1].x = m_sxyz[2].x;
                m_sxyz[1].y = m_sxyz[2].y;

                m_sxyz[2].x = value;
                m_sxyz[2].y = value >> 16;
            } break;
            case 16: { m_sxyz[0].z = value; } break;
            case 17: { m_sxyz[1].z = value; } break;
            case 18: { m_sxyz[2].z = value; } break;
            case 19: { m_sxyz[3].z = value; } break;
            case 20: { m_crgb[0].raw() = value; } break;
            case 21: { m_crgb[1].raw() = value; } break;
            case 22: { m_crgb[2].raw() = value; } break;
            case 23: { m_res1.raw() = value; } break; // prohibited???
            case 24: { m_mac[0].raw() = value; } break;
            case 25: { m_mac[1].raw() = value; } break;
            case 26: { m_mac[2].raw() = value; } break;
            case 27: { m_mac[3].raw() = value; } break;
            case 28:
            {
                TODO();
            }
            case 29: {} break;
            case 30:
            {
                m_lzcs.raw() = value;
                m_lzcr.raw() = std::countl_zero(value);
            }
            case 31: {} break;
            case 32: 
            { 
                m_rotation_matrix.at(0, 0) = value; 
                m_rotation_matrix.at(1, 0) = value >> 16;
            } break;
            case 33:
            { 
                m_rotation_matrix.at(2, 0) = value; 
                m_rotation_matrix.at(0, 1) = value >> 16;
            } break;
            case 34:
            { 
                m_rotation_matrix.at(1, 1) = value; 
                m_rotation_matrix.at(2, 1) = value >> 16;
            } break;
            case 35:
            { 
                m_rotation_matrix.at(0, 2) = value; 
                m_rotation_matrix.at(1, 2) = value >> 16;
            } break;
            case 36: { m_rotation_matrix.at(2, 2) = value; } break;
            case 37: { m_translation_vector.x = value; } break;
            case 38: { m_translation_vector.y = value; } break;
            case 39: { m_translation_vector.z = value; } break;
            case 40:
            { 
                m_light_source_matrix.at(0, 0) = value; 
                m_light_source_matrix.at(1, 0) = value >> 16;
            } break;
            case 41:
            { 
                m_light_source_matrix.at(2, 0) = value; 
                m_light_source_matrix.at(0, 1) = value >> 16;
            } break;
            case 42:
            { 
                m_light_source_matrix.at(1, 1) = value; 
                m_light_source_matrix.at(2, 1) = value >> 16;
            } break;
            case 43:
            { 
                m_light_source_matrix.at(0, 2) = value; 
                m_light_source_matrix.at(1, 2) = value >> 16;
            } break;
            case 44: { m_light_source_matrix.at(2, 2) = value; } break;
            case 45: { m_background_color.x = value; } break;
            case 46: { m_background_color.y = value; } break;
            case 47: { m_background_color.z = value; } break;
            case 48:
            { 
                m_light_color_matrix.at(0, 0) = value; 
                m_light_color_matrix.at(1, 0) = value >> 16;
            } break;
            case 49:
            { 
                m_light_color_matrix.at(2, 0) = value; 
                m_light_color_matrix.at(0, 1) = value >> 16;
            } break;
            case 50: 
            { 
                m_light_color_matrix.at(1, 1) = value; 
                m_light_color_matrix.at(2, 1) = value >> 16;
            } break;
            case 51:
            { 
                m_light_color_matrix.at(0, 2) = value; 
                m_light_color_matrix.at(1, 2) = value >> 16;
            } break;
            case 52: { m_light_color_matrix.at(2, 2) = value; } break;
            case 53: { m_far_color.r = value; } break;
            case 54: { m_far_color.g = value; } break;
            case 55: { m_far_color.b = value; } break;
            case 56: { m_screen_offset[0].raw() = value; } break;
            case 57: { m_screen_offset[1].raw() = value; } break;
            case 58: { m_projection_plane_distance.raw() = value; } break;
            case 59: { m_depth_queing_parameter_coeff.raw() = value; } break;
            case 60: { m_depth_queing_parameter_offset.raw() = value; } break;
            case 61: { m_zsf3.raw() = value; } break;
            case 62: { m_zsf4.raw() = value; } break;
            case 63:
            {
                TODO();
            }
            default:
            {
                UNREACHABLE();
            }
        }
    }

    void GTE::reset()
    {
        m_vxyz0.reset();
        m_vxyz1.reset();
        m_vxyz2.reset();
        m_rgbc.raw() = 0;
        m_otz.raw() = 0;
        m_ir[0] = 0;
        m_ir[1] = 0;
        m_ir[2] = 0;
        m_ir[3] = 0;
        m_sxyz[0].reset();
        m_sxyz[1].reset();
        m_sxyz[2].reset();
        m_sxyz[3].reset();
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

        m_rotation_matrix.reset();
        m_translation_vector.reset();
        m_light_source_matrix.reset();
        m_background_color.reset();
        m_light_color_matrix.reset();
        m_far_color.reset();
        m_screen_offset[0].raw() = 0;
        m_screen_offset[1].raw() = 0;
        m_projection_plane_distance.raw() = 0;
        m_depth_queing_parameter_coeff.raw() = 0;
        m_depth_queing_parameter_offset.raw() = 0;
        m_zsf3.raw() = 0;
        m_zsf4.raw() = 0;
        m_error_flags.raw = 0;
    }

    /**
     * @brief since GTE is only accessible through the cop2 instruction we need to directly execute the instruction 
     */
    void GTE::execute(const GTEInstruction& ins)
    {
        m_error_flags.raw = 0;
        m_current_instruction = ins;

        (this->*m_handlers[ins.opcode])(ins);
    }

    /**
     * @brief multiply 3d vector a by 3d vector b
     */
    void GTE::multiply(const GTEVector<s16>& a, const GTEVector<s16>& b)
    {
        check_and_assign_result(a.x * b.x, 1, !m_current_instruction.lm);
        check_and_assign_result(a.y * b.y, 2, !m_current_instruction.lm);
        check_and_assign_result(a.z * b.z, 3, !m_current_instruction.lm);
    }

    /**
     * @brief multiply 3x3 matrix a by 3d vector b
     */
    void GTE::multiply(const GTEMatrix<s16>& a, const GTEVector<s16>& b)
    {
        GTEVector<s64> result = GTEVector<s64> { 0 };

        result.x = 
        check_and_extend_from_mac
        (
            check_and_extend_from_mac
            (
                check_and_extend_from_mac
                (
                    a.at(0, 0) * b.x, 1
                ) + 
                a.at(1, 0) * b.y, 1
            ) + 
            a.at(2, 0) * b.z, 1
        );

        result.y =
        check_and_extend_from_mac
        (
            check_and_extend_from_mac
            (
                check_and_extend_from_mac
                (
                    a.at(0, 1) * b.x, 2
                ) + 
                a.at(1, 1) * b.y, 2
            ) + 
            a.at(2, 1) * b.z, 2
        );

        result.z =
        check_and_extend_from_mac
        (
            check_and_extend_from_mac
            (
                check_and_extend_from_mac
                (
                    a.at(0, 2) * b.x, 3
                ) + 
                a.at(1, 2) * b.y, 3
            ) + 
            a.at(2, 2) * b.z, 3
        );

        check_and_assign_result(result.x, 1, !m_current_instruction.lm);
        check_and_assign_result(result.y, 2, !m_current_instruction.lm);
        check_and_assign_result(result.z, 3, !m_current_instruction.lm);
    }

    /**
     * @brief check overflow/underflow of a value and assign it to mac and ir registers
     */
    void GTE::check_and_assign_result(s64 value, u32 accumulator_index, bool allow_negative)
    {
        check_and_assign_to_ir
        (
            check_and_assign_to_mac(value, accumulator_index), 
            accumulator_index,
            allow_negative
        );
    }

    /**
     * @brief check overflow/underflow of a value and assign it to mac register
     */
    s64 GTE::check_and_assign_to_mac(s64 value, u32 accumulator_index)
    {
        // check overflow/underflow
        switch(accumulator_index)
        {
            case 0:
            {
                m_error_flags.mac0_overflow = check_overflow_bits<32>(value);
                m_error_flags.mac0_underflow = check_underflow_bits<32>(value);
            } break;
            case 1:
            {
                m_error_flags.mac1_overflow = check_overflow_bits<44>(value);
                m_error_flags.mac1_underflow = check_underflow_bits<44>(value);
            } break;
            case 2:
            {
                m_error_flags.mac2_overflow = check_overflow_bits<44>(value);
                m_error_flags.mac2_underflow = check_underflow_bits<44>(value);
            } break;
            case 3:
            {
                m_error_flags.mac3_overflow = check_overflow_bits<44>(value);
                m_error_flags.mac3_underflow = check_underflow_bits<44>(value);
            } break;
            default:
            {
                UNREACHABLE();
            } break;
        }

        // get rid of 12bit fraction in mac1, mac2 and mac3
        if(accumulator_index != 0 && m_current_instruction.shift_fraction)
        {
            value >>= 12;
        }

        // assign to mac
        m_mac[accumulator_index] = value;
        return value;
    }

    /**
     * @brief check overflow/underflow of a value and assign it to mac register
     */
    s64 GTE::check_and_extend_from_mac(s64 value, u32 accumulator_index)
    {
        switch(accumulator_index)
        {
            case 0:
            {
                m_error_flags.mac0_overflow = check_overflow_bits<32>(value);
                m_error_flags.mac0_underflow = check_underflow_bits<32>(value);
                return extend_sign<s64, 32>(value);
            } break;
            case 1:
            {
                m_error_flags.mac1_overflow = check_overflow_bits<44>(value);
                m_error_flags.mac1_underflow = check_underflow_bits<44>(value);
                return extend_sign<s64, 44>(value);
            } break;
            case 2:
            {
                m_error_flags.mac2_overflow = check_overflow_bits<44>(value);
                m_error_flags.mac2_underflow = check_underflow_bits<44>(value);
                return extend_sign<s64, 44>(value);
            } break;
            case 3:
            {
                m_error_flags.mac3_overflow = check_overflow_bits<44>(value);
                m_error_flags.mac3_underflow = check_underflow_bits<44>(value);
                return extend_sign<s64, 44>(value);
            } break;
            default:
            {
                UNREACHABLE();
            } break;
        }

        return 0;
    }

    /**
     * @brief check overflow/underflow of a value and assign it to vector ir register
     */
    void GTE::check_and_assign_to_ir(s64 value, u32 accumulator_index, bool allow_negative)
    {
        s64 clamped_value = std::clamp(value, s64(allow_negative ? -0x8000 : 0), s64(0x7FFF));

        switch(accumulator_index)
        {
            case 1:
            {
                m_error_flags.ir1_saturated = value != clamped_value;
            } break;
            case 2:
            {
                m_error_flags.ir2_saturated = value != clamped_value;
            } break;
            case 3:
            {
                m_error_flags.ir3_saturated = value != clamped_value;
            } break;
            default:
            {
                UNREACHABLE();
            } break;
        }

        m_ir[accumulator_index] = clamped_value;
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
     * @brief normal clipping
     */
    void GTE::NCLIP(const GTEInstruction&)
    {
        auto result = m_sxyz[0].x * m_sxyz[1].y + m_sxyz[1].x * m_sxyz[2].y +
                      m_sxyz[2].x * m_sxyz[0].y - m_sxyz[0].x * m_sxyz[2].y -
                      m_sxyz[1].x * m_sxyz[0].y - m_sxyz[2].x * m_sxyz[1].y;

        MARK_UNUSED(check_and_assign_to_mac(result, 0));
    }

    /**
     * @brief 
     */
    void GTE::OP(const GTEInstruction&)
    {
        check_and_assign_result(m_rotation_matrix.at(1, 1) * m_ir[3].raw() - m_rotation_matrix.at(2, 2) * m_ir[2].raw(), 1, !m_current_instruction.lm);
        check_and_assign_result(m_rotation_matrix.at(1, 1) * m_ir[3].raw() - m_rotation_matrix.at(2, 2) * m_ir[2].raw(), 1, !m_current_instruction.lm);
        check_and_assign_result(m_rotation_matrix.at(1, 1) * m_ir[3].raw() - m_rotation_matrix.at(2, 2) * m_ir[2].raw(), 1, !m_current_instruction.lm);
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