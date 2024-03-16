/**
 * @file      SaveState.hpp
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

#ifndef SAVESTATE_HPP
#define SAVESTATE_HPP

#include <set>
#include <vector>
#include <array>
#include <memory>
#include <queue>
#include <optional>
#include "Types.hpp"
#include "Utils.hpp"
#include "CDROMTypes.hpp"
#include "DiscTypes.hpp"
#include "CPUInstruction.hpp"
#include "DMATypes.hpp"
#include "GPUTypes.hpp"
#include "GTETypes.hpp"
#include "GTEInstruction.hpp"
#include "MDECInstruction.hpp"
#include "TimerTypes.hpp"
#include "CPUTypes.hpp"
#include "MemoryRegion.hpp"
#include "Macros.hpp"
#include "PeripheralsTypes.hpp"

namespace PSX
{
    /**
     * @brief Interface for saving state of the emulator 
     */
    class SaveState
    {
    public:

        static std::shared_ptr<SaveState> create();

        /**
         * @brief serialize common types 
         */
        void serialize_from(bool);
        void serialize_from(u8);
        void serialize_from(s8);
        void serialize_from(u16);
        void serialize_from(s16);
        void serialize_from(u32);
        void serialize_from(s32);
        void serialize_from(u64);
        void serialize_from(s64);
        void serialize_from(float);
        void serialize_from(const std::string&);
        template<typename T>
        void serialize_from(Register<T> reg)
        {
            serialize_from(reg.raw());
        }
        template<typename T>
        void serialize_from(const std::vector<T>& vector)
        {
            serialize_from(u32(vector.size()));
            for(auto& value: vector)
            {
                serialize_from(value);
            }
        }
        template<u32 Size>
        void serialize_from(const MemoryRegion<Size>& memory_region)
        {
            for(u32 i = 0; i < Size; i++)
            {
                serialize_from(memory_region.template read<u8>(i));
            }
        }
        template<typename T, u32 Size>
        void serialize_from(const fixed_queue<T, Size>& queue)
        {
            serialize_from(queue.size());
            for(u32 i = 0; i < queue.size(); i++)
            {
                serialize_from(queue.at(i));
            }
        }
        template<typename T, std::size_t Size>
        void serialize_from(const std::array<T, Size>& array)
        {
            for(auto& value: array)
            {
                serialize_from(value);
            }
        }
        template<typename T>
        void serialize_from(const std::set<T>& set)
        {
            serialize_from(u32(set.size()));
            for(auto& v: set)
            {
                serialize_from(v);
            }
        }
        template<typename T>
        void serialize_from(const std::optional<T>& optional)
        {
            serialize_from(optional.has_value());
            if(optional.has_value())
            {
                serialize_from(optional.value());
            }
        }

        /**
         * @brief de-serialize common types 
         */
        void deserialize_to(bool&);
        void deserialize_to(u8&);
        void deserialize_to(s8&);
        void deserialize_to(u16&);
        void deserialize_to(s16&);
        void deserialize_to(u32&);
        void deserialize_to(s32&);
        void deserialize_to(u64&);
        void deserialize_to(s64&);
        void deserialize_to(float&);
        void deserialize_to(std::string&);
        template<typename T>
        void deserialize_to(Register<T>& reg)
        {
            deserialize_to(reg.raw());
        }
        template<typename T>
        void deserialize_to(std::vector<T>& vector)
        {
            u32 vector_size = 0;
            deserialize_to(vector_size);

            vector.resize(vector_size);
            for(u32 i = 0; i < vector_size; i++)
            {
                deserialize_to(reinterpret_cast<T&>(vector[i]));
            }
        }
        template<u32 Size>
        void deserialize_to(MemoryRegion<Size>& memory_region)
        {
            for(u32 i = 0; i < Size; i++)
            {
                u8 value = 0;
                deserialize_to(value);
                memory_region.template write<u8>(i, value);
            }
        }
        template<typename T, u32 Size>
        void deserialize_to(fixed_queue<T, Size>& queue)
        {
            queue.clear();

            u32 queue_size = 0;
            deserialize_to(queue_size);

            for(u32 i = 0; i < queue_size; i++)
            {
                T value;
                deserialize_to(value);
                queue.push(value);
            }
        }
        template<typename T, std::size_t Size>
        void deserialize_to(std::array<T, Size>& array)
        {
            for(auto& value: array)
            {
                deserialize_to(value);
            }
        }
        template<typename T>
        void deserialize_to(std::set<T>& set)
        {
            set.clear();

            u32 set_size = 0;
            deserialize_to(set_size);

            for(u32 i = 0; i < set_size; i++)
            {
                T value;
                deserialize_to(value);
                set.insert(value);
            }
        }
        template<typename T>
        void deserialize_to(std::optional<T>& optional)
        {
            optional.reset();

            bool has_value = false;
            deserialize_to(has_value);

            if(has_value)
            {
                T value;
                deserialize_to(value);
                optional = value;
            }
        }

        /**
         * @brief serialize cdrom specific types 
         */
        void serialize_from(Sector);
        void serialize_from(ADPCMFilter);
        void serialize_from(SubChannelQ);
        void serialize_from(ConsoleRegion);
        void serialize_from(Track);
        void serialize_from(Position);

        /**
         * @brief de-serialize cdrom specific types 
         */
        void deserialize_to(Sector&);
        void deserialize_to(ADPCMFilter&);
        void deserialize_to(SubChannelQ&);
        void deserialize_to(ConsoleRegion&);
        void deserialize_to(Track&);
        void deserialize_to(Position&);

        /**
         * @brief serialize cpu specific types 
         */
        void serialize_from(CPUInstruction);
        void serialize_from(LoadDelaySlot);
        void serialize_from(ExecutedInstruction);

        /**
         * @brief de-serialize cpu specific types 
         */
        void deserialize_to(CPUInstruction&);
        void deserialize_to(LoadDelaySlot&);
        void deserialize_to(ExecutedInstruction&);

        /**
         * @brief serialize gpu specific types 
         */
        void serialize_from(GPUCommand);

        /**
         * @brief de-serialize gpu specific types 
         */
        void deserialize_to(GPUCommand&);

        /**
         * @brief serialize gte specific types 
         */
        void serialize_from(GTEInstruction);
        template<typename T>
        void serialize_from(const GTEVector<T>& vector)
        {
            serialize_from(vector.x);
            serialize_from(vector.y);
            serialize_from(vector.z);
        }
        template<typename T>
        void serialize_from(const GTEMatrix<T>& matrix)
        {
            for(u32 i = 0; i < 3 * 3; i++)
            {
                serialize_from(matrix.data[i]);
            }
        }

        /**
         * @brief de-serialize gte specific types 
         */
        void deserialize_to(GTEInstruction&);
        template<typename T>
        void deserialize_to(GTEVector<T>& vector)
        {
            deserialize_to(vector.x);
            deserialize_to(vector.y);
            deserialize_to(vector.z);
        }
        template<typename T>
        void deserialize_to(GTEMatrix<T>& matrix)
        {
            for(u32 i = 0; i < 3 * 3; i++)
            {
                deserialize_to(matrix.data[i]);
            }
        }

        /**
         * @brief serialize mdec specific types 
         */
        void serialize_from(MDECInstruction);

        /**
         * @brief de-serialize mdec specific types 
         */
        void deserialize_to(MDECInstruction&);

        /**
         * @brief serialize peripherals specific types 
         */
        void serialize_from(CurrentlyCommunicatingWith);

        /**
         * @brief de-serialize peripherals specific types 
         */
        void deserialize_to(CurrentlyCommunicatingWith&);
        
        /**
         * @brief save accumulated serialized data to a file
         */
        void write_to_file(const std::string& file_path);

        /**
         * @brief load serialized data from a file
         */
        void read_from_file(const std::string& file_path);

    private:

        explicit SaveState():
            m_serialized_state_cursor(0)
        {

        }
        
        u32             m_serialized_state_cursor;
        std::vector<u8> m_serialized_state;

    };
}

#endif // SAVESTATE_HPP