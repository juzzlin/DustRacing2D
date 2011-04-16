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

#include "route.h"
#include "tracktile.h"

void Route::clear()
{
    Q_FOREACH (TrackTile * tile, m_route)
    {
        tile->setRouteIndex(-1);
        tile->update();
    }

    m_route.clear();
}

int Route::push(TrackTile * tile)
{
    if (!m_route.contains(tile))
    {
        tile->setRouteIndex(m_route.size());
        m_route.append(tile);
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
