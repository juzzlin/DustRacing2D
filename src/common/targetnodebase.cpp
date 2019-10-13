// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "targetnodebase.hpp"
#include "tracktilebase.hpp"

TargetNodeBase::TargetNodeBase()
  : m_size(QSize(TrackTileBase::TILE_H, TrackTileBase::TILE_W))
  , m_index(-1)
{
}

void TargetNodeBase::setNext(TargetNodeBasePtr tnode)
{
    m_next = tnode;
}

TargetNodeBasePtr TargetNodeBase::next() const
{
    return m_next;
}

void TargetNodeBase::setPrev(TargetNodeBasePtr tnode)
{
    m_prev = tnode;
}

TargetNodeBasePtr TargetNodeBase::prev() const
{
    return m_prev;
}

QPointF TargetNodeBase::location() const
{
    return m_location;
}

void TargetNodeBase::setLocation(QPointF newLocation)
{
    m_location = newLocation;
}

int TargetNodeBase::index() const
{
    return m_index;
}

void TargetNodeBase::setIndex(int index)
{
    m_index = index;
}

void TargetNodeBase::setSize(QSizeF size)
{
    m_size = size;
}

QSizeF TargetNodeBase::size() const
{
    return m_size;
}
