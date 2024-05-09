/**
 * @file      PeripheralsMemoryCardController.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for a Memory Card peripheral
 *
 * @version   0.1
 *
 * @date      17. 2. 2024 (created)
 *
 * @section License
 * This file is part of the Restation \n
 *
 * Copyright (C) 2024
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

#ifndef PERIPHERALSMEMORYCARDCONTROLLER_HPP
#define PERIPHERALSMEMORYCARDCONTROLLER_HPP

#include "PeripheralsController.hpp"

namespace PSX
{
    /**
     * @brief Implementation for the Memory Card Peripheral
     */
    class PeripheralsMemoryCardController final : public PeripheralsController
    {
    public:

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

    };
}

#endif // PERIPHERALSMEMORYCARDCONTROLLER_HPP