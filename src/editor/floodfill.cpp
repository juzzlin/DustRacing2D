// This file is part of Dust Racing 2D.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
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

#include "floodfill.hpp"
#include "map.hpp"
#include "tracktile.hpp"

#include <QAction>

namespace FloodFill {
void setTileType(TrackTile & tile, QAction * action)
{
    tile.setTileType(action->data().toString());
    tile.setPixmap(action->icon().pixmap(TrackTile::width(), TrackTile::height()));
}
} // namespace FloodFill

void FloodFill::floodFill(TrackTile & tile, QAction * action, const QString & typeToFill, MapBase & map)
{
    static const int DIRECTION_COUNT = 4;

    // Coordinates of neighbor tiles can be calculated by adding these
    // adjustments to tile coordinates.
    static const QPoint neighborAdjustments[DIRECTION_COUNT] = {
        QPoint(1, 0), // right
        QPoint(0, -1), // up
        QPoint(-1, 0), // left
        QPoint(0, 1) // down
    };

    setTileType(tile, action);

    QPoint location = tile.matrixLocation();

    for (int i = 0; i < DIRECTION_COUNT; ++i)
    {
        int x = location.x() + neighborAdjustments[i].x();
        int y = location.y() + neighborAdjustments[i].y();

        if (x >= 0 && y >= 0)
        {
            auto tile = std::dynamic_pointer_cast<TrackTile>(map.getTile(x, y));
            if (tile && tile->tileType() == typeToFill)
            {
                FloodFill::floodFill(*(tile.get()), action, typeToFill, map);
            }
        }
    }
}
