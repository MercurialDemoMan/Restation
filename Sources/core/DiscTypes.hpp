/**
 * @file      CDROM.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Definition of PSX Disc specific types
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

#ifndef DISCTYPES_HPP
#define DISCTYPES_HPP

#include "Types.hpp"
#include "DiscConstants.hpp"

#include <vector>
#include <fstream>

namespace PSX
{
    /**
     * @brief manage position on a disc 
     */
    struct Position
    {
        static Position create(u32 linear_block_address)
        {
            return Position
            {
                .minutes   = (linear_block_address /  SecondsPerMinute / FractionsPerSecond),
                .seconds   = (linear_block_address % (SecondsPerMinute * FractionsPerSecond)) / FractionsPerSecond,
                .fractions = (linear_block_address % FractionsPerSecond)
            };
        }

        u32 linear_block_address() const
        {
            return (minutes * SecondsPerMinute * FractionsPerSecond) + (seconds * FractionsPerSecond) + fractions;
        }

        u32 minutes;
        u32 seconds;
        u32 fractions;
    };

    /**
     * @brief description of a singular track on a disc
     */
    struct Track
    {
        enum class Type : u8
        {
            Data    = 0,
            Audio   = 1,
            Invalid = 2
        };

        u32      id_number;
        Type     type;
        Position data_position;
        u32      offset;
        u32      num_sectors;
        std::string meta_file_path;
        std::shared_ptr<std::ifstream> meta_file;
    };

    /**
     * @brief portion of a disc track
     */
    struct Sector
    {
        static constexpr u32 SizeWithoutHeader                 = 2048;
        static constexpr u32 SizeWithHeaderAndWithoutSyncBytes = 2340;
        static constexpr u32 SizeWithHeaderAndWithSyncBytes    = 2352;
        static constexpr u32 ReadDelay                         = SizeWithHeaderAndWithoutSyncBytes;
        static constexpr u32 SyncBytesSize                     = SizeWithHeaderAndWithSyncBytes - SizeWithHeaderAndWithoutSyncBytes;

        std::vector<u8> data;
        Track::Type     type;
    };

    /**
     * @brief subchannel multiplexed with the user data
     *        contains meta information about following sector
     *        like current position and track index
     *        and also acts as a checksum for error detection (not implemented)
     * https://en.wikipedia.org/wiki/Compact_Disc_subcode
     * https://psx-spx.consoledev.net/cdromdrive/#cdrom-subchannels
     */
    struct SubChannelQ
    {
        u8 track_number;
        u8 index_number;
        u8 relative_position_mm;
        u8 relative_position_ss;
        u8 relative_position_ff;
        u8 reserved;
        u8 absolute_position_mm;
        u8 absolute_position_ss;
        u8 absolute_position_ff;
    };
}

#endif // DISCTYPES_HPP