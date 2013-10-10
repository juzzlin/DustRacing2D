// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#include "world.hpp"

namespace STFH {

World::World()
    : m_x1(0)
    , m_y1(0)
    , m_x2(0)
    , m_y2(0)
{
}

void World::addListener(ListenerPtr listener)
{
    m_listeners.push_back(listener);
}

void World::addSource(SourcePtr source)
{
    m_sources.push_back(source);
}

void World::setDimensions(float x1, float y1, float x2, float y2)
{
    m_x1 = x1;
    m_y1 = y1;
    m_x2 = x2;
    m_y2 = y2;
}

void World::updateVolumes()
{
}

Location World::getAverageListenerLocation()
{
    float x = 0;
    float y = 0;

    const int listenerCount = m_listeners.size();
    if (listenerCount)
    {
        for (auto listener : m_listeners)
        {
            x += listener->location().x();
            y += listener->location().y();
        }

        x /= listenerCount;
        y /= listenerCount;

        return Location(x, y);
    }

    return Location();
}

World::~World()
{
}

} // STFH
