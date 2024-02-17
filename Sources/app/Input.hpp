/**
 * @file      Input.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for sending input to the host emulator
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

#ifndef INPUT_HPP
#define INPUT_HPP

#include <memory>
#include <unordered_map>
#include "../core/PeripheralsInput.hpp"
#include "../core/Types.hpp"

class Input final : public PSX::PeripheralsInput
{
public:

    /**
     * @brief allocate input manager
     */
    static std::shared_ptr<Input> create();

    /**
     * @brief Interface used by peripherals component to receive input from host system
     */
    virtual bool is_digital_button_down(PSX::PeripheralsInput::DigitalButton) override;

private:

    explicit Input() {}

    std::unordered_map<PSX::u32, PSX::PeripheralsInput::DigitalButton> m_button_mapping;

};

#endif // INPUT_HPP