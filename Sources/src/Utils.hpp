#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstring>
#include <climits>
#include <type_traits>
#include "Types.hpp"

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
     * @brief check whether result after adding 2 integers had overflow
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
     * @brief check whether result after subtracting 2 integers had undeflow
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
}

#endif // UTILS_HPP
