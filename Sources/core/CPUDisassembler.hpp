/**
 * @file      CPUDisassembler.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the MIPS R3000 PSX Central processing unit disassembler
 *
 * @version   0.1
 *
 * @date      3. 11. 2023, 13:08 (created)
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

#ifndef CPUDISASSEMBLER_HPP
#define CPUDISASSEMBLER_HPP

#include "CPUInstruction.hpp"

#include <string>

namespace PSX
{
    /**
     * @brief take encoded CPU instruction and convert it to human readable format 
     */
    std::string disassemble(const CPUInstruction&);
}

#endif // CPUDISASSEMBLER_HPP