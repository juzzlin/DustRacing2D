// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#include <vector>

class TrackTileBase;
class TrackDataBase;

//! Base class for the tile matrix used by TrackData.
class MapBase
{
public:

    //! Constructor.
    MapBase(TrackDataBase & trackData, unsigned int cols, unsigned int rows);

    //! Destructor.
    virtual ~MapBase();

    //! Get track data.
    TrackDataBase & trackData();

    //! Get column count
    unsigned int cols() const;

    //! Get row count
    unsigned int rows() const;

    //! Resize the map.
    virtual void resize(unsigned int newCols, unsigned int newRows);

    //! Set given tile to given coordinates.
    //! Returns false if impossible coordinates.
    bool setTile(unsigned int x, unsigned int y, TrackTileBase * pTile);

    //! Get tile at given coordinates.
    //! Returns nullptr if no tile set or impossible coordinates.
    TrackTileBase * getTile(unsigned int x, unsigned int y) const;

private:

    TrackDataBase & m_trackData;
    unsigned int m_cols, m_rows;
    typedef std::vector<TrackTileBase *> TrackTileRow;
    typedef std::vector<TrackTileRow> TrackTileMap;
    TrackTileMap m_map;
};

#endif // MAPBASE_HPP
