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

Map::Map(size_t cols, size_t rows)
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
    for (size_t j = 0; j < rows(); j++)
    {
        for (size_t i = 0; i < cols(); i++)
        {
            setTile(i, j, std::make_shared<TrackTile>(*std::dynamic_pointer_cast<TrackTile>(other.getTile(i, j))));
        }
    }
}

void Map::createEmptyTiles()
{
    // Create tiles and set coordinates.
    for (size_t j = 0; j < rows(); j++)
    {
        for (size_t i = 0; i < cols(); i++)
        {
            if (!getTile(i, j))
            {
                const auto newTile = std::make_shared<TrackTile>(
                  QPointF(TrackTile::width() / 2 + i * TrackTile::width(),
                          TrackTile::height() / 2 + j * TrackTile::height()),
                  QPoint(static_cast<int>(i), static_cast<int>(j)));
                setTile(i, j, newTile);
            }
            else
            {
                getTile(i, j)->setLocation(
                  QPointF(
                    TrackTile::width() / 2 + i * TrackTile::width(),
                    TrackTile::height() / 2 + j * TrackTile::height()));
                getTile(i, j)->setMatrixLocation(QPoint(static_cast<int>(i), static_cast<int>(j)));
            }
        }
    }
}

void Map::moveTilesAfterColumnDeletion(size_t at)
{
    for (size_t j = 0; j < rows(); j++)
    {
        for (size_t i = at + 1; i < cols(); i++)
        {
            const QPointF location(getTile(i, j)->location());
            getTile(i, j)->setLocation(
              QPointF(
                location.x() - TrackTile::width(),
                location.y()));
            getTile(i, j)->setMatrixLocation(QPoint(static_cast<int>(i) - 1, static_cast<int>(j)));
        }
    }
}

void Map::moveTilesAfterRowDeletion(size_t at)
{
    for (size_t j = at + 1; j < rows(); j++)
    {
        for (size_t i = 0; i < cols(); i++)
        {
            const QPointF location(getTile(i, j)->location());
            getTile(i, j)->setLocation(
              QPointF(
                location.x(),
                location.y() - TrackTile::height()));
            getTile(i, j)->setMatrixLocation(QPoint(static_cast<int>(i), static_cast<int>(j) - 1));
        }
    }
}

void Map::resize(size_t newCols, size_t newRows)
{
    MapBase::resize(newCols, newRows);

    createEmptyTiles();
}

size_t Map::insertColumn(size_t at, InsertDirection insertDirection)
{
    at = MapBase::insertColumn(at, insertDirection);

    createEmptyTiles();

    return at;
}

std::vector<TrackTileBasePtr> Map::deleteColumn(size_t at)
{
    moveTilesAfterColumnDeletion(at);
    return MapBase::deleteColumn(at);
}

size_t Map::insertRow(size_t at, InsertDirection insertDirection)
{
    at = MapBase::insertRow(at, insertDirection);

    createEmptyTiles();

    return at;
}

std::vector<TrackTileBasePtr> Map::deleteRow(size_t at)
{
    moveTilesAfterRowDeletion(at);
    return MapBase::deleteRow(at);
}

Map::~Map()
{
}
