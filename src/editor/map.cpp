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

#include "map.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"

#include <QPoint>
#include <QPointF>

Map::Map(TrackData * trackData, unsigned int cols, unsigned int rows)
: MapBase(trackData, cols, rows)
{
    // Create tiles and set coordinates.
    for (unsigned int i = 0; i < cols; i++)
        for (unsigned int j = 0; j < rows; j++)
        {
            TrackTileBase * newTile = new TrackTile(trackData,
                QPointF(TrackTile::TILE_W / 2 + i * TrackTile::TILE_W,
                TrackTile::TILE_H / 2 + j * TrackTile::TILE_H),
                QPoint(i, j));
            setTile(i, j, newTile);
        }
}

Map::~Map()
{
}
