/**
 * @file      SerialPort.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX Serial Port
 *
 * @version   0.1
 *
 * @date      10. 11. 2023, 20:01 (created)
 *
 * @section License
 * This file is part of the Restation \n
 *
 * Copyright (C) 2023
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

#ifndef SERIALPORT_HPP
#define SERIALPORT_HPP

#include "Forward.hpp"
#include "Component.hpp"

#include <memory>

namespace PSX
{
    /**
     * @brief PSX Serial Port
     */
    class SerialPort final : public Component
    {
    public:

        SerialPort()
        {
            reset();
        }
        
        virtual ~SerialPort() override = default;

        virtual void execute(u32 num_steps) override;
        virtual u32  read(u32 address) override;
        virtual void write(u32 address, u32 value) override;
        virtual void reset() override;
        virtual void serialize(std::shared_ptr<SaveState>&) override;
        virtual void deserialize(std::shared_ptr<SaveState>&) override;

    private:

    };
}

#endif // SERIALPORT_HPP