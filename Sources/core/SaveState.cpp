/**
 * @file      SaveState.cpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Interface for saving state of the emulator
 *
 * @version   0.1
 *
 * @date      15. 03. 2024, 14:50 (created)
 *
 * @section   TODO: replace with actual documentation
 * TODO: documentation text
 *
 * @section License
 * This file is part of the TODO: project \n
 *
 * Copyright (C) 2024
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

#include "SaveState.hpp"

#include "Utils.hpp"

#include <fstream>

namespace PSX
{
    std::shared_ptr<SaveState> SaveState::create()
    {
        return std::shared_ptr<SaveState>(new SaveState());
    }

    /**
     * @brief serialize common types 
     */
    void SaveState::serialize_from(bool value)
    {
        serialize_from(static_cast<u8>(value));
    }
    void SaveState::serialize_from(u8 value)
    {
        m_serialized_state.push_back(value);
    }
    void SaveState::serialize_from(s8 value)
    {
        serialize_from(static_cast<u8>(value));
    }
    void SaveState::serialize_from(u16 value)
    {
        serialize_from(static_cast<u8>((value >> 0) & 0xFF));
        serialize_from(static_cast<u8>((value >> 8) & 0xFF));
    }
    void SaveState::serialize_from(s16 value)
    {
        serialize_from(static_cast<u16>(value));
    }
    void SaveState::serialize_from(u32 value)
    {
        serialize_from(static_cast<u16>((value >>  0) & 0xFFFF));
        serialize_from(static_cast<u16>((value >> 16) & 0xFFFF));
    }
    void SaveState::serialize_from(s32 value)
    {
        serialize_from(static_cast<u32>(value));
    }
    void SaveState::serialize_from(u64 value)
    {
        serialize_from(static_cast<u32>((value >>  0) & 0xFFFF'FFFF));
        serialize_from(static_cast<u32>((value >> 32) & 0xFFFF'FFFF));
    }
    void SaveState::serialize_from(s64 value)
    {
        serialize_from(static_cast<u64>(value));
    }
    void SaveState::serialize_from(float value)
    {
        serialize_from(bit_cast<u32>(value));
    }
    void SaveState::serialize_from(const std::string& string)
    {
        serialize_from(u32(string.size()));
        for(u8 value: string)
        {
            serialize_from(value);
        }
    }

    /**
     * @brief deserialize common types 
     */
    void SaveState::deserialize_to(bool& value)
    {
        deserialize_to(reinterpret_cast<u8&>(value));
    }
    void SaveState::deserialize_to(u8& value)
    {
        value = m_serialized_state[m_serialized_state_cursor++];
    }
    void SaveState::deserialize_to(s8& value)
    {
        deserialize_to(reinterpret_cast<u8&>(value));
    }
    void SaveState::deserialize_to(u16& value)
    {
        u16 _1 = u16(m_serialized_state[m_serialized_state_cursor++]) << 0;
        u16 _2 = u16(m_serialized_state[m_serialized_state_cursor++]) << 8;
        value = _1 | _2;
    }
    void SaveState::deserialize_to(s16& value)
    {
        deserialize_to(reinterpret_cast<u16&>(value));
    }
    void SaveState::deserialize_to(u32& value)
    {
        u32 _1 = u32(m_serialized_state[m_serialized_state_cursor++]) <<  0;
        u32 _2 = u32(m_serialized_state[m_serialized_state_cursor++]) <<  8;
        u32 _3 = u32(m_serialized_state[m_serialized_state_cursor++]) << 16;
        u32 _4 = u32(m_serialized_state[m_serialized_state_cursor++]) << 24;
        value = _1 | _2 | _3 | _4;
    }
    void SaveState::deserialize_to(s32& value)
    {
        deserialize_to(reinterpret_cast<u32&>(value));
    }
    void SaveState::deserialize_to(u64& value)
    {
        u64 _1 = u64(m_serialized_state[m_serialized_state_cursor++]) <<  0;
        u64 _2 = u64(m_serialized_state[m_serialized_state_cursor++]) <<  8;
        u64 _3 = u64(m_serialized_state[m_serialized_state_cursor++]) << 16;
        u64 _4 = u64(m_serialized_state[m_serialized_state_cursor++]) << 24;
        u64 _5 = u64(m_serialized_state[m_serialized_state_cursor++]) << 32;
        u64 _6 = u64(m_serialized_state[m_serialized_state_cursor++]) << 40;
        u64 _7 = u64(m_serialized_state[m_serialized_state_cursor++]) << 48;
        u64 _8 = u64(m_serialized_state[m_serialized_state_cursor++]) << 56;
        value = _1 | _2 | _3 | _4 | _5 | _6 | _7 | _8;
    }
    void SaveState::deserialize_to(s64& value)
    {
        deserialize_to(reinterpret_cast<u64&>(value));
    }
    void SaveState::deserialize_to(float& value)
    {
        deserialize_to(reinterpret_cast<u32&>(value));
    }
    void SaveState::deserialize_to(std::string& value)
    {
        u32 string_size = 0;
        deserialize_to(string_size);

        value.resize(string_size);

        for(u32 i = 0; i < string_size; i++)
        {
            deserialize_to(reinterpret_cast<u8&>(value[i]));
        }
    }

    /**
     * @brief serialize cdrom specific types 
     */
    void SaveState::serialize_from(Sector value)
    {
        serialize_from(value.data);
        serialize_from(static_cast<u8>(value.type));
    }
    void SaveState::serialize_from(ADPCMFilter value)
    {
        serialize_from(value.file);
        serialize_from(value.channel);
    }
    void SaveState::serialize_from(SubChannelQ value)
    {
        serialize_from(value.track_number);
        serialize_from(value.index_number);
        serialize_from(value.relative_position_mm);
        serialize_from(value.relative_position_ss);
        serialize_from(value.relative_position_ff);
        serialize_from(value.reserved);
        serialize_from(value.absolute_position_mm);
        serialize_from(value.absolute_position_ss);
        serialize_from(value.absolute_position_ff);
    }
    void SaveState::serialize_from(ConsoleRegion value)
    {
        serialize_from(static_cast<u8>(value));
    }
    void SaveState::serialize_from(Track value)
    {
        serialize_from(value.id_number);
        serialize_from(static_cast<u8>(value.type));
        serialize_from(value.data_position);
        serialize_from(value.offset);
        serialize_from(value.num_sectors);
        serialize_from(value.meta_file_path);
    }
    void SaveState::serialize_from(Position value)
    {
        serialize_from(value.minutes);
        serialize_from(value.seconds);
        serialize_from(value.fractions);
    }

    /**
     * @brief deserialize cdrom specific types 
     */
    void SaveState::deserialize_to(Sector& value)
    {
        deserialize_to(value.data);
        deserialize_to(reinterpret_cast<u8&>(value.type));
    }
    void SaveState::deserialize_to(ADPCMFilter& value)
    {
        deserialize_to(value.file);
        deserialize_to(value.channel);
    }
    void SaveState::deserialize_to(SubChannelQ& value)
    {
        deserialize_to(value.track_number);
        deserialize_to(value.index_number);
        deserialize_to(value.relative_position_mm);
        deserialize_to(value.relative_position_ss);
        deserialize_to(value.relative_position_ff);
        deserialize_to(value.reserved);
        deserialize_to(value.absolute_position_mm);
        deserialize_to(value.absolute_position_ss);
        deserialize_to(value.absolute_position_ff);
    }
    void SaveState::deserialize_to(ConsoleRegion& value)
    {
        deserialize_to(reinterpret_cast<u8&>(value));
    }
    void SaveState::deserialize_to(Track& value)
    {
        deserialize_to(value.id_number);
        deserialize_to(reinterpret_cast<u8&>(value.type));
        deserialize_to(value.data_position);
        deserialize_to(value.offset);
        deserialize_to(value.num_sectors);
        deserialize_to(value.meta_file_path);
        if(value.meta_file)
        {
            value.meta_file->close();
        }
        value.meta_file = std::make_shared<std::ifstream>(value.meta_file_path, std::ios::binary);
        if(!value.meta_file->is_open())
        {
            ABORT_WITH_MESSAGE(fmt::format("failed to load disc from path {}", value.meta_file_path));
        }
    }
    void SaveState::deserialize_to(Position& value)
    {
        deserialize_to(value.minutes);
        deserialize_to(value.seconds);
        deserialize_to(value.fractions);
    }

    /**
     * @brief serialize cpu specific types 
     */
    void SaveState::serialize_from(CPUInstruction value)
    {
        serialize_from(value.raw);
    }
    void SaveState::serialize_from(LoadDelaySlot value)
    {
        serialize_from(value.register_id);
        serialize_from(value.value);
    }
    void SaveState::serialize_from(ExecutedInstruction value)
    {
        serialize_from(value.address);
        serialize_from(value.ins);
    }

    /**
     * @brief serialize cpu specific types 
     */
    void SaveState::deserialize_to(CPUInstruction& value)
    {
        deserialize_to(value.raw);
    }
    void SaveState::deserialize_to(LoadDelaySlot& value)
    {
        deserialize_to(value.register_id);
        deserialize_to(value.value);
    }
    void SaveState::deserialize_to(ExecutedInstruction& value)
    {
        deserialize_to(value.address);
        deserialize_to(value.ins);
    }

    /**
     * @brief serialize gpu specific types 
     */
    void SaveState::serialize_from(GPUCommand value)
    {
        serialize_from(static_cast<u8>(value));
    }

    /**
     * @brief de-serialize gpu specific types 
     */
    void SaveState::deserialize_to(GPUCommand& value)
    {
        deserialize_to(reinterpret_cast<u8&>(value));
    }

    /**
     * @brief serialize gte specific types 
     */
    void SaveState::serialize_from(GTEInstruction value)
    {
        serialize_from(value.raw);
    }

    /**
     * @brief de-serialize gte specific types 
     */
    void SaveState::deserialize_to(GTEInstruction& value)
    {
        deserialize_to(value.raw);
    }

    /**
     * @brief serialize mdec specific types 
     */
    void SaveState::serialize_from(MDECInstruction value)
    {
        serialize_from(value.raw);
    }
    
    /**
     * @brief serialize mdec specific types 
     */
    void SaveState::deserialize_to(MDECInstruction& value)
    {
        deserialize_to(value.raw);
    }

    /**
     * @brief serialize peripherals specific types 
     */
    void SaveState::serialize_from(CurrentlyCommunicatingWith value)
    {
        serialize_from(static_cast<u8>(value));
    }

    /**
     * @brief de-serialize peripherals specific types 
     */
    void SaveState::deserialize_to(CurrentlyCommunicatingWith& value)
    {
        deserialize_to(reinterpret_cast<u8&>(value));
    }

    /**
     * @brief save accumulated serialized data to a file
     */
    void SaveState::write_to_file(const std::string& file_path)
    {
        LOG_DEBUG(1, fmt::format("saving state to {}", file_path));

        // open state file
        std::ofstream state_file(file_path, std::ios::binary);

        if(!state_file.is_open())
        {
            LOG_ERROR(fmt::format("state file {} could not be opened", file_path));
            return;
        }

        // write file contents
        std::copy(m_serialized_state.begin(), m_serialized_state.end(), std::ostreambuf_iterator<char>(state_file));

        state_file.close();

        LOG_DEBUG(1, "state saved");
    }

    /**
     * @brief load serialized data from a file
     */
    void SaveState::read_from_file(const std::string& file_path)
    {
        LOG_DEBUG(1, fmt::format("loading state from {}", file_path));

        // open state file
        std::ifstream state_file(file_path, std::ios::binary);

        if(!state_file.is_open())
        {
            LOG_ERROR(fmt::format("state file {} could not be opened", file_path));
            return;
        }

        // read file contents
        std::vector<u8> state_file_contents((std::istreambuf_iterator<char>(state_file)),
                                             std::istreambuf_iterator<char>());

        m_serialized_state = std::move(state_file_contents);

        m_serialized_state_cursor = 0;

        state_file.close();

        LOG_DEBUG(1, "state loaded");
    }
}