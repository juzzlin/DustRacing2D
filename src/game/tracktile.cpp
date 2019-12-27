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

TrackTile::TrackTile(
  QPointF location,
  QPoint matrixLocation,
  const QString & type,
  TrackTile::TileType typeEnum)
  : TrackTileBase(location, matrixLocation, type)
  , m_rotation(0)
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

void TrackTile::setSurface(MCSurfacePtr surface)
{
    m_surface = surface;
}

MCSurfacePtr TrackTile::surface() const
{
    return m_surface;
}

void TrackTile::setPreviewSurface(MCSurfacePtr surface)
{
    m_previewSurface = surface;
}

MCSurfacePtr TrackTile::previewSurface() const
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
    case TileType::Bridge:
    case TileType::Corner90:
    case TileType::Corner45Left:
    case TileType::Corner45Right:
    case TileType::Straight:
    case TileType::Straight45Male:
    case TileType::Straight45Female:
    case TileType::Finish:
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
