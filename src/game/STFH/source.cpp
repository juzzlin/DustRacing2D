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

#include "source.hpp"

namespace STFH {

Source::Source()
  : m_pitch(1.0)
  , m_volume(1.0)
  , m_maxDist(0.0)
  , m_refDist(0.0)
{
}

void Source::setData(DataPtr data)
{
    m_data = data;
}

void Source::setVolume(float volume)
{
    m_volume = volume;
}

float Source::volume() const
{
    return m_volume;
}

void Source::setPitch(float pitch)
{
    m_pitch = pitch;
}

float Source::pitch() const
{
    return m_pitch;
}

void Source::setLocation(const Location & location)
{
    m_location = location;
}

const Location & Source::location() const
{
    return m_location;
}

void Source::setMaxDist(float maxDist)
{
    m_maxDist = maxDist;
}

void Source::setReferenceDist(float refDist)
{
    m_refDist = refDist;
}

Source::~Source()
{
}

} // namespace STFH
