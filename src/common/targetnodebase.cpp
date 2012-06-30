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

#include "targetnodebase.hpp"

TargetNodeBase::TargetNodeBase()
: m_index(-1)
, m_next(nullptr)
, m_prev(nullptr)
{
}

void TargetNodeBase::setNext(TargetNodeBase * tnode)
{
    m_next = tnode;
}

TargetNodeBase * TargetNodeBase::next() const
{
    return m_next;
}

void TargetNodeBase::setPrev(TargetNodeBase * tnode)
{
    m_prev = tnode;
}

TargetNodeBase * TargetNodeBase::prev() const
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
