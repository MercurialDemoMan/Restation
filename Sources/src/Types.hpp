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
            m_raw = other.m_raw;
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