#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "Types.hpp"
#include "Macros.hpp"

namespace PSX
{
    /**
     * @brief interface for psx hardware component connected to the Bus 
     */
    class Component
    {
    public:
        virtual ~Component() = default;

        /**
         * @brief execute the device for num_steps amount of clock cycles 
         */
        virtual void execute(u32 num_steps) = 0;

        /**
         * @brief read data from the device
         * 
         * @arg address relative address in the device
         * 
         * @returns read value
         */
        virtual u32  read(u32 address) = 0;

        /**
         * @brief write data itno the device
         * 
         * @arg address relative address in the device 
         * @arg value to be written
         */
        virtual void write(u32 address, u32 value) = 0;

        /**
         * @brief reset device into its' initial state
         */
        virtual void reset() = 0;
    };
}

#endif // COMPONENT_HPP