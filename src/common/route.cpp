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

#include "tracktile.h"
#include "route.h"

#include <algorithm>

void Route::clear()
{
    for (TrackTile * tile : m_route)
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

            int deltaX = tile->matrixLocation().x() - prev->matrixLocation().x();
            int deltaY = tile->matrixLocation().y() - prev->matrixLocation().y();

            // Equal X-coordinates?
            if (deltaX == 0)
            {
                if (deltaY > 0)
                {
                    prev->setRouteDirection(TrackTile::RD_DOWN);
                }
                else
                {
                    prev->setRouteDirection(TrackTile::RD_UP);
                }

                okToAdd = true;
            }
            // Equal Y-coordinates?
            else if (deltaY == 0)
            {
                if (deltaX > 0)
                {
                    prev->setRouteDirection(TrackTile::RD_RIGHT);
                }
                else
                {
                    prev->setRouteDirection(TrackTile::RD_LEFT);
                }

                okToAdd = true;
            }
            // Segment at a 45 degree angle?
            // Possible only if not the starting segment which
            // can be only at a straight angle.
            else if (m_route.size() > 1)
            {
                if (std::abs(deltaX) == std::abs(deltaY))
                {
                    if (deltaX > 0)
                    {
                        if (deltaY > 0)
                        {
                            // TODO: Is this needed?
                            prev->setRouteDirection(TrackTile::RD_DOWN_RIGHT);
                        }
                        else
                        {
                            // TODO: Is this needed?
                            prev->setRouteDirection(TrackTile::RD_UP_RIGHT);
                        }

                        okToAdd = true;
                    }
                    else
                    {
                        if (deltaY > 0)
                        {
                            // TODO: Is this needed?
                            prev->setRouteDirection(TrackTile::RD_DOWN_LEFT);
                        }
                        else
                        {
                            // TODO: Is this needed?
                            prev->setRouteDirection(TrackTile::RD_UP_LEFT);
                        }

                        okToAdd = true;
                    }
                }
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

    return nullptr;
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

        for (TrackTile * tile : routeVector)
        {
            if (tile->routeIndex() >= 0)
            {
                push(tile);
            }
        }

        push(routeVector.at(0));
    }
}
