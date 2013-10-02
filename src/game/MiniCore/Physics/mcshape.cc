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

#include <cassert>

MCUint MCShape::m_typeCount = 0;

MCShape::MCShape(MCShapeViewPtr view)
: m_pParent(nullptr)
, m_angle(0)
{
    if (view)
    {
        setView(view);
    }

    // Set default shadow offset
    setShadowOffset(MCVector2dF(2, -2));
}

void MCShape::setParent(MCObject & parent)
{
    m_pParent = &parent;
}

MCObject & MCShape::parent() const
{
    assert(m_pParent);
    return *m_pParent;
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
        m_view->renderShadow(m_location + MCVector3dF(m_shadowOffset), m_angle, p);
    }
}

void MCShape::renderScaled(MCFloat w, MCFloat h, MCCamera * p)
{
    if (m_view)
    {
        m_view->renderScaled(m_location, m_angle, w, h, p);
    }
}

void MCShape::renderShadowScaled(MCFloat w, MCFloat h, MCCamera * p)
{
    if (m_view)
    {
        m_view->renderShadowScaled(
            m_location + MCVector3dF(m_shadowOffset), m_angle, w, h, p);
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

void MCShape::setShadowOffset(const MCVector2dF & p)
{
    m_shadowOffset = p;
}

void MCShape::rotate(MCFloat newAngle)
{
    m_angle = newAngle;
}

MCFloat MCShape::angle() const
{
    return m_angle;
}
