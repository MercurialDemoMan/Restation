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
#include <string>
#include <fmt/core.h>

/**
 * @brief mark variable as unused
 * 
 * shuts down compiler warnings 
 */
#define MARK_UNUSED(x) ((void)(x))

/**
 * @brief abort and print information where the abort occured
 */
#ifdef __FILE_NAME__ /// prints only basename of the source file instead of the full path (supported in clang and gcc)
#define ABORT_WITH_MESSAGE(message) do \
{ \
    std::fprintf(stderr, "[%s]: %s:%u, aborting...\n", std::string(message).c_str(), __FILE_NAME__, __LINE__); \
    std::exit(1); \
} while(0)
#else
#define ABORT_WITH_MESSAGE(message) do \
{ \
    std::fprintf(stderr, "[%s]: %s:%u, aborting...\n", std::string(message).c_str(), __FILE__, __LINE__); \
    std::exit(1); \
} while(0)
#endif

#define UNREACHABLE() ABORT_WITH_MESSAGE("\e[1;91munreachable code\e[0m")
#define TODO()        ABORT_WITH_MESSAGE("\e[1;95mtodo\e[0m")

/**
 * class manipulation utilities 
 */
#define DELETE_COPY_CONSTRUCTOR(class_name) class_name(const class_name&) = delete
#define DELETE_MOVE_CONSTRUCTOR(class_name) class_name(const class_name&&) = delete
#define DELETE_COPY_ASSIGNMENT(class_name) class_name operator=(const class_name&) = delete
#define DELETE_MOVE_ASSIGNMENT(class_name) class_name operator=(const class_name&&) = delete

/**
 * packing 
 */
#if defined(__GNUC__)
    #define PACKED(structure) structure __attribute__((packed))
#elif defined(_MSC_VER)
    #define PACKED(structure) __pragma(pack(push, 1)) structure __pragma(pack(pop))
#else
#error "Unsupported compiler, please specify struct packing in Macros.hpp for your compiler"
#endif

/**
 * logging utilities 
 */
#define LOG(message) do \
{ \
    std::fprintf(stdout, "[\e[0;36minfo\e[0m]: %s\n", std::string(message).c_str()); \
} while(0)
#define LOG_IF(value, message) do \
{ \
    if(value) \
    { \
        std::fprintf(stdout, "[\e[0;36minfo\e[0m]: %s\n", std::string(message).c_str()); \
    } \
} while(0)

#ifndef LOG_DEBUG_LEVEL
#define LOG_DEBUG(level, message)
#define LOG_WARNING(message)
#define LOG_ERROR(message)
#else
#define LOG_DEBUG(level, message) do \
{ \
    if((level) <= LOG_DEBUG_LEVEL) \
    { \
        std::fprintf(stdout, "[\e[0;35mdebug\e[0m]: %s\n", std::string(message).c_str()); \
    } \
} while(0)
#define LOG_WARNING(message) do \
{ \
    std::fprintf(stdout, "[\e[0;33mwarning\e[0m]: %s\n", std::string(message).c_str()); \
} while(0)
#define LOG_ERROR(message) do \
{ \
    std::fprintf(stderr, "[\e[1;31merror\e[0m]: %s\n", std::string(message).c_str()); \
} while(0)
#endif

#endif // MACROS_HPP