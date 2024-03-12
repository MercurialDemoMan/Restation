/**
 * @file      ExecutableFile.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implentation for the PSX immediate executable file
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

#include "ExecutableFile.hpp"
#include <filesystem>
#include <fstream>

namespace PSX
{
    /**
     * @brief create and initialize executable 
     */
    std::shared_ptr<ExecutableFile> ExecutableFile::create(const std::string& meta_file_path)
    {
        auto exe_file = std::shared_ptr<ExecutableFile>(new ExecutableFile());

        if(meta_file_path.ends_with(".exe"))
        {
            exe_file->meta_initialize_from_exe(meta_file_path);
        }
        else
        {
            ABORT_WITH_MESSAGE(fmt::format("trying to load unsupported disc format {}", meta_file_path));
        }

        return exe_file;
    }

    /**
     * @brief load 'exe' program and initialize the structure 
     */
    void ExecutableFile::meta_initialize_from_exe(const std::string& meta_file_path)
    {
        LOG(fmt::format("loading executable from {}", meta_file_path));

        auto file_size = std::filesystem::file_size(meta_file_path);

        if(file_size <= sizeof(Header))
        {
            ABORT_WITH_MESSAGE(fmt::format("failed to load executable from path {}: file is too small to be PS-X Executable (file size: {}B, minimal size: {}B)", meta_file_path, file_size, sizeof(Header)));
        }

        LOG_DEBUG(1, fmt::format("loading exe from {} of size {}B", meta_file_path, file_size));

        std::ifstream meta_file(meta_file_path, std::ios::binary);

        if(!meta_file.is_open())
        {
            ABORT_WITH_MESSAGE(fmt::format("failed to load executable from path {}", meta_file_path));
        }

        std::vector<u8> exe_file_contents((std::istreambuf_iterator<char>(meta_file)),
                                           std::istreambuf_iterator<char>());
        
        if(exe_file_contents.size() <= sizeof(Header))
        {
            ABORT_WITH_MESSAGE(fmt::format("failed to load executable from path {}: file is too small to be PS-X Executable (file size: {}B, minimal size: {}B)", meta_file_path, file_size, sizeof(Header)));
        }

        // copy header
        std::memcpy(&m_header, exe_file_contents.data(), sizeof(Header));

        if(exe_file_contents.size() - sizeof(Header) != m_header.file_size)
        {
            auto header_file_size = m_header.file_size;
            ABORT_WITH_MESSAGE(fmt::format("failed to lead executable from path {}: the text size {}B does not match the text size in the header {}B", meta_file_path, exe_file_contents.size() - sizeof(Header), header_file_size));
        }

        // copy text section
        m_program.resize(m_header.file_size);
        std::memcpy(m_program.data(), exe_file_contents.data() + sizeof(Header), m_header.file_size);

        if(m_header.file_size != m_program.size())
        {
            auto header_file_size = m_header.file_size;
            ABORT_WITH_MESSAGE(fmt::format("failed to load executable from path {}: the file size {}B does not match the file size in the header {}B", meta_file_path, m_program.size(), header_file_size));
        }

        LOG("exe loaded");

        meta_file.close();
    }

    /**
     * @brief access PS-X Executable information 
     */
    u32 ExecutableFile::initial_pc() const
    {
        return m_header.initial_pc;
    }
    u32 ExecutableFile::initial_gp() const
    {
        return m_header.initial_gp_r28;
    }
    u32 ExecutableFile::initial_sp() const
    {
        return m_header.initial_sp_r29_base + m_header.initial_sp_r29_offset;
    }
    u32 ExecutableFile::text_base() const
    {
        return m_header.ram_placement;
    }
    const std::vector<u8>& ExecutableFile::text() const
    {
        return m_program;
    }

}