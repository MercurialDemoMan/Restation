/**
 * @file      MenuTypes.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for menu configuration types
 *
 * @version   0.1
 *
 * @date      16. 2. 2024 (created)
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

#ifndef MENUTYPES_HPP
#define MENUTYPES_HPP

#include "../core/Types.hpp"

enum class EmulatorSpeed : PSX::u8
{
    _25Percent,
    _50Percent,
    _100Percent,
    Unlimited
};

#endif // MENUTYPES_HPP