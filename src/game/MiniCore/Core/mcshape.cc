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
#include "mcshapeimpl.hh"
#include "mcshapeview.hh"
#include "mccamera.hh"

MCUint MCShapeImpl::m_typeCount = 0;

MCShapeImpl::MCShapeImpl(MCObject & parent)
: m_parent(parent)
, m_angle(0)
{}

MCShapeImpl::~MCShapeImpl()
{}

MCShape::MCShape(MCObject & parent, MCShapeView * pView)
: m_pImpl(new MCShapeImpl(parent))
{
    if (pView) {
        setView(pView);
    }

    // Set default shadow offset
    setShadowOffset(MCVector2d<MCFloat>(2, -2));
}

MCObject & MCShape::parent() const
{
    return m_pImpl->m_parent;
}

MCUint MCShape::registerType()
{
    return ++MCShapeImpl::m_typeCount;
}

MCShape::~MCShape()
{
    delete m_pImpl;
}

void MCShape::setView(MCShapeView * p)
{
    m_pImpl->m_pView.reset(p);
}

void MCShape::setView(std::shared_ptr<MCShapeView> p)
{
    m_pImpl->m_pView = p;
}

MCShapeView * MCShape::view() const
{
    return m_pImpl->m_pView.get();
}

void MCShape::render(MCCamera * p)
{
    if (m_pImpl->m_pView) {
        m_pImpl->m_pView->render(m_pImpl->m_location, m_pImpl->m_angle, p);
    }
}

void MCShape::renderShadow(MCCamera * p)
{
    if (m_pImpl->m_pView) {
        m_pImpl->m_pView->renderShadow(
            m_pImpl->m_location + MCVector3d<MCFloat>(m_pImpl->m_shadowOffset),
            m_pImpl->m_angle, p);
    }
}

void MCShape::renderScaled(MCFloat wr, MCFloat hr, MCCamera * p)
{
    if (m_pImpl->m_pView) {
        m_pImpl->m_pView->renderScaled(m_pImpl->m_location,
            m_pImpl->m_angle, wr, hr, p);
    }
}

void MCShape::renderShadowScaled(MCFloat wr, MCFloat hr, MCCamera * p)
{
    if (m_pImpl->m_pView) {
        m_pImpl->m_pView->renderShadowScaled(
            m_pImpl->m_location + MCVector3d<MCFloat>(m_pImpl->m_shadowOffset),
            m_pImpl->m_angle, wr, hr, p);
    }
}

void MCShape::translate(const MCVector3d<MCFloat> & p)
{
    m_pImpl->m_location = p;
}

const MCVector3d<MCFloat> & MCShape::location() const
{
    return m_pImpl->m_location;
}

void MCShape::setShadowOffset(const MCVector2d<MCFloat> & p)
{
    m_pImpl->m_shadowOffset = p;
}

void MCShape::rotate(MCFloat newAngle)
{
    m_pImpl->m_angle = newAngle;
}

MCFloat MCShape::angle() const
{
    return m_pImpl->m_angle;
}
