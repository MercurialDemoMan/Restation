/**
 * @file      Disc.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Implementation for the PSX Disc
 *
 * @version   0.1
 *
 * @date      12. 2. 2024, 16:51 (created)
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

#include <filesystem>
#include <cmath>
#include "Disc.hpp"
#include "Macros.hpp"

namespace PSX
{
    /**
     * @brief create and initialize disc from disc dump 
     */
    std::shared_ptr<Disc> Disc::create(const std::string& meta_file_path)
    {
        std::shared_ptr<Disc> disc = std::shared_ptr<Disc>(new Disc());

        if(meta_file_path.ends_with(".bin"))
        {
            disc->initialize_from_bin(meta_file_path);
        }
        else
        {
            ABORT_WITH_MESSAGE(fmt::format("trying to load unsupported disc format {}", meta_file_path));
        }

        return disc;
    }

    /**
     * @brief load 'bin' disc dump and initialize the disc 
     */
    void Disc::initialize_from_bin(const std::string& meta_file_path)
    {
        auto file_size = std::filesystem::file_size(meta_file_path);

        LOG_DEBUG(1, fmt::format("loading disc from {} of size {}B", meta_file_path, file_size));

        auto meta_file = std::make_shared<std::ifstream>(meta_file_path, std::ios::binary);

        if(!meta_file->is_open())
        {
            ABORT_WITH_MESSAGE(fmt::format("failed to load disc from path {}", meta_file_path));
        }

        Track track =
        {
            .id_number     = 1,
            .type          = Track::Type::Data,
            .data_position = Position { 0 },
            .offset        = 0,
            .num_sectors   = std::ceil(float(file_size) / Sector::Size),
            .meta_file     = meta_file
        };

        m_tracks.push_back(track);
    }

    Disc::~Disc()
    {
        for(auto& track : m_tracks)
        {
            track.meta_file->close();
        }
    }
}