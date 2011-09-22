// This file is part of Dust Rallycross (DustRAC).
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

#include "tracktile.h"
#include "route.h"

#include <algorithm>

void Route::clear()
{
    Q_FOREACH (TrackTile * tile, m_route)
    {
        tile->setRouteIndex(-1);
    }

    m_route.clear();
}

int Route::push(TrackTile * tile)
{
    bool routeClosed = false;
    if (m_route.size() && tile->routeIndex() == 0)
    {
        routeClosed = true;
    }

    if (!m_route.contains(tile) || routeClosed)
    {
        bool okToAdd = false;

        if (m_route.size())
        {
            TrackTile * prev = m_route.back();

            if (prev->matrixLocation().x() == tile->matrixLocation().x())
            {
                if (prev->matrixLocation().y() < tile->matrixLocation().y())
                {
                    prev->setRouteDirection(TrackTile::RD_DOWN);
                }
                else
                {
                    prev->setRouteDirection(TrackTile::RD_UP);
                }

                okToAdd = true;
            }
            else if (prev->matrixLocation().y() == tile->matrixLocation().y())
            {
                if (prev->matrixLocation().x() < tile->matrixLocation().x())
                {
                    prev->setRouteDirection(TrackTile::RD_RIGHT);
                }
                else
                {
                    prev->setRouteDirection(TrackTile::RD_LEFT);
                }

                okToAdd = true;
            }
        }
        else
        {
            okToAdd = true;
        }

        if (okToAdd)
        {
            if (!routeClosed)
            {
                tile->setRouteIndex(m_route.size());
                m_route.append(tile);
            }
        }
    }

    return tile->routeIndex();
}

unsigned int Route::length() const
{
    return m_route.size();
}

TrackTile * Route::get(unsigned int index) const
{
    if (index < length())
        return m_route[index];

    return NULL;
}

struct mySortFunc
{
    bool operator () (TrackTile * lhs, TrackTile * rhs)
    {
        return lhs->routeIndex() < rhs->routeIndex();
    }
};

void Route::buildFromVector(QVector<TrackTile *> routeVector)
{
    if (routeVector.size())
    {
        clear();

        std::sort(routeVector.begin(), routeVector.end(), mySortFunc());

        Q_FOREACH(TrackTile * tile, routeVector)
        {
            if (tile->routeIndex() >= 0)
            {
                push(tile);
            }
        }

        push(routeVector.at(0));
    }
}
