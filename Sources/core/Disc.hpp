/**
 * @file      Disc.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX Disc
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


#ifndef DISC_HPP
#define DISC_HPP

#include <vector>
#include <memory>
#include <optional>
#include "DiscTypes.hpp"
#include "SaveState.hpp"

namespace PSX
{
    class Disc
    {
    public:

        /**
         * @brief create and initialize disc from disc dump 
         */
        static std::shared_ptr<Disc> create(const std::string& meta_file_path);

        /**
         * @brief create empty disc
         */
        static std::shared_ptr<Disc> create_unloaded();

        ~Disc();

        /**
         * @brief read sector from the disc 
         */
        Sector read_sector(const Position&);

        /**
         * @brief obtain number of loaded tracks
         */
        u32 num_tracks() const;

        /**
         * @brief read subchannel Q for a sector 
         */
        SubChannelQ read_subchannelq(const Position&);

        /**
         * @brief serialize loaded disc 
         */
        void serialize(std::shared_ptr<SaveState>&);

        /**
         * @brief deserialize loaded disc 
         */
        void deserialize(std::shared_ptr<SaveState>&);

    private:

        explicit Disc(): 
            m_meta_loaded(false) 
        {

        }

        /**
         * @brief load 'bin' disc dump and initialize the disc 
         */
        void meta_initialize_from_bin(const std::string& meta_file_path);

        /**
         * @brief get track index based on an absolute position 
         */
        std::optional<u32> get_track_index(const Position&);

        /**
         * @brief get track position offset 
         */
        Position get_track_offset(u32 index);

        std::vector<Track> m_tracks; /// track information  
        bool m_meta_loaded;          /// did we load disc into the emulator?
    };
}

#endif // DISC_HPP