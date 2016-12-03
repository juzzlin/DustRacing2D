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

#include "mcshape.hh"
#include "mccamera.hh"

MCUint MCShape::m_typeCount = 0;

MCVector3dF MCShape::m_defaultShadowOffset = MCVector3dF(2, -2, 0.5f);

MCShape::MCShape(MCShapeViewPtr view)
    : m_parent(nullptr)
    , m_angle(0)
    , m_radius(0)
{
    if (view)
    {
        setView(view);
    }

    setShadowOffset(MCShape::m_defaultShadowOffset);
}

void MCShape::setParent(MCObject & parent)
{
    m_parent = &parent;
}

MCObject & MCShape::parent() const
{
    return *m_parent;
}

MCUint MCShape::registerType()
{
    return ++MCShape::m_typeCount;
}

MCShape::~MCShape()
{
}

void MCShape::setView(MCShapeViewPtr view)
{
    m_view = view;
}

MCShapeViewPtr MCShape::view() const
{
    return m_view;
}

void MCShape::render(MCCamera * p)
{
    if (m_view)
    {
        m_view->render(m_location, m_angle, p);
    }
}

void MCShape::renderShadow(MCCamera * p)
{
    if (m_view)
    {
        const MCVector3dF shadowLocation(
            m_shadowOffset.i() + m_location.i(),
            m_shadowOffset.j() + m_location.j(),
            m_shadowOffset.k()
        );

        m_view->renderShadow(shadowLocation, m_angle, p);
    }
}

void MCShape::translate(const MCVector3dF & p)
{
    m_location = p;
}

const MCVector3dF & MCShape::location() const
{
    return m_location;
}

void MCShape::setShadowOffset(const MCVector3dF & p)
{
    m_shadowOffset = p;
}

void MCShape::setDefaultShadowOffset(const MCVector3dF & p)
{
    MCShape::m_defaultShadowOffset = p;
}

const MCVector3dF & MCShape::shadowOffset() const
{
    return m_shadowOffset;
}

void MCShape::rotate(MCFloat newAngle)
{
    m_angle = newAngle;
}

MCFloat MCShape::angle() const
{
    return m_angle;
}

MCFloat MCShape::radius() const
{
    return m_radius;
}

void MCShape::setRadius(MCFloat radius)
{
    m_radius = radius;
}

bool MCShape::mayIntersect(MCShape & other)
{
    return !(m_location.i() + m_radius < other.m_location.i() - other.m_radius ||
        m_location.j() + m_radius < other.m_location.j() - other.m_radius ||
        m_location.i() - m_radius > other.m_location.i() + other.m_radius ||
        m_location.j() - m_radius > other.m_location.j() + other.m_radius);
}
