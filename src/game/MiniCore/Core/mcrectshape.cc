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

#include "mcrectshape.hh"
#include "mcrectshapeimpl.hh"
#include "mccamera.hh"
#include "mcobject.hh"
#include "mcray.hh"
#include "mcshapeview.hh"
#include "mcmathutil.hh"

#include <cassert>

#include <GL/gl.h>

MCUint MCRectShapeImpl::typeID = MCShape::registerType();

MCRectShapeImpl::MCRectShapeImpl(
    MCObject & parent, MCFloat width, MCFloat height)
: MCShapeImpl(parent)
, obbox(width / 2, height / 2, MCVector2d<MCFloat>())
, momentOfInertiaFactor((width * width + height * height) / 12)
, radius(std::max(width, height) / 2)
{}

MCRectShapeImpl::~MCRectShapeImpl()
{}

MCEdge<MCFloat> MCRectShapeImpl::edgeForSegment(const MCSegment<MCFloat> & p) const
{
    const MCSegment<MCFloat> s0s1(obbox.vertex(0), obbox.vertex(1));
    const MCSegment<MCFloat> s1s2(obbox.vertex(1), obbox.vertex(2));
    const MCSegment<MCFloat> s2s3(obbox.vertex(2), obbox.vertex(3));
    const MCSegment<MCFloat> s3s0(obbox.vertex(3), obbox.vertex(0));

    if (MCMathUtil::crosses(p, s0s1))
    {
        return MCEdge<MCFloat>(obbox.vertex(1) - obbox.vertex(0), obbox.vertex(0));
    }
    else if (MCMathUtil::crosses(p, s1s2))
    {
        return MCEdge<MCFloat>(obbox.vertex(2) - obbox.vertex(1), obbox.vertex(1));
    }
    else if (MCMathUtil::crosses(p, s2s3))
    {
        return MCEdge<MCFloat>(obbox.vertex(3) - obbox.vertex(2), obbox.vertex(2));
    }
    else
    {
        return MCEdge<MCFloat>(obbox.vertex(0) - obbox.vertex(3), obbox.vertex(3));
    }
}

int MCRectShapeImpl::interpenetrationDepth(const MCSegment<MCFloat> & p) const
{
    MCEdge<MCFloat> edge(edgeForSegment(p));
    return MCMathUtil::distanceFromVector(p.vertex0 - edge.origin, edge.edge);
}

MCRectShape::MCRectShape(
    MCObject & parent, MCShapeView * pView, MCFloat width, MCFloat height)
: MCShape(parent, pView)
, m_pImpl(new MCRectShapeImpl(parent, width, height))
{}

MCEdge<MCFloat> MCRectShape::edgeForSegment(const MCSegment<MCFloat> & p) const
{
    return m_pImpl->edgeForSegment(p);
}

void MCRectShape::translate(const MCVector3d<MCFloat> & p)
{
    MCShape::translate(p);
    m_pImpl->obbox.translate(p);
}

void MCRectShape::rotate(MCUint a)
{
    MCShape::rotate(a);
    m_pImpl->obbox.rotate(a);
}

MCBBox<MCFloat> MCRectShape::bbox() const
{
    return m_pImpl->obbox.bbox();
}

MCFloat MCRectShape::momentOfInertia() const
{
    return m_pImpl->momentOfInertiaFactor * parent().mass();
}

bool MCRectShape::contains(const MCVector2d<MCFloat> & p) const
{
    return m_pImpl->obbox.contains(p);
}

int MCRectShape::interpenetrationDepth(const MCSegment<MCFloat> & p) const
{
    return m_pImpl->interpenetrationDepth(p);
}

MCVector2d<MCFloat> MCRectShapeImpl::contactNormal(
    const MCSegment<MCFloat> & p) const
{
    // Get the crossing edge for p and build a normal vector from it
    static const MCVector3d<MCFloat> DOWN(0, 0, -1);
    MCVector3d<MCFloat> normal(
        MCVector3d<MCFloat>(edgeForSegment(p).edge) * DOWN);
    return MCVector2d<MCFloat>(normal).normalizedFast();
}

void MCRectShapeImpl::renderShapeOutline(MCCamera * pCamera)
{
    // This code is shitty and only for debug purposes.

    glPushAttrib(GL_ENABLE_BIT);

    for (int i = 0; i < 4; i++) {
        int x = obbox.vertex(i).i();
        int y = obbox.vertex(i).j();

        if (pCamera) {
            pCamera->mapToCamera(x, y);
        }

        glPointSize(4);
        glPushMatrix();
        glTranslated(x, y, 0.0f);
        glColor4f(1.0, 0.0, 0.0, 1.0);
        glBegin(GL_POINTS);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glEnd();
        glPopMatrix();
    }

    glPopAttrib();
}

MCVector2d<MCFloat> MCRectShape::contactNormal(
    const MCSegment<MCFloat> & p) const
{
    return m_pImpl->contactNormal(p);
}

MCUint MCRectShape::typeID()
{
    return MCRectShapeImpl::typeID;
}

MCUint MCRectShape::instanceTypeID() const
{
    return MCRectShapeImpl::typeID;
}

const MCOBBox<MCFloat> & MCRectShape::obbox() const
{
    return m_pImpl->obbox;
}

void MCRectShape::resize(MCFloat width, MCFloat height)
{
    m_pImpl->obbox =
        MCOBBox<MCFloat>(width / 2, height / 2, location());
    m_pImpl->obbox.rotate(angle());
    m_pImpl->momentOfInertiaFactor = (width * width + height * height) / 12;
}

void MCRectShape::render(MCCamera * pCamera)
{
    MCShape::render(pCamera);

    // Render outline for debug purposes
    if (parent().renderShapeOutline()) {
        m_pImpl->renderShapeOutline(pCamera);
    }
}

void MCRectShape::renderScaled(MCFloat wr, MCFloat hr, MCCamera * pCamera)
{
    MCShape::renderScaled(wr, hr, pCamera);

    // Render outline for debug purposes
    if (parent().renderShapeOutline()) {
        m_pImpl->renderShapeOutline(pCamera);
    }
}

MCFloat MCRectShape::radius() const
{
    return m_pImpl->radius;
}

MCRectShape::~MCRectShape()
{
    delete m_pImpl;
}
