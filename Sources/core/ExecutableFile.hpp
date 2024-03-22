/**
 * @file      ExecutableFile.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX immediate executable file
 *
 * @version   0.1
 *
 * @date      11. 03. 2024, 15:22 (created)
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

#ifndef EXECUTABLEFILE_HPP
#define EXECUTABLEFILE_HPP

#include "Types.hpp"
#include "Macros.hpp"
#include "CPUInstruction.hpp"

#include <memory>
#include <string>
#include <vector>

namespace PSX
{
    /**
     * @brief Loadable debug PSX Executable 
     */
    class ExecutableFile
    {
    public:

        /**
         * @brief create and initialize executable 
         */
        static std::shared_ptr<ExecutableFile> create(const std::string& meta_file_path);

        /**
         * @brief access header information 
         */
        u32 initial_pc() const;
        u32 initial_gp() const;
        u32 initial_sp() const;
        u32 text_base() const;
        const std::vector<u8>& text() const;

    private:

        explicit ExecutableFile()
        {
            
        }

        /**
         * @brief load 'exe' program and initialize the structure 
         */
        void meta_initialize_from_exe(const std::string& meta_file_path);

        /**
         * @brief definition of the PSX Executable header
         */
        PACKED(struct Header
        {
            u8  ascii_id[8]; // PS-X EXE
            u8  zerofilled[8];
            u32 initial_pc;
            u32 initial_gp_r28;
            u32 ram_placement;
            u32 file_size;
            u32 data_section_address;
            u32 data_section_size;
            u32 bss_section_address;
            u32 bss_section_size;
            u32 initial_sp_r29_base;
            u32 initial_sp_r29_offset;
            u8  reserved[20];
            u8  ascii_marker[0x07B4];
        });

        static_assert(sizeof(Header) == 0x0800);

        Header          m_header;  // header information about the executable
        std::vector<u8> m_program; // text section of the executable
    };
}

#endif // EXECUTABLEFILE_HPP