/**
 * @file      PeripheralsDigitalController.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for a digital controller peripheral
 *
 * @version   0.1
 *
 * @date      17. 2. 2024 (created)
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

#ifndef PERIPHERALSDIGITALCONTROLLER_HPP
#define PERIPHERALSDIGITALCONTROLLER_HPP

#include "PeripheralsController.hpp"

namespace PSX
{
    /**
     * @brief Implementation for the Digital Controller Peripheral
     */
    class PeripheralsDigitalController final : public PeripheralsController
    {
    public:

        PeripheralsDigitalController(const std::shared_ptr<PeripheralsInput>& input);

        /**
         * @brief transfer and handle 1 byte in communication sequence
         */
        virtual u8 send_byte(u8) override;

        /**
         * @brief check if controller ended communication sequence 
         */
        virtual bool communication_ended() const override;

        /**
         * @brief send acknowledge flag back
         */
        virtual bool ack() const override;

        /**
         * @brief reset any communication temporaries 
         */
        virtual void reset() override;

        virtual void serialize(std::shared_ptr<SaveState>&) override;
        virtual void deserialize(std::shared_ptr<SaveState>&) override;
    private:

        /**
         * @brief Internal bit layout to determine buttons states 
         */
        union ButtonsState
        {
            struct
            {
                u16 select:   1;
                u16 l3:       1;
                u16 r3:       1;
                u16 start:    1;
                u16 up:       1;
                u16 right:    1;
                u16 down:     1;
                u16 left:     1;
                u16 l2:       1;
                u16 r2:       1;
                u16 l1:       1;
                u16 r1:       1;
                u16 triangle: 1;
                u16 circle:   1;
                u16 cross:    1;
                u16 square:   1;
            };

            u16 raw;
            u8  bytes[sizeof(u16)];
        };

        std::shared_ptr<PeripheralsInput> m_input;

        ButtonsState m_buttons_state;
        u32          m_communication_counter;
    };
}

#endif // PERIPHERALSDIGITALCONTROLLER_HPP