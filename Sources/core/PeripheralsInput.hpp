/**
 * @file      PeripheralsInput.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Interface class for sending input to the host emulator
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

#ifndef PERIPHERALSINPUT_HPP
#define PERIPHERALSINPUT_HPP

namespace PSX
{
    /**
     * @brief Interface used by peripherals to receive input from the host system
     */
    class PeripheralsInput
    {
    public:

        /**
         * @brief layout of digital controller buttons 
         */
        enum class DigitalButton
        {
            Select   =  0,
            L3       =  1,
            R3       =  2,
            Start    =  3,
            Up       =  4,
            Right    =  5,
            Down     =  6,
            Left     =  7,
            L2       =  8,
            R2       =  9,
            L1       = 10,
            R1       = 11,
            Triangle = 12,
            Circle   = 13,
            Cross    = 14,
            Square   = 15,
            Size     = 16
        };

        static constexpr const char* DigitalButtonName[] =
        {
            "Select",
            "L3",
            "R3",
            "Start",
            "Up",
            "Right",
            "Down",
            "Left",
            "L2",
            "R2",
            "L1",
            "R1",
            "Triangle",
            "Circle",
            "Cross",
            "Square",
        };

        /**
         * @brief check if specific button is currently pressed 
         */
        virtual bool is_digital_button_down(DigitalButton) = 0;
    };
}

#endif // PERIPHERALSINPUT_HPP