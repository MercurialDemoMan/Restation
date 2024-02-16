/**
 * @file      Types.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Common data types
 *
 * @version   0.1
 *
 * @date      25. 10. 2023, 20:39 (created)
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

#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>

namespace PSX
{
    /**
     * Common data types 
     */
    using u8  = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;
    using s8  = int8_t;
    using s16 = int16_t;
    using s32 = int32_t;
    using s64 = int64_t;

    /**
     * @brief Basic register used by Components
     */
    template<typename T>
    union Register
    {
    public:

        /**
         * @brief read specific byte from the register
         */
        u8 read(u32 address) const
        {
            return m_bytes[address];
        }

        /**
         * @brief write specific byte to the register
         */
        void write(u32 address, u8 value)
        {
            m_bytes[address] = value;
        }

        Register& operator=(const T& other)
        {
            m_raw = other;
            return *this;
        }

        T& raw()
        {
            return m_raw;
        }

    private:

        /**
         * NOTE: m_raw and m_bytes are !unionized! 
         */
        T  m_raw;
        u8 m_bytes[sizeof(T)];
    };
}

#endif // TYPES_HPP