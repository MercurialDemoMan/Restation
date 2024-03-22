/**
 * @file      DMAController.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX Interrupt controller
 *
 * @version   0.1
 *
 * @date      26. 10. 2023, 16:51 (created)
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

#ifndef INTERRUPTCONTROLLER_HPP
#define INTERRUPTCONTROLLER_HPP

#include "Forward.hpp"
#include "Component.hpp"

#include <memory>

namespace PSX
{
    /**
     * @brief enumeration of all exception interrupt causes 
     */
    enum class Interrupt
    {
        VBlank     = 0,
        GPU        = 1,
        CDROM      = 2,
        DMA        = 3,
        Timer0     = 4,
        Timer1     = 5,
        Timer2     = 6,
        Peripheral = 7,
        SIO        = 8,
        SPU        = 9,
        Lightpen   = 10
    };

    /**
     * @brief PSX Direct memory access controller
     */
    class InterruptController final : public Component
    {
    public:

        InterruptController(const std::shared_ptr<ExceptionController>& exception_controller) :
            m_exception_controller(exception_controller)
        {
            reset();
        }
        
        virtual ~InterruptController() override = default;

        virtual void execute(u32) override {};
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;
        virtual void serialize(std::shared_ptr<SaveState>&) override;
        virtual void deserialize(std::shared_ptr<SaveState>&) override;

        /**
         * @brief check for queued up interrupt 
         */
        bool is_interrupt_pending();

        /**
         * @brief send exception to the cpu
         */
        void trigger_interrupt(Interrupt);

    private:

        std::shared_ptr<ExceptionController> m_exception_controller;

        Register<u32> m_status;
        Register<u32> m_mask;

    };
}

#endif // INTERRUPTCONTROLLER_HPP