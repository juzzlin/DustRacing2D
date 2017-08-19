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
#include "tracktile.hpp"

#include <QPoint>
#include <QPointF>

#include <memory>

Map::Map(unsigned int cols, unsigned int rows)
    : MapBase(cols, rows)
{
    createEmptyTiles();
}

Map::Map(const Map & other)
    : MapBase(other.cols(), other.rows())
{
    copyTiles(other);
}

void Map::copyTiles(const Map & other)
{
    // Create tiles and set coordinates.
    for (unsigned int j = 0; j < rows(); j++)
    {
        for (unsigned int i = 0; i < cols(); i++)
        {
            auto newTile = new TrackTile(*std::dynamic_pointer_cast<TrackTile>(other.getTile(i, j)));
            setTile(i, j, TrackTileBasePtr(newTile));
        }
    }
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
                auto newTile = new TrackTile(
                    QPointF(TrackTile::TILE_W / 2 + i * TrackTile::TILE_W,
                    TrackTile::TILE_H / 2 + j * TrackTile::TILE_H),
                    QPoint(i, j));
                setTile(i, j, TrackTileBasePtr(newTile));
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

void Map::moveTilesAfterColumnDeletion(unsigned int at)
{
    for (unsigned int j = 0; j < rows(); j++)
    {
        for (unsigned int i = at + 1; i < cols(); i++)
        {
            const QPointF location(getTile(i, j)->location());
            getTile(i, j)->setLocation(
                QPointF(
                    location.x() - TrackTile::TILE_W,
                    location.y()));
            getTile(i, j)->setMatrixLocation(QPoint(i - 1, j));
        }
    }
}

void Map::moveTilesAfterRowDeletion(unsigned int at)
{
    for (unsigned int j = at + 1; j < rows(); j++)
    {
        for (unsigned int i = 0; i < cols(); i++)
        {
            const QPointF location(getTile(i, j)->location());
            getTile(i, j)->setLocation(
                QPointF(
                    location.x(),
                    location.y() - TrackTile::TILE_H));
            getTile(i, j)->setMatrixLocation(QPoint(i, j - 1));
        }
    }
}

void Map::resize(unsigned int newCols, unsigned int newRows)
{
    MapBase::resize(newCols, newRows);

    createEmptyTiles();
}

unsigned int Map::insertColumn(unsigned int at, InsertDirection insertDirection)
{
    at = MapBase::insertColumn(at, insertDirection);

    createEmptyTiles();

    return at;
}

std::vector<TrackTileBasePtr> Map::deleteColumn(unsigned int at)
{
    moveTilesAfterColumnDeletion(at);
    return MapBase::deleteColumn(at);
}

unsigned int Map::insertRow(unsigned int at, InsertDirection insertDirection)
{
    at = MapBase::insertRow(at, insertDirection);

    createEmptyTiles();

    return at;
}

std::vector<TrackTileBasePtr> Map::deleteRow(unsigned int at)
{
    moveTilesAfterRowDeletion(at);
    return MapBase::deleteRow(at);
}

Map::~Map()
{
}
