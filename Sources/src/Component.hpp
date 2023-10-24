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

        virtual void execute(u32 num_steps)        = 0;
        virtual u32  read(u32 address)             = 0;
        virtual void write(u32 address, u32 value) = 0;
        virtual void reset()                       = 0;
    };
}

#endif // COMPONENT_HPP