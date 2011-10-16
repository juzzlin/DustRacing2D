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

TrackTile::TrackTile(TrackData * trackData,
                     QPointF location, QPoint matrixLocation,
                     const QString & type)
: TrackTileBase(trackData, location, matrixLocation, type)
, m_rotation(0)
, m_surface(nullptr)
{
}

void TrackTile::setRotation(int rotation)
{
    m_rotation = rotation;
}

int TrackTile::rotation() const
{
    return m_rotation;
}

void TrackTile::setSurface(MCSurface * surface)
{
    m_surface = surface;
}

MCSurface * TrackTile::surface() const
{
    return m_surface;
}

TrackTile::~TrackTile()
{
}
