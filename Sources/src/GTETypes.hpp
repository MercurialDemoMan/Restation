/**
 * @file      GTETypes.hpp
 *
 * @author    Filip Stupka \n
 *            xstupk05@fit.vutbr.cz
 *
 * @brief     Header for the PSX Geometry Transformation Engine Types
 *
 * @version   0.1
 *
 * @date      9. 11. 2023, 10:12 (created)
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

#ifndef GTETYPES_HPP
#define GTETYPES_HPP

namespace PSX
{
    /**
     * @brief 3D Vector structure the PSX GTE operate with
     */
    template<typename T>
    struct GTEVector
    {
        union { T x, r; };
        union { T y, g; };
        union { T z, b; };

        void reset()
        {
            x = y = z = 0;
        }
    };

    /**
     * @brief 3x3 Matrix structure the PSX GTE operate with
     */
    template<typename T>
    struct GTEMatrix
    {
        T data[3 * 3];

        T& at(u32 x, u32 y)
        {
            return data[y * 3 + x];
        }

        const T& at(u32 x, u32 y) const
        {
            return data[y * 3 + x];
        }

        void reset()
        {
            for(u32 i = 0; i < 3 * 3; i++)
                data[i] = 0;
        }
    };
}

#endif // GTETYPES_HPP