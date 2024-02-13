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
#include "DiscConstants.hpp"

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
            disc->meta_initialize_from_bin(meta_file_path);
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
    void Disc::meta_initialize_from_bin(const std::string& meta_file_path)
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
            .data_position = Position { .minutes = 0, .seconds = 0, .fractions = 0 },
            .offset        = 0,
            .num_sectors   = u32(std::ceil(float(file_size) / float(Sector::Size))),
            .meta_file     = meta_file
        };

        m_tracks.push_back(track);
    }

    /**
     * @brief get track position offset 
     */
    Position Disc::get_track_offset(u32 index)
    {
        u32 result = 0;

        for(s32 i = 0; i < s32(index) - 1; i++)
        {
            result += m_tracks[i].num_sectors;

            // note: for some reason, first track containing data starts at 2 second offset
            if(i == 0 && m_tracks[i].type == Track::Type::Data)
            {
                result += FractionsPerSecond * 2;
            }
        }

        return Position::create(result);
    }

    /**
     * @brief get track index based on an absolute position 
     */
    std::optional<u32> Disc::get_track_index(const Position& pos)
    {
        for(u32 i = 0; i < m_tracks.size(); i++)
        {
            auto offset = get_track_offset(i);
            auto size   = m_tracks[i].num_sectors;

            if(pos.linear_block_address() >= offset.linear_block_address() &&
               pos.linear_block_address() <  offset.linear_block_address() + size)
            {
                return i;
            }
        }

        return {};
    }

    /**
     * @brief read sector from the disc 
     */
    Sector Disc::read_sector(const Position& pos)
    {
        auto index_or_error = get_track_index(pos);

        if(index_or_error)
        {
            auto index    = index_or_error.value();
            auto track    = m_tracks[index];
            auto disc_pos = pos;

            // note: for some reason, first track containing data starts at 2 second offset, 
            //       which is relevant in subchannelQ handling, so in order to correctly 
            //       read from the file, we need to set it back 2 seconds
            if(index == 0 && track.type == Track::Type::Data)
            {
                disc_pos = Position::create(pos.linear_block_address() - 2 * FractionsPerSecond);
            }

            // TODO: if audio present, we need to manipulate the disc_pos

            std::vector<u8> sector_buffer(Sector::Size);
            track.meta_file->seekg(track.offset + disc_pos.linear_block_address() * Sector::Size);
            track.meta_file->read(reinterpret_cast<char*>(sector_buffer.data()), Sector::Size);
            if(track.meta_file->eof())
            {
                ABORT_WITH_MESSAGE(fmt::format("trying to read outside of the disc file at {}", pos.linear_block_address()));
            }

            return Sector
            {
                .data = std::move(sector_buffer)
            };
        }
        else
        {
            ABORT_WITH_MESSAGE(fmt::format("trying to read from invalid position {}", pos.linear_block_address()));
        }
    }

    Disc::~Disc()
    {
        for(auto& track : m_tracks)
        {
            track.meta_file->close();
        }
    }
}