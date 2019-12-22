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

#include "map.hpp"
#include "tracktile.hpp"

#include <QPoint>
#include <QPointF>

Map::Map(size_t cols, size_t rows)
  : MapBase(cols, rows)
{
    // Create tiles and set coordinates.
    for (size_t i = 0; i < cols; i++)
    {
        for (size_t j = 0; j < rows; j++)
        {
            auto newTile = std::make_shared<TrackTile>(
              QPointF(TrackTile::width() / 2 + i * TrackTile::width(),
                      TrackTile::height() / 2 + j * TrackTile::height()),
              QPoint(static_cast<int>(i), static_cast<int>(j)));
            setTile(i, j, newTile);
        }
    }
}

Map::~Map()
{
}
