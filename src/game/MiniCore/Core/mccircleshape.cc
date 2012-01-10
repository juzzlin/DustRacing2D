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
#include "mccircleshapeimpl.hh"
#include "mcshapeview.hh"
#include "mcobject.hh"

MCUint MCCircleShapeImpl::typeID = MCShape::registerType();

MCCircleShapeImpl::MCCircleShapeImpl(MCObject & parent, MCFloat radius)
: MCShapeImpl(parent)
, radius(radius)
, momentOfInertiaFactor(radius * radius / 2)
{}

MCCircleShapeImpl::~MCCircleShapeImpl()
{}

MCCircleShape::MCCircleShape(
    MCObject & parent, MCShapeView * pView, MCFloat newRadius)
: MCShape(parent, pView)
, m_pImpl(new MCCircleShapeImpl(parent, newRadius))
{}

MCFloat MCCircleShape::radius() const
{
    return m_pImpl->radius;
}

void MCCircleShape::setRadius(MCFloat r)
{
    m_pImpl->radius = r;
    m_pImpl->momentOfInertiaFactor = r * r / 2;
}

MCFloat MCCircleShape::momentOfInertia() const
{
    return m_pImpl->momentOfInertiaFactor * parent().mass();
}

bool MCCircleShape::contains(const MCVector2d<MCFloat> & p) const
{
    return (MCVector2d<MCFloat>(location()) - p).lengthFast() <= radius();
}

int MCCircleShape::interpenetrationDepth(const MCSegment<MCFloat> & p) const
{
    return radius() - (MCVector2d<MCFloat>(location()) - p.vertex0).lengthFast();
}

MCVector2d<MCFloat> MCCircleShape::contactNormal(const MCSegment<MCFloat> & p) const
{
    return (p.vertex0 - MCVector2d<MCFloat>(location())).normalizedFast();
}

MCUint MCCircleShape::typeID()
{
    return MCCircleShapeImpl::typeID;
}

MCUint MCCircleShape::instanceTypeID() const
{
    return MCCircleShapeImpl::typeID;
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
