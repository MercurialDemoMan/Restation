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
 * @section License
 * This file is part of the Restation \n
 *
 * Copyright (C) 2023
 *
 * This file is part of Restation. Restation is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Restation is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Restation. If not, see http://www.gnu.org/licenses/.
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
            case 12: return (u16(m_sxy[0].y) << 16) | u16(m_sxy[0].x); // prevent sign extension
            case 13: return (u16(m_sxy[1].y) << 16) | u16(m_sxy[1].x); // prevent sign extension
            case 14: 
            case 15: return (u16(m_sxy[2].y) << 16) | u16(m_sxy[2].x); // prevent sign extension
            case 16: return u16(m_sz[0].raw()); // prevent sign extesion
            case 17: return u16(m_sz[1].raw()); // prevent sign extesion
            case 18: return u16(m_sz[2].raw()); // prevent sign extesion
            case 19: return u16(m_sz[3].raw()); // prevent sign extesion
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
                m_error_flags.error = !!(m_error_flags.raw & 0x4087'E000);
                return m_error_flags.raw;
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
                m_sxy[0].x = value; 
                m_sxy[0].y = value >> 16; 
            } break;
            case 13:
            { 
                m_sxy[1].x = value; 
                m_sxy[1].y = value >> 16; 
            } break;
            case 14: 
            { 
                m_sxy[2].x = value; 
                m_sxy[2].y = value >> 16; 
            } break;
            case 15:
            {
                //TODO: replace by queue? but then it will always be of size 3...
                m_sxy[0].x = m_sxy[1].x;
                m_sxy[0].y = m_sxy[1].y;

                m_sxy[1].x = m_sxy[2].x;
                m_sxy[1].y = m_sxy[2].y;

                m_sxy[2].x = value;
                m_sxy[2].y = value >> 16;
            } break;
            case 16: { m_sz[0].raw() = value; } break;
            case 17: { m_sz[1].raw() = value; } break;
            case 18: { m_sz[2].raw() = value; } break;
            case 19: { m_sz[3].raw() = value; } break;
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
        m_sxy[0].reset();
        m_sxy[1].reset();
        m_sxy[2].reset();
        m_sz[0].raw() = 0;
        m_sz[1].raw() = 0;
        m_sz[2].raw() = 0;
        m_sz[3].raw() = 0;
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

    void GTE::serialize(std::shared_ptr<SaveState>& save_state)
    {
        save_state->serialize_from(m_vxyz0);
        save_state->serialize_from(m_vxyz1);
        save_state->serialize_from(m_vxyz2);
        save_state->serialize_from(m_rgbc);
        save_state->serialize_from(m_otz);
        save_state->serialize_from(m_ir[0]);
        save_state->serialize_from(m_ir[1]);
        save_state->serialize_from(m_ir[2]);
        save_state->serialize_from(m_ir[3]);
        save_state->serialize_from(m_sxy[0]);
        save_state->serialize_from(m_sxy[1]);
        save_state->serialize_from(m_sxy[2]);
        save_state->serialize_from(m_sz[0]);
        save_state->serialize_from(m_sz[1]);
        save_state->serialize_from(m_sz[2]);
        save_state->serialize_from(m_sz[3]);
        save_state->serialize_from(m_res1);
        save_state->serialize_from(m_mac[0]);
        save_state->serialize_from(m_mac[1]);
        save_state->serialize_from(m_mac[2]);
        save_state->serialize_from(m_mac[3]);
        save_state->serialize_from(m_iorgb);
        save_state->serialize_from(m_lzcs);
        save_state->serialize_from(m_lzcr);
        save_state->serialize_from(m_rotation_matrix);
        save_state->serialize_from(m_translation_vector);
        save_state->serialize_from(m_light_source_matrix);
        save_state->serialize_from(m_background_color);
        save_state->serialize_from(m_light_color_matrix);
        save_state->serialize_from(m_far_color);
        save_state->serialize_from(m_screen_offset[0]);
        save_state->serialize_from(m_screen_offset[1]);
        save_state->serialize_from(m_projection_plane_distance);
        save_state->serialize_from(m_depth_queing_parameter_coeff);
        save_state->serialize_from(m_depth_queing_parameter_offset);
        save_state->serialize_from(m_zsf3);
        save_state->serialize_from(m_zsf4);
        save_state->serialize_from(m_error_flags.raw);
        save_state->serialize_from(m_current_instruction);
    }

    void GTE::deserialize(std::shared_ptr<SaveState>& save_state)
    {
        save_state->deserialize_to(m_vxyz0);
        save_state->deserialize_to(m_vxyz1);
        save_state->deserialize_to(m_vxyz2);
        save_state->deserialize_to(m_rgbc);
        save_state->deserialize_to(m_otz);
        save_state->deserialize_to(m_ir[0]);
        save_state->deserialize_to(m_ir[1]);
        save_state->deserialize_to(m_ir[2]);
        save_state->deserialize_to(m_ir[3]);
        save_state->deserialize_to(m_sxy[0]);
        save_state->deserialize_to(m_sxy[1]);
        save_state->deserialize_to(m_sxy[2]);
        save_state->deserialize_to(m_sz[0]);
        save_state->deserialize_to(m_sz[1]);
        save_state->deserialize_to(m_sz[2]);
        save_state->deserialize_to(m_sz[3]);
        save_state->deserialize_to(m_res1);
        save_state->deserialize_to(m_mac[0]);
        save_state->deserialize_to(m_mac[1]);
        save_state->deserialize_to(m_mac[2]);
        save_state->deserialize_to(m_mac[3]);
        save_state->deserialize_to(m_iorgb);
        save_state->deserialize_to(m_lzcs);
        save_state->deserialize_to(m_lzcr);
        save_state->deserialize_to(m_rotation_matrix);
        save_state->deserialize_to(m_translation_vector);
        save_state->deserialize_to(m_light_source_matrix);
        save_state->deserialize_to(m_background_color);
        save_state->deserialize_to(m_light_color_matrix);
        save_state->deserialize_to(m_far_color);
        save_state->deserialize_to(m_screen_offset[0]);
        save_state->deserialize_to(m_screen_offset[1]);
        save_state->deserialize_to(m_projection_plane_distance);
        save_state->deserialize_to(m_depth_queing_parameter_coeff);
        save_state->deserialize_to(m_depth_queing_parameter_offset);
        save_state->deserialize_to(m_zsf3);
        save_state->deserialize_to(m_zsf4);
        save_state->deserialize_to(m_error_flags.raw);
        save_state->deserialize_to(m_current_instruction);
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
     * @brief multiply 3d vector a by 3d vector b and translate it
     */
    void GTE::multiply_and_translate(const GTEVector<s16>& a, const GTEVector<s16>& b, const GTEVector<s32>& translate)
    {
        check_and_assign_result((s64(translate.x) << 12) + a.x * b.x, 1, !m_current_instruction.lm);
        check_and_assign_result((s64(translate.y) << 12) + a.y * b.y, 2, !m_current_instruction.lm);
        check_and_assign_result((s64(translate.z) << 12) + a.z * b.z, 3, !m_current_instruction.lm);
    }

    /**
     * @brief multiply 3x3 matrix a by 3d vector b and translate it
     */
    void GTE::multiply_and_translate(const GTEMatrix<s16>& a, const GTEVector<s16>& b, const GTEVector<s32>& translate)
    {
        GTEVector<s64> result = multiply_and_translate_impl(a, b, translate);

        check_and_assign_result(result.x, 1, !m_current_instruction.lm);
        check_and_assign_result(result.y, 2, !m_current_instruction.lm);
        check_and_assign_result(result.z, 3, !m_current_instruction.lm);
    }

    /**
     * @brief multiply 3x3 matrix by 3d vector and translate it (RTP command specialized)
     */
    s64 GTE::multiply_and_translate_rtp(const GTEMatrix<s16>& a, const GTEVector<s16>& b, const GTEVector<s32>& translate)
    {
        GTEVector<s64> result = multiply_and_translate_impl(a, b, translate);

        check_and_assign_result(result.x, 1, !m_current_instruction.lm);
        check_and_assign_result(result.y, 2, !m_current_instruction.lm);
        check_and_assign_to_mac(result.z, 3);

        auto clipped_z = std::clamp(result.z >> 12, s64(-0x8000), s64(0x7FFF));

        m_error_flags.ir3_saturated = (result.z >> 12) != clipped_z;

        m_ir[3].raw() = std::clamp(s64(m_mac[3].raw()), m_current_instruction.lm ? s64(0) : s64(-0x8000), s64(0x7FFF));

        return result.z;
    }

    /**
     * @brief multiply 3x3 matrix a by 3d vector b and translate it
     */
    GTEVector<s64> GTE::multiply_and_translate_impl(const GTEMatrix<s16>& a, const GTEVector<s16>& b, const GTEVector<s32>& translate)
    {
        GTEVector<s64> result = GTEVector<s64> 
        { 
            .x = 0,
            .y = 0,
            .z = 0
        };

        result.x = 
        check_and_extend_from_mac
        (
            check_and_extend_from_mac
            (
                check_and_extend_from_mac
                (
                    (s64(translate.x) << 12) + a.at(0, 0) * b.x, 1
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
                    (s64(translate.y) << 12) + a.at(0, 1) * b.x, 2
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
                    (s64(translate.z) << 12) + a.at(0, 2) * b.x, 3
                ) + 
                a.at(1, 2) * b.y, 3
            ) + 
            a.at(2, 2) * b.z, 3
        );

        return result;
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
     * @brief push new color to the crgb queue 
     */
    void GTE::push_to_crgb_fifo(const GTEVector<s32>& color)
    {
        // move values in queue
        m_crgb[0].raw() = m_crgb[1].raw();
        m_crgb[1].raw() = m_crgb[2].raw();

        s32 red   = std::clamp(color.r, 0x00, 0xFF);
        s32 green = std::clamp(color.g, 0x00, 0xFF);
        s32 blue  = std::clamp(color.b, 0x00, 0xFF);

        // update error flags
        m_error_flags.color_r_saturated = color.r != red;
        m_error_flags.color_g_saturated = color.g != green;
        m_error_flags.color_b_saturated = color.b != blue;

        // update new value
        m_crgb[2].write(0, red);
        m_crgb[2].write(1, green);
        m_crgb[2].write(2, blue);
        m_crgb[2].write(3, m_rgbc.read(3));
    }

    /**
     * @brief push new z coord to the screen queue 
     */
    void GTE::push_to_screen_z_fifo(s32 z)
    {
        m_sz[0].raw() = m_sz[1].raw();
        m_sz[1].raw() = m_sz[2].raw();
        m_sz[2].raw() = m_sz[3].raw();
        m_sz[3].raw() = u16(std::clamp<s32>(z, 0x0000, 0xFFFF));

        m_error_flags.sz3_or_otz_saturated = m_sz[3].raw() != z;
    }

    /**
     * @brief push new z coord to the screen queue 
     */
    void GTE::push_to_screen_xy_fifo(s32 x, s32 y)
    {
        m_sxy[0].x = m_sxy[1].x;
        m_sxy[0].y = m_sxy[1].y;
        m_sxy[1].x = m_sxy[2].x;
        m_sxy[1].y = m_sxy[2].y;
        m_sxy[2].x = u16(std::clamp(x, -0x0400, 0x03FF));
        m_sxy[2].y = u16(std::clamp(y, -0x0400, 0x03FF));

        m_error_flags.sx2_saturated = m_sxy[2].x != x;
        m_error_flags.sy2_saturated = m_sxy[2].y != y;
    }

    /**
     * @brief general implementation for the RTPS/RTPT commands 
     */
    void GTE::rtps_impl(u32 general_purpose_vector_index, bool set_mac0)
    {
        s64 result = 0;
        switch(general_purpose_vector_index)
        {
            case 0: { result = multiply_and_translate_rtp(m_rotation_matrix, m_vxyz0, m_translation_vector); } break;
            case 1: { result = multiply_and_translate_rtp(m_rotation_matrix, m_vxyz1, m_translation_vector); } break;
            case 2: { result = multiply_and_translate_rtp(m_rotation_matrix, m_vxyz2, m_translation_vector); } break;
            default: { UNREACHABLE(); } break;
        }

        push_to_screen_z_fifo(s32(result >> 12));

        result = unr_division(m_projection_plane_distance.raw(), m_sz[3].raw());

        s32 x = check_and_assign_to_mac(result * m_ir[1].raw() + m_screen_offset[0].raw(), 0) >> 16;
        s32 y = check_and_assign_to_mac(result * m_ir[2].raw() + m_screen_offset[1].raw(), 0) >> 16;
        
        push_to_screen_xy_fifo(x, y);

        if(set_mac0)
        {
            s64 mac = check_and_assign_to_mac(result * m_depth_queing_parameter_coeff.raw() + m_depth_queing_parameter_offset.raw(), 0);
            m_ir[0].raw() = std::clamp(mac >> 12, s64(0x0000), s64(0x1000));
            m_error_flags.ir0_saturated = (mac >> 12) != m_ir[0].raw();
        }
    }

    /**
     * @brief general implementation for the NCDS/NCDT commands 
     */
    void GTE::ncds_impl(u32 general_purpose_vector_index)
    {
        switch(general_purpose_vector_index)
        {
            case 0: { multiply_and_translate(m_light_source_matrix, m_vxyz0, GTEVector<s32> { .x = 0, .y = 0, .z = 0 }); } break;
            case 1: { multiply_and_translate(m_light_source_matrix, m_vxyz1, GTEVector<s32> { .x = 0, .y = 0, .z = 0 }); } break;
            case 2: { multiply_and_translate(m_light_source_matrix, m_vxyz2, GTEVector<s32> { .x = 0, .y = 0, .z = 0 }); } break;
            default: { UNREACHABLE(); } break;
        }

        multiply_and_translate
        (
            m_light_color_matrix, 
            GTEVector<s16>
            {
                .x = m_ir[1].raw(),
                .y = m_ir[2].raw(),
                .z = m_ir[3].raw()
            },
            m_background_color
        );

        auto new_ir = GTEVector<s16>
        {
            .x = m_ir[1].raw(),
            .y = m_ir[2].raw(),
            .z = m_ir[3].raw()
        };

        check_and_assign_result((s64(m_far_color.r) << 12) - ((m_rgbc.read(0) << 4) * m_ir[1].raw()), 1, true);
        check_and_assign_result((s64(m_far_color.g) << 12) - ((m_rgbc.read(1) << 4) * m_ir[2].raw()), 2, true);
        check_and_assign_result((s64(m_far_color.b) << 12) - ((m_rgbc.read(2) << 4) * m_ir[3].raw()), 3, true);
    
        check_and_assign_result(((m_rgbc.read(0) << 4) * new_ir.x) + m_ir[0].raw() * m_ir[1].raw(), 1, !m_current_instruction.lm);
        check_and_assign_result(((m_rgbc.read(1) << 4) * new_ir.y) + m_ir[0].raw() * m_ir[2].raw(), 2, !m_current_instruction.lm);
        check_and_assign_result(((m_rgbc.read(2) << 4) * new_ir.z) + m_ir[0].raw() * m_ir[3].raw(), 3, !m_current_instruction.lm);

        push_to_crgb_fifo(GTEVector<s32>
        {
            .r = m_mac[1].raw() >> 4,
            .g = m_mac[2].raw() >> 4,
            .b = m_mac[3].raw() >> 4
        });
    }

    /**
     * @brief general implementation for the NCCS/NCCT commands 
     */
    void GTE::nccs_impl(u32 general_purpose_vector_index)
    {
        switch(general_purpose_vector_index)
        {
            case 0: { multiply_and_translate(m_light_source_matrix, m_vxyz0, GTEVector<s32> { .x = 0, .y = 0, .z = 0 }); } break;
            case 1: { multiply_and_translate(m_light_source_matrix, m_vxyz1, GTEVector<s32> { .x = 0, .y = 0, .z = 0 }); } break;
            case 2: { multiply_and_translate(m_light_source_matrix, m_vxyz2, GTEVector<s32> { .x = 0, .y = 0, .z = 0 }); } break;
            default: { UNREACHABLE(); } break;
        }

        multiply_and_translate
        (
            m_light_color_matrix, 
            GTEVector<s16>
            {
                .x = m_ir[1].raw(),
                .y = m_ir[2].raw(),
                .z = m_ir[3].raw()
            },
            m_background_color
        );
        
        multiply_and_translate
        (
            GTEVector<s16>
            {
                .x = s16(m_rgbc.read(0) << 4),
                .y = s16(m_rgbc.read(1) << 4),
                .z = s16(m_rgbc.read(2) << 4)
            },
            GTEVector<s16>
            {
                .x = m_ir[1].raw(),
                .y = m_ir[2].raw(),
                .z = m_ir[3].raw()
            },
            GTEVector<s32>
            {
                .x = 0,
                .y = 0,
                .z = 0
            }
        );

        push_to_crgb_fifo(GTEVector<s32>
        {
            .r = m_mac[1].raw() >> 4,
            .g = m_mac[2].raw() >> 4,
            .b = m_mac[3].raw() >> 4
        });
    }

    /**
     * @brief general implementation for the DPCS/DPCT commands 
     */
    void GTE::dpcs_impl(bool rgb_register_selector)
    {
        GTEVector<s16> color;
        switch(rgb_register_selector)
        {
            case 1: 
            {  
                color = GTEVector<s16>
                {
                    .r = s16(m_crgb[0].read(0) << 4),
                    .g = s16(m_crgb[0].read(1) << 4),
                    .b = s16(m_crgb[0].read(2) << 4)
                };
            } break;
            case 0:
            {
                color = GTEVector<s16>
                {
                    .r = s16(m_rgbc.read(0) << 4),
                    .g = s16(m_rgbc.read(1) << 4),
                    .b = s16(m_rgbc.read(2) << 4)  
                };
            } break;
        }

        check_and_assign_result((s64(m_far_color.r) << 12) - (color.r << 12), 1, true);
        check_and_assign_result((s64(m_far_color.g) << 12) - (color.g << 12), 2, true);
        check_and_assign_result((s64(m_far_color.b) << 12) - (color.b << 12), 3, true);

        multiply_and_translate
        (
            GTEVector<s16>
            {
                .r = m_ir[0].raw(),
                .g = m_ir[0].raw(),
                .b = m_ir[0].raw()
            },
            GTEVector<s16>
            {
                .r = m_ir[1].raw(),
                .g = m_ir[2].raw(),
                .b = m_ir[3].raw()
            },
            GTEVector<s32>
            {
                .r = color.r,
                .g = color.g,
                .b = color.b
            }
        );

        push_to_crgb_fifo(GTEVector<s32>
        {
            .r = m_mac[1].raw() >> 4,
            .g = m_mac[2].raw() >> 4,
            .b = m_mac[3].raw() >> 4
        });
    }

    /**
     * @brief divide two values using the UNR division algorithm
     */
    u32 GTE::unr_division(u32 numerator, u32 denominator)
    {        
#if 1
        if(denominator == 0)
            return 0x0001'FFFF;

        u32 result = ((numerator * 0x0001'0000 + denominator / 2) / denominator);

        return std::min(result, u32(0x0001'FFFF));
#else
        static constexpr const std::array<u8, 0x101> UNRTable =
        {
            0xFF, 0xFD, 0xFB, 0xF9, 0xF7, 0xF5, 0xF3, 0xF1, 0xEF, 0xEE, 0xEC, 0xEA, 0xE8, 0xE6, 0xE4, 0xE3,
            0xE1, 0xDF, 0xDD, 0xDC, 0xDA, 0xD8, 0xD6, 0xD5, 0xD3, 0xD1, 0xD0, 0xCE, 0xCD, 0xCB, 0xC9, 0xC8, 
            0xC6, 0xC5, 0xC3, 0xC1, 0xC0, 0xBE, 0xBD, 0xBB, 0xBA, 0xB8, 0xB7, 0xB5, 0xB4, 0xB2, 0xB1, 0xB0,
            0xAE, 0xAD, 0xAB, 0xAA, 0xA9, 0xA7, 0xA6, 0xA4, 0xA3, 0xA2, 0xA0, 0x9F, 0x9E, 0x9C, 0x9B, 0x9A,
            0x99, 0x97, 0x96, 0x95, 0x94, 0x92, 0x91, 0x90, 0x8F, 0x8D, 0x8C, 0x8B, 0x8A, 0x89, 0x87, 0x86,
            0x85, 0x84, 0x83, 0x82, 0x81, 0x7F, 0x7E, 0x7D, 0x7C, 0x7B, 0x7A, 0x79, 0x78, 0x77, 0x75, 0x74, 
            0x73, 0x72, 0x71, 0x70, 0x6F, 0x6E, 0x6D, 0x6C, 0x6B, 0x6A, 0x69, 0x68, 0x67, 0x66, 0x65, 0x64,
            0x63, 0x62, 0x61, 0x60, 0x5F, 0x5E, 0x5D, 0x5D, 0x5C, 0x5B, 0x5A, 0x59, 0x58, 0x57, 0x56, 0x55,
            0x54, 0x53, 0x53, 0x52, 0x51, 0x50, 0x4F, 0x4E, 0x4D, 0x4D, 0x4C, 0x4B, 0x4A, 0x49, 0x48, 0x48,
            0x47, 0x46, 0x45, 0x44, 0x43, 0x43, 0x42, 0x41, 0x40, 0x3F, 0x3F, 0x3E, 0x3D, 0x3C, 0x3C, 0x3B, 
            0x3A, 0x39, 0x39, 0x38, 0x37, 0x36, 0x36, 0x35, 0x34, 0x33, 0x33, 0x32, 0x31, 0x31, 0x30, 0x2F,
            0x2E, 0x2E, 0x2D, 0x2C, 0x2C, 0x2B, 0x2A, 0x2A, 0x29, 0x28, 0x28, 0x27, 0x26, 0x26, 0x25, 0x24,
            0x24, 0x23, 0x22, 0x22, 0x21, 0x20, 0x20, 0x1F, 0x1E, 0x1E, 0x1D, 0x1D, 0x1C, 0x1B, 0x1B, 0x1A,
            0x19, 0x19, 0x18, 0x18, 0x17, 0x16, 0x16, 0x15, 0x15, 0x14, 0x14, 0x13, 0x12, 0x12, 0x11, 0x11, 
            0x10, 0x0F, 0x0F, 0x0E, 0x0E, 0x0D, 0x0D, 0x0C, 0x0C, 0x0B, 0x0A, 0x0A, 0x09, 0x09, 0x08, 0x08,
            0x07, 0x07, 0x06, 0x06, 0x05, 0x05, 0x04, 0x04, 0x03, 0x03, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00,
            0x00
        };

        if(numerator < denominator * 2)
        {
            auto zeros = std::countl_zero(u16(denominator));
            u32 n      = numerator   << zeros;
            u32 d      = denominator << zeros;
            u32 u      = 0x101 + UNRTable.at((d - 0x7FC0) >> 7);
            d          = ((0x0200'0080 - (d * u)) >> 8);
            d          = ((0x0000'0080 + (d * u)) >> 8);
            return std::min(u32(0x0001'FFFF), (((n * d) + 0x8000) >> 16));
        }
        else
        {
            m_error_flags.divide_overflow = 1;
            return 0x0001'FFFF;
        }
#endif
    }

    /**
     * @brief 
     */
    void GTE::UNK(const GTEInstruction&)
    {
        TODO();
    }

    /**
     * @brief perspective transformation single
     */
    void GTE::RTPS(const GTEInstruction&)
    {
        rtps_impl(0, true);
    } 

    /**
     * @brief normal clipping
     */
    void GTE::NCLIP(const GTEInstruction&)
    {
        auto result = s64(m_sxy[0].x) * s64(m_sxy[1].y) + 
                      s64(m_sxy[1].x) * s64(m_sxy[2].y) +
                      s64(m_sxy[2].x) * s64(m_sxy[0].y) - 
                      s64(m_sxy[0].x) * s64(m_sxy[2].y) -
                      s64(m_sxy[1].x) * s64(m_sxy[0].y) - 
                      s64(m_sxy[2].x) * s64(m_sxy[1].y);
        
        MARK_UNUSED(check_and_assign_to_mac(result, 0));
    }

    /**
     * @brief outer product of 2 vectors
     */
    void GTE::OP(const GTEInstruction& ins)
    {
        MARK_UNUSED(check_and_assign_to_mac(m_rotation_matrix.at(1, 1) * m_ir[3].raw() - m_rotation_matrix.at(2, 2) * m_ir[2].raw(), 1));
        MARK_UNUSED(check_and_assign_to_mac(m_rotation_matrix.at(2, 2) * m_ir[1].raw() - m_rotation_matrix.at(0, 0) * m_ir[3].raw(), 2));
        MARK_UNUSED(check_and_assign_to_mac(m_rotation_matrix.at(0, 0) * m_ir[2].raw() - m_rotation_matrix.at(1, 1) * m_ir[1].raw(), 3));
        check_and_assign_to_ir(m_mac[1].raw(), 1, !ins.lm);
        check_and_assign_to_ir(m_mac[2].raw(), 2, !ins.lm);
        check_and_assign_to_ir(m_mac[3].raw(), 3, !ins.lm);
    } 

    /**
     *  @brief depth cueing single
     */
    void GTE::DPCS(const GTEInstruction&)
    {
        dpcs_impl(0);
    }  

    /**
     * @brief interpolation of a vector and far color vector
     */
    void GTE::INTPL(const GTEInstruction&)
    {
        auto current_ir = GTEVector<s32>
        {
            .x = m_ir[1].raw(),
            .y = m_ir[2].raw(),
            .z = m_ir[3].raw()
        };

        check_and_assign_result(((s64(m_far_color.r << 12) - s64(current_ir.r << 12))), 1, true);
        check_and_assign_result(((s64(m_far_color.g << 12) - s64(current_ir.g << 12))), 2, true);
        check_and_assign_result(((s64(m_far_color.b << 12) - s64(current_ir.b << 12))), 3, true);
        multiply_and_translate
        (
            GTEVector<s16>
            {
                .x = m_ir[0].raw(),
                .y = m_ir[0].raw(),
                .z = m_ir[0].raw()
            },
            GTEVector<s16>
            {
                .x = m_ir[1].raw(),
                .y = m_ir[2].raw(),
                .z = m_ir[3].raw()
            },
            current_ir
        );

        push_to_crgb_fifo(GTEVector<s32>
        {
            .r = m_mac[1].raw() >> 4,
            .g = m_mac[2].raw() >> 4,
            .b = m_mac[3].raw() >> 4
        });
    } 

    /**
     * @brief multiply vector by matrix and add vector
     */
    void GTE::MVMVA(const GTEInstruction& ins)
    {
        GTEMatrix<s16> matrix_mult_operand;
        switch(ins.mul_matrix)
        {
            case 0: 
            {
                matrix_mult_operand = m_rotation_matrix;
            } break;
            case 1:
            {
                matrix_mult_operand = m_light_source_matrix;
            } break;
            case 2:
            {
                matrix_mult_operand = m_light_color_matrix;
            } break;
            case 3:
            {
                TODO();
            } break;
            default:
            {
                UNREACHABLE();
            } break;
        }

        GTEVector<s16> vector_mult_operand = GTEVector<s16>
        {
            .x = 0,
            .y = 0,
            .z = 0
        };
        switch(ins.mul_vector)
        {
            case 0:
            {
                vector_mult_operand = m_vxyz0;
            } break;
            case 1:
            {
                vector_mult_operand = m_vxyz1;
            } break;
            case 2:
            {
                vector_mult_operand = m_vxyz2;
            } break;
            case 3:
            {
                vector_mult_operand = GTEVector<s16>
                {
                    .x = m_ir[1].raw(), 
                    .y = m_ir[2].raw(), 
                    .z = m_ir[3].raw()
                };
            } break;
            default:
            {
                UNREACHABLE();
            } break;
        }

        GTEVector<s32> vector_add_operand = GTEVector<s32>
        {
            .x = 0,
            .y = 0,
            .z = 0
        };
        switch(ins.trans_vector)
        {
            case 0:
            {
                vector_add_operand = m_translation_vector;
            } break;
            case 1:
            {
                vector_add_operand = m_background_color;
            } break;
            case 2:
            {
                TODO();
            } break;
            case 3:
            {
                // None == (.x = 0, .y = 0, .z = 0) ???
            } break;
            default:
            {
                UNREACHABLE();
            } break;
        }

        multiply_and_translate(matrix_mult_operand, vector_mult_operand, vector_add_operand);
    } 

    /**
     * @brief normal color depth cue single vector
     */
    void GTE::NCDS(const GTEInstruction&)
    {
        ncds_impl(0);
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
     * @brief normal color color single vector
     */
    void GTE::NCCS(const GTEInstruction&)
    {
        nccs_impl(0);
    }  

    /**
     * @brief color color
     */
    void GTE::CC(const GTEInstruction&)
    {
        multiply_and_translate
        (
            m_light_color_matrix,
            GTEVector<s16>
            {
                .x = m_ir[1].raw(),
                .y = m_ir[2].raw(),
                .z = m_ir[3].raw()
            },
            m_background_color
        );
        multiply_and_translate
        (
            GTEVector<s16>
            {
                .x = s16(m_rgbc.read(0) << 4),
                .y = s16(m_rgbc.read(1) << 4),
                .z = s16(m_rgbc.read(2) << 4)
            },
            GTEVector<s16>
            {
                .x = m_ir[1].raw(),
                .y = m_ir[2].raw(),
                .z = m_ir[3].raw()
            },
            GTEVector<s32>
            {
                .x = 0,
                .y = 0,
                .z = 0
            }
        );

        push_to_crgb_fifo(GTEVector<s32>
        {
            .r = m_mac[1].raw() >> 4,
            .g = m_mac[2].raw() >> 4,
            .b = m_mac[3].raw() >> 4
        });
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
     * @brief square of vector IR
     */
    void GTE::SQR(const GTEInstruction&)
    {
        auto ir_vector = GTEVector<s16>
        {
            .x = m_ir[1].raw(),
            .y = m_ir[2].raw(),
            .z = m_ir[3].raw()
        };
        multiply_and_translate(ir_vector, ir_vector, GTEVector<s32> { .x = 0, .y = 0, .z = 0 });
    }   

    /**
     * @brief depth cue color light
     */
    void GTE::DCPL(const GTEInstruction& ins)
    {
        auto current_ir = GTEVector<s16>
        {
            .x = m_ir[1].raw(),
            .y = m_ir[2].raw(),
            .z = m_ir[3].raw()
        };

        check_and_assign_result((s64(m_far_color.r << 12) - s64((m_rgbc.read(0) << 4) * current_ir.r)), 1, true);
        check_and_assign_result((s64(m_far_color.g << 12) - s64((m_rgbc.read(1) << 4) * current_ir.g)), 2, true);
        check_and_assign_result((s64(m_far_color.b << 12) - s64((m_rgbc.read(2) << 4) * current_ir.b)), 3, true);
        check_and_assign_result((m_rgbc.read(0) << 4) * current_ir.r + m_ir[0].raw() * m_ir[1].raw(), 1, !ins.lm);
        check_and_assign_result((m_rgbc.read(1) << 4) * current_ir.g + m_ir[0].raw() * m_ir[2].raw(), 2, !ins.lm);
        check_and_assign_result((m_rgbc.read(2) << 4) * current_ir.b + m_ir[0].raw() * m_ir[3].raw(), 3, !ins.lm);

        push_to_crgb_fifo(GTEVector<s32>
        {
            .r = m_mac[1].raw() >> 4,
            .g = m_mac[2].raw() >> 4,
            .b = m_mac[3].raw() >> 4
        });
    }  

    /**
     * @brief depth cueing triple
     */
    void GTE::DPCT(const GTEInstruction&)
    {
        dpcs_impl(1);
        dpcs_impl(1);
        dpcs_impl(1);
    }  

    /**
     * @brief average of three z values
     */
    void GTE::AVSZ3(const GTEInstruction&)
    {
        auto result = check_and_assign_to_mac(s64(m_zsf3.raw()) * (m_sz[1].raw() + m_sz[2].raw() + m_sz[3].raw()), 0);
        m_otz.raw() = std::clamp(result >> 12, s64(0x0000), s64(0xFFFF));
        m_error_flags.sz3_or_otz_saturated = m_otz.raw() != (result >> 12);
    } 

    /**
     * @brief average of four z values
     */
    void GTE::AVSZ4(const GTEInstruction&)
    {
        auto result = check_and_assign_to_mac(s64(m_zsf4.raw()) * (m_sz[0].raw() + m_sz[1].raw() + m_sz[2].raw() + m_sz[3].raw()), 0);
        m_otz.raw() = std::clamp(result >> 12, s64(0x0000), s64(0xFFFF));
        m_error_flags.sz3_or_otz_saturated = m_otz.raw() != (result >> 12);
    } 

    /**
     * @brief perspective transformation triple
     */
    void GTE::RTPT(const GTEInstruction&)
    {
        rtps_impl(0, false);
        rtps_impl(1, false);
        rtps_impl(2, true);
    }  

    /**
     * @brief general purpose interpolation
     */
    void GTE::GPF(const GTEInstruction&)
    {
        multiply_and_translate
        (
            GTEVector<s16>
            {
                .x = m_ir[0].raw(),
                .y = m_ir[0].raw(),
                .z = m_ir[0].raw()
            },
            GTEVector<s16>
            {
                .x = m_ir[1].raw(),
                .y = m_ir[2].raw(),
                .z = m_ir[3].raw()
            },
            GTEVector<s32> 
            {
                .x = 0, 
                .y = 0, 
                .z = 0
            }
        );

        push_to_crgb_fifo(GTEVector<s32>
        {
            .r = m_mac[1].raw() >> 4,
            .g = m_mac[2].raw() >> 4,
            .b = m_mac[3].raw() >> 4
        });
    }   

    /**
     * @brief general purpose interpolation with base
     */
    void GTE::GPL(const GTEInstruction& ins)
    {
        check_and_assign_result((s64(m_mac[1].raw()) << (ins.shift_fraction * 12)) + m_ir[0].raw() * m_ir[1].raw(), 1, !ins.lm);
        check_and_assign_result((s64(m_mac[2].raw()) << (ins.shift_fraction * 12)) + m_ir[0].raw() * m_ir[2].raw(), 2, !ins.lm);
        check_and_assign_result((s64(m_mac[3].raw()) << (ins.shift_fraction * 12)) + m_ir[0].raw() * m_ir[3].raw(), 3, !ins.lm);

        push_to_crgb_fifo(GTEVector<s32>
        {
            .r = m_mac[1].raw() >> 4,
            .g = m_mac[2].raw() >> 4,
            .b = m_mac[3].raw() >> 4
        });
    }   

    /**
     * @brief 
     */
    void GTE::NCCT(const GTEInstruction&)
    {
        TODO();
    }  
}