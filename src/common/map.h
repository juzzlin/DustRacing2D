// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#ifndef MAP_H
#define MAP_H

#include <QVector>

class TrackTile;
class TrackData;

//! The tile matrix used by TrackData.
class Map
{
public:

    //! Constructor
    Map(TrackData * trackData, unsigned int cols, unsigned int rows);

    //! Get column count
    unsigned int cols() const;

    //! Get row count
    unsigned int rows() const;

    //! Set given tile to given coordinates.
    //! Returns false if impossible coordinates.
    bool setTile(unsigned int x, unsigned int y, TrackTile * pTile);

    //! Get tile at given coordinates.
    //! Returns nullptr if no tile set or impossible coordinates.
    TrackTile * tile(unsigned int x, unsigned int y) const;

private:

    TrackData * m_trackData;
    unsigned int m_cols, m_rows;
    QVector<QVector<TrackTile *> > m_map;
};

#endif // MAP_H
