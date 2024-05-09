/**
 * @file      MemoryRegion.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     PSX memory region for storing volatile data
 *
 * @version   0.1
 *
 * @date      4. 11. 2023, 10:25 (created)
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

#ifndef MEMORY_REGION_HPP
#define MEMORY_REGION_HPP

#include "Types.hpp"
#include "Macros.hpp"

#include <array>
#include <cstring>

namespace PSX
{
    /**
     * @brief implementation of PSX memory regions 
     */
    template<u32 Size>
    class MemoryRegion
    {
    public:

        MemoryRegion()
        {
            reset();
        }

        /**
         * @brief clear the memory with zeros 
         */
        void reset()
        {
            std::memset(m_storage, 0, Size);
        }

        /**
         * @brief read from memory region
         */
        template<typename T>
        T read(u32 address) const
        {
            if(address >= Size)
            {
                UNREACHABLE();
            }

            return reinterpret_cast<const T*>(m_storage)[address / sizeof(T)];
        }

        /**
         * @brief write into the memory region 
         */
        template<typename T>
        void write(u32 address, T value)
        {
            if(address >= Size)
            {
                UNREACHABLE();
            }

            reinterpret_cast<T*>(m_storage)[address / sizeof(T)] = value;
        }

        /**
         * @brief get pointer to the storage
         */
        void* data()
        {
            return m_storage;
        }

    private:

        u8 m_storage[Size];

    };
}

#endif // MEMORY_REGION_HPP