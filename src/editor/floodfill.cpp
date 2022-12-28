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

#include <deque>

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
    // Coordinates of neighbor tiles can be calculated by adding these offsets to tile coordinates.
    const size_t directionCount = 4;
    const QPoint neighborOffsets[directionCount] = {
        { 1, 0 }, // right
        { 0, -1 }, // up
        { -1, 0 }, // left
        { 0, 1 } // down
    };

    std::deque<QPoint> stack;
    stack.push_back(tile.matrixLocation());
    while (stack.size())
    {
        const auto location = stack.back();
        if (const auto tile = std::dynamic_pointer_cast<TrackTile>(map.getTile(location.x(), location.y())))
        {
            setTileType(*tile, action);
        }
        stack.pop_back();

        for (size_t i = 0; i < directionCount; ++i)
        {
            const int x = location.x() + neighborOffsets[i].x();
            const int y = location.y() + neighborOffsets[i].y();
            if (x >= 0 && y >= 0 && x < static_cast<int>(map.cols()) && y < static_cast<int>(map.rows()))
            {
                if (const auto tile = std::dynamic_pointer_cast<TrackTile>(map.getTile(x, y)); tile && tile->tileType() == typeToFill)
                {
                    stack.push_back({ x, y });
                }
            }
        }
    }
}
