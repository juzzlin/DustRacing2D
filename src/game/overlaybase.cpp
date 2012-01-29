// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "overlaybase.hpp"

OverlayBase::OverlayBase()
  : m_width(0)
  , m_height(0)
{
}

void OverlayBase::setDimensions(MCUint width, MCUint height)
{
    m_width = width;
    m_height = height;
}

MCUint OverlayBase::width() const
{
    return m_width;
}

MCUint OverlayBase::height() const
{
    return m_height;
}

OverlayBase::~OverlayBase()
{
}
