/**
 * @file      Utils.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Bit manipulation and arithmetic utilities
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

#ifndef UTILS_HPP
#define UTILS_HPP

#include <array>
#include <string>
#include <cstring>
#include <climits>
#include <optional>
#include <type_traits>
#include "Types.hpp"
#include "Constants.hpp"

namespace PSX
{
    /**
     * @brief Reinterpret bit representation of From value into To value
     *
     * https://en.cppreference.com/w/cpp/numeric/bit_cast
     */
    template<typename To, typename From>
    std::enable_if_t<
        sizeof(To) == sizeof(From) &&
        std::is_trivially_copyable_v<From> &&
        std::is_trivially_copyable_v<To>,
    To> bit_cast(From value)
    {
        static_assert(std::is_trivially_constructible_v<To>);

        To result;
        std::memcpy(&result, &value, sizeof(To));
        return result;
    }

    /**
     * @brief check if integer value is a power of 2 
     *
     * http://www.graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
     */
    template<typename T>
    std::enable_if_t<std::is_integral<T>::value,
    bool> is_power_of_two(T value)
    {
        return value && !(value & (value - 1));
    }

    /**
     * @brief check if integer value is a power of 2 
     *
     * http://www.graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
     */
    template<typename T>
    constexpr
    std::enable_if_t<std::is_integral<T>::value,
    bool> constexpr_is_power_of_two(T value)
    {
        return value && !(value & (value - 1));
    }

    /**
     * @brief check whether address is multiple of the Value type size 
     */
    template<typename Value>
    bool is_aligned_to(u32 address)
    {
        if constexpr(constexpr_is_power_of_two(sizeof(Value)))
        {
            return !(address & (sizeof(Value) - 1));
        }
        else
        {
            return !(address % sizeof(Value));
        }
    }

    /**
     * @brief check whether result after adding 2 signed integers (2s complement) had overflow
     *
     * @arg x first number
     * @arg y second number
     * @arg add_result result of x + y
     * 
     * @returns true if overflow has occured, false otherwise 
     */
    template<typename T>
    std::enable_if_t<std::is_integral<T>::value,
    bool> check_overflow_add(T x, T y, T add_result)
    {
        return (((add_result ^ x) & (add_result ^ y)) & (1 << (sizeof(T) * CHAR_BIT - 1)));
    }

    /**
     * @brief check whether result after subtracting 2 signed integers (2s complement) had undeflow
     * 
     * @arg x first number
     * @arg y second number
     * @arg sub_result result of x - y
     * 
     * @returns true if undeflow has occured, false otherwise 
     */
    template<typename T>
    std::enable_if_t<std::is_integral<T>::value,
    bool> check_underflow_sub(T x, T y, T sub_result)
    {
        return (((sub_result ^ x) & (x ^ y)) & (1 << (sizeof(T) * CHAR_BIT - 1)));
    }

    /**
     * @brief extend sign of a arbitrary bit-length signed integer
     * 
     * http://www.graphics.stanford.edu/~seander/bithacks.html#FixedSignExtend
     */
    template<typename T, u32 Bits>
    std::enable_if_t<std::is_integral<T>::value && std::is_signed<T>::value,
    T> extend_sign(const T& x)
    {
        struct
        {
            T bits: Bits;
        } value;

        return value.bits = x;
    }

    /**
     * @brief convert binary-coded decimal to binary number
     */
    u8 bcd_to_binary(u8 bcd);

    /**
     * @brief circular fixed sized queue 
     */
    template<typename T, u32 Capacity>
    class fixed_queue
    {
    public:

        fixed_queue():
            m_start(0),
            m_end(0)
        {
            
        }

        bool push(const T& value)
        {
            if((m_end + 1) % Capacity == m_start)
                return false;
            
            m_data[m_end] = value;
            m_end = (m_end + 1) % Capacity;

            return true;
        }

        std::optional<T> top()
        {
            if(m_start == m_end)
                return {};
            
            return m_data[m_start];
        }

        std::optional<T> pop()
        {
            if(m_start == m_end)
                return {};

            T result = m_data[m_start];
            m_start = (m_start + 1) % Capacity;

            return result;
        }

        bool empty()
        {
            return m_start == m_end;
        }

    private:

        std::array<T, Capacity> m_data;
        u32 m_start;
        u32 m_end;
    };

    /**
     * @brief helper for managing fixed point arithmetic 
     */
    template<typename T, u32 Bits>
    union fixed
    {
        fixed(): raw(0)
        {

        }

        fixed(float value)
        {
            raw = static_cast<u32>(value * (1 << Bits));
        }

        float to_float() const
        {
            return float(raw) / (1 << Bits);
        }

        struct
        {
            T fraction: Bits;
            T integer: (sizeof(T) * 8) - Bits;
        };

        T raw;
    };
}

#endif // UTILS_HPP
