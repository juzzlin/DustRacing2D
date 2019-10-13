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

#include "trackdatabase.hpp"

TrackDataBase::TrackDataBase(QString name, bool isUserTrack)
  : m_name(name)
  , m_index(999)
  , m_isUserTrack(isUserTrack)
{
}

TrackDataBase::TrackDataBase(const TrackDataBase & other)
  : m_name(other.m_name)
  , m_index(other.m_index)
  , m_isUserTrack(other.m_isUserTrack)
{
}

QString TrackDataBase::name() const
{
    return m_name;
}

bool TrackDataBase::isUserTrack() const
{
    return m_isUserTrack;
}

unsigned int TrackDataBase::index() const
{
    return m_index;
}

void TrackDataBase::setName(QString name)
{
    m_name = name;
}

void TrackDataBase::setUserTrack(bool isUserTrack)
{
    m_isUserTrack = isUserTrack;
}

void TrackDataBase::setIndex(unsigned int index)
{
    m_index = index;
}

TrackDataBase::~TrackDataBase()
{
}
