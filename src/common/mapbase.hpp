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
    MapBase(unsigned int cols, unsigned int rows);

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
    unsigned int cols() const;

    //! Get row count
    unsigned int rows() const;

    //! Resize the map.
    virtual void resize(unsigned int newCols, unsigned int newRows);

    /*! Set given tile to given coordinates.
     *  Returns false if impossible coordinates. */
    bool setTile(unsigned int x, unsigned int y, TrackTileBasePtr tile);

    /*! Get tile at given coordinates.
     *  Returns nullptr if no tile set or impossible coordinates. */
    TrackTileBasePtr getTile(unsigned int x, unsigned int y) const;

    //! Insert column after given index.
    virtual unsigned int insertColumn(unsigned int at, InsertDirection insertDirection);

    //! Delete column at given index. Return deleted tiles.
    virtual std::vector<TrackTileBasePtr> deleteColumn(unsigned int at);

    //! Insert row after given index.
    virtual unsigned int insertRow(unsigned int at, InsertDirection insertDirection);

    //! Delete row at given index. Return deleted tiles.
    virtual std::vector<TrackTileBasePtr> deleteRow(unsigned int at);

private:
    unsigned int m_cols, m_rows;

    typedef std::vector<TrackTileBasePtr> TrackTileRow;
    typedef std::vector<TrackTileRow> TrackTileMap;
    TrackTileMap m_map;
};

#endif // MAPBASE_HPP
