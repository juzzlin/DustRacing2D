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

#ifndef WORLD_HPP
#define WORLD_HPP

#include "listener.hpp"
#include "location.hpp"
#include "source.hpp"

#include <vector>

namespace STFH {

class World
{
public:

    World();

    virtual ~World();

    virtual void addListener(ListenerPtr listener);

    virtual void removeListeners();

    virtual void addSource(SourcePtr source);

    virtual void removeSources();

    virtual void setDimensions(float x1, float y1, float x2, float y2);

    virtual void updateVolumes();

private:

    Location getAverageListenerLocation();

    std::vector<SourcePtr> m_sources;
    std::vector<ListenerPtr> m_listeners;

    float m_x1, m_y1, m_x2, m_y2;

    Location m_averageLocation;
};

} // STFH

#endif // WORLD_HPP
