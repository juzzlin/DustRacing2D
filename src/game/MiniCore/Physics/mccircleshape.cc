// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#include "mccircleshape.hh"
#include "mcshapeview.hh"
#include "mcobject.hh"

MCUint MCCircleShape::m_typeID = MCShape::registerType();

MCCircleShape::MCCircleShape(MCShapeView * pView, MCFloat radius)
: MCShape(pView)
, m_radius(radius)
{}

MCFloat MCCircleShape::radius() const
{
    return m_radius;
}

void MCCircleShape::setRadius(MCFloat r)
{
    m_radius = r;
}

bool MCCircleShape::contains(const MCVector2d<MCFloat> & p) const
{
    return (MCVector2d<MCFloat>(location()) - p).lengthFast() <= radius();
}

int MCCircleShape::interpenetrationDepth(
    const MCSegment<MCFloat> & p, MCVector2dF & contactNormal) const
{
    contactNormal = this->contactNormal(p);
    return radius() - (MCVector2d<MCFloat>(location()) - p.vertex0).lengthFast();
}

MCVector2d<MCFloat> MCCircleShape::contactNormal(const MCSegment<MCFloat> & p) const
{
    return (p.vertex0 - MCVector2d<MCFloat>(location())).normalizedFast();
}

MCUint MCCircleShape::typeID()
{
    return m_typeID;
}

MCUint MCCircleShape::instanceTypeID() const
{
    return m_typeID;
}

MCBBox<MCFloat> MCCircleShape::bbox() const
{
    return MCBBox<MCFloat>(
        MCVector2d<MCFloat>(
            location()) - MCVector2d<MCFloat>(radius(), radius()),
            radius() * 2, radius() * 2);
}

MCCircleShape::~MCCircleShape()
{}
