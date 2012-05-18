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

#include "trackdata.hpp"
#include "tracktile.hpp"

TrackData::TrackData(QString name, unsigned int cols, unsigned int rows)
: m_name(name)
, m_map(*this, cols, rows)
, m_route()
{}

QString TrackData::name() const
{
    return m_name;
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

void TrackData::enlargeCanvas()
{
    m_map.resize(m_map.cols() + 1, m_map.rows() + 1);
}

TrackData::~TrackData()
{
}
