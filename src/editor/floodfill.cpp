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

#define FLOOD_LINE(c) (&flood_buf[c])

namespace FloodFill {

enum FloodFlags
{
    KFlood_Nonoe = 0,
    KFlood_In = 1,
    KFlood_Up = 2,
    KFlood_Down = 4,
};

struct Flood_Line
{
    short flags; // status of the segment
    short upos, dpos; // up and down ends of segment
    short x; // x coordinate of the segment
    int next; // linked list if several per colum
};

static int flood_count = 0; // number of flooded segments
static QList<Flood_Line> flood_buf = QList<Flood_Line> {};

void setTileType(TrackTile & tile, QAction * action)
{
    tile.setTileType(action->data().toString());
    tile.setPixmap(action->icon().pixmap(TrackTile::width(), TrackTile::height()));
}

static void FillLine(int x, int top, int bottom, QAction * action, MapBase & map)
{
    for (int y = top; y <= bottom; y++)
    {
        auto tile = std::dynamic_pointer_cast<TrackTile>(map.getTile(x, y));
        setTileType(*(tile.get()), action);
    }
}

// Fill in the vertical bar and return the y-coordinate of the first processing part
static int Flooder(int x, int y, const QString & typeToFill, QAction * action, MapBase & map)
{
    Flood_Line * p;
    int up = 0, down = 0;
    int c;

    // Check starting point
    auto tile = std::dynamic_pointer_cast<TrackTile>(map.getTile(x, y));
    if (tile && tile->tileType() != typeToFill)
    {
        return y + 1;
    }

    // up
    for (up = y - 1; up >= 0; up--)
    {
        tile = std::dynamic_pointer_cast<TrackTile>(map.getTile(x, up));
        if (tile && tile->tileType() != typeToFill)
        {
            break;
        }
    }

    // down
    for (down = y + 1; down < map.rows(); down++)
    {
        tile = std::dynamic_pointer_cast<TrackTile>(map.getTile(x, down));
        if (tile && tile->tileType() != typeToFill)
        {
            break;
        }
    }
    up++;
    down--;

    FillLine(x, up, down, action, map);

    /* Store it in the list of flooded segments */
    c = x;
    p = FLOOD_LINE(c);

    if (p->flags)
    {
        while (p->next)
        {
            c = p->next;
            p = FLOOD_LINE(c);
        }

        flood_buf.reserve(flood_count);
        c = flood_count++;
        {
            Flood_Line p;
            p.flags = KFlood_Nonoe;
            p.upos = SHRT_MAX;
            p.dpos = SHRT_MIN;
            p.x = c;
            p.next = 0;
            flood_buf << p;
        }
        p = FLOOD_LINE(c);
    }

    p->flags = KFlood_In;
    p->upos = up;
    p->dpos = down;
    p->x = x;
    p->next = 0;

    if (x > 0)
        p->flags |= KFlood_Up;

    if (x + 1 < map.cols())
        p->flags |= KFlood_Down;

    return down + 2;
}

/*
 *  Checks a line segment, using the scratch buffer is to store a list of
 *  segments which have already been drawn in order to minimise the required
 *  number of tests.
 */
static bool CheakFloodLine(
  int x, int up, int down, const QString & typeToFill, QAction * action, MapBase & map)
{

    int c;
    Flood_Line * p;
    int ret = false;

    while (up <= down)
    {
        c = x;

        for (;;)
        {
            p = FLOOD_LINE(c);

            if ((up >= p->upos) && (up <= p->dpos))
            {
                up = p->dpos + 2;
                break;
            }

            c = p->next;

            if (!c)
            {
                up = Flooder(x, up, typeToFill, action, map);
                ret = true;
                break;
            }
            else
            {
                c = p->next;
            }
        }
    }

    return ret;
}

} // namespace FloodFill

void FloodFill::floodFill(TrackTile & tile, QAction * action, const QString & typeToFill, MapBase & map)
{
    flood_buf.clear();
    flood_count = map.rows();
    flood_buf.reserve(flood_count);

    Flood_Line p;
    p.flags = KFlood_Nonoe;
    p.upos = SHRT_MAX;
    p.dpos = SHRT_MIN;
    p.next = 0;
    for (int c = 0; c < flood_count; c++)
    {
        p.x = c;
        flood_buf << p;
    }

    QPoint location = tile.matrixLocation();
    Flooder(location.x(), location.y(), typeToFill, action, map);

    bool done;
    Flood_Line * line;
    do
    {
        done = true;

        for (int c = 0; c < flood_count; c++)
        {

            line = FLOOD_LINE(c);

            // right
            if (line->flags & KFlood_Down)
            {
                line->flags &= ~KFlood_Down;

                if (CheakFloodLine(
                      line->x + 1, line->upos, line->dpos, typeToFill, action, map))
                {
                    done = false;
                    line = FLOOD_LINE(c);
                }
            }

            // left
            if (line->flags & KFlood_Up)
            {
                line->flags &= ~KFlood_Up;
                if (CheakFloodLine(line->x - 1, line->upos, line->dpos, typeToFill, action, map))
                {
                    done = false;
                    // Special case shortcut for going backwards
                    if ((c > 0) && (map.rows()))
                        c -= 2;
                }
            }
        }
    } while (!done);
}
