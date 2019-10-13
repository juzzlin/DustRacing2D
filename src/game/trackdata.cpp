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

#include "trackdata.hpp"
#include "tracktile.hpp"

TrackData::TrackData(QString name, bool isUserTrack, unsigned int cols, unsigned int rows)
  : TrackDataBase(name, isUserTrack)
  , m_map(cols, rows)
  , m_route()
  , m_isLocked(false)
{
}

QString TrackData::fileName() const
{
    return m_fileName;
}

void TrackData::setFileName(QString newFileName)
{
    m_fileName = newFileName;
}

Route & TrackData::route()
{
    return m_route;
}

const Route & TrackData::route() const
{
    return m_route;
}

MapBase & TrackData::map()
{
    return m_map;
}

const MapBase & TrackData::map() const
{
    return m_map;
}

Objects & TrackData::objects()
{
    return m_objects;
}

const Objects & TrackData::objects() const
{
    return m_objects;
}

bool TrackData::isLocked() const
{
    return m_isLocked;
}

void TrackData::setIsLocked(bool locked)
{
    m_isLocked = locked;
}

TrackData::~TrackData()
{
}
