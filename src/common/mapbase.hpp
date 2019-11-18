// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#ifndef MAPBASE_HPP
#define MAPBASE_HPP

#include "tracktilebase.hpp"
#include <vector>

//! Base class for the tile matrix used by TrackData.
class MapBase
{
public:
    //! Constructor.
    MapBase(size_t cols, size_t rows);

    MapBase(const MapBase & other) = delete;

    MapBase & operator=(const MapBase & other) = delete;

    enum class InsertDirection
    {
        Before,
        After
    };

    //! Destructor.
    virtual ~MapBase();

    //! Get column count
    size_t cols() const;

    //! Get row count
    size_t rows() const;

    //! Resize the map.
    virtual void resize(size_t newCols, size_t newRows);

    /*! Set given tile to given coordinates.
     *  Returns false if impossible coordinates. */
    bool setTile(size_t x, size_t y, TrackTileBasePtr tile);

    /*! Get tile at given coordinates.
     *  Returns nullptr if no tile set or impossible coordinates. */
    TrackTileBasePtr getTile(size_t x, size_t y) const;

    //! Insert column after given index.
    virtual size_t insertColumn(size_t at, InsertDirection insertDirection);

    //! Delete column at given index. Return deleted tiles.
    virtual std::vector<TrackTileBasePtr> deleteColumn(size_t at);

    //! Insert row after given index.
    virtual size_t insertRow(size_t at, InsertDirection insertDirection);

    //! Delete row at given index. Return deleted tiles.
    virtual std::vector<TrackTileBasePtr> deleteRow(size_t at);

private:
    size_t m_cols, m_rows;

    typedef std::vector<TrackTileBasePtr> TrackTileRow;
    typedef std::vector<TrackTileRow> TrackTileMap;
    TrackTileMap m_map;
};

#endif // MAPBASE_HPP
