/**
 * @file      CDROMConstants.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Definition of PSX CDROM specific constants
 *
 * @version   0.1
 *
 * @date      12. 2. 2024, 16:51 (created)
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

#ifndef CDROMCONSTANTS_HPP
#define CDROMCONSTANTS_HPP

#include "Types.hpp"

namespace PSX
{
    constexpr const u32 ResponseFIFOSize = 16;
    constexpr const u32 ParameterFIFOSize = 16;
    constexpr const u32 InterruptFIFOSize = 16;
}

#endif // CDROMCONSTANTS_HPP