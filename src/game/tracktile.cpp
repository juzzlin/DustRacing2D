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

#include "tracktile.hpp"
#include "trackdata.hpp"

TrackTile::TrackTile(
    TrackData & trackData,
    QPointF location,
    QPoint matrixLocation,
    const QString & type,
    TrackTile::TileType typeEnum)
: TrackTileBase(trackData, location, matrixLocation, type)
, m_rotation(0)
, m_surface(nullptr)
, m_previewSurface(nullptr)
, m_typeEnum(typeEnum)
, m_hasAsphalt(false)
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

void TrackTile::setPreviewSurface(MCSurface * surface)
{
    m_previewSurface = surface;
}

MCSurface * TrackTile::previewSurface() const
{
    return m_previewSurface;
}

TrackTile::TileType TrackTile::tileTypeEnum() const
{
    return m_typeEnum;
}

void TrackTile::setTileTypeEnum(TrackTile::TileType type)
{
    m_typeEnum = type;

    switch (type)
    {
    case TT_CORNER_90:
    case TT_CORNER_45_LEFT:
    case TT_CORNER_45_RIGHT:
    case TT_STRAIGHT:
    case TT_STRAIGHT_45_MALE:
    case TT_STRAIGHT_45_FEMALE:
    case TT_SAND_GRASS_STRAIGHT:
    case TT_FINISH:
        m_hasAsphalt = true;
        break;
    default:
        m_hasAsphalt = false;
        break;
    }
}

bool TrackTile::hasAsphalt() const
{
    return m_hasAsphalt;
}

TrackTile::~TrackTile()
{
}
