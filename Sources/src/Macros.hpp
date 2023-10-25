/**
 * @file      Macros.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Common utility macros
 *
 * @version   0.1
 *
 * @date      25. 10. 2023, 20:39 (created)
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

#ifndef MACROS_HPP
#define MACROS_HPP

#include <cstdio>
#include <cstdlib>

/**
 * @brief mark variable as unused
 * 
 * shuts down compiler warnings 
 */
#define MARK_UNUSED(x) ((void)(x))

/**
 * @brief abort and print information where the abort occured
 */
#define ABORT_WITH_MESSAGE(message) do \
{ \
    std::fprintf(stderr, "Encountered %s at %s:%u, aborting...\n", message, __FILE__, __LINE__); \
    std::exit(1); \
} while(0)

#define UNREACHABLE() ABORT_WITH_MESSAGE("\e[1;91munreachable code\e[0m")
#define TODO()        ABORT_WITH_MESSAGE("\e[1;95mtodo\e[0m")

#endif // MACROS_HPP