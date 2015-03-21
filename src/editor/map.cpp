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

#include "map.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"

#include <QPoint>
#include <QPointF>

Map::Map(TrackData & trackData, unsigned int cols, unsigned int rows)
  : MapBase(trackData, cols, rows)
{
    createEmptyTiles();
}

void Map::createEmptyTiles()
{
    // Create tiles and set coordinates.
    for (unsigned int j = 0; j < rows(); j++)
    {
        for (unsigned int i = 0; i < cols(); i++)
        {
            if (!getTile(i, j))
            {
                TrackTileBase * newTile = new TrackTile(
                    static_cast<TrackData &>(trackData()),
                    QPointF(TrackTile::TILE_W / 2 + i * TrackTile::TILE_W,
                    TrackTile::TILE_H / 2 + j * TrackTile::TILE_H),
                    QPoint(i, j));
                setTile(i, j, newTile);
            }
            else
            {
                getTile(i, j)->setLocation(
                    QPointF(
                        TrackTile::TILE_W / 2 + i * TrackTile::TILE_W,
                        TrackTile::TILE_H / 2 + j * TrackTile::TILE_H));
                getTile(i, j)->setMatrixLocation(QPoint(i, j));
            }
        }
    }
}

void Map::resize(unsigned int newCols, unsigned int newRows)
{
    MapBase::resize(newCols, newRows);

    createEmptyTiles();
}

void Map::insertColumn(unsigned int at)
{
    MapBase::insertColumn(at);

    createEmptyTiles();
}

void Map::deleteColumn(unsigned int at)
{
    MapBase::deleteColumn(at);
}

void Map::insertRow(unsigned int at)
{
    MapBase::insertRow(at);

    createEmptyTiles();
}

void Map::deleteRow(unsigned int at)
{
    MapBase::deleteRow(at);
}

Map::~Map()
{
}
