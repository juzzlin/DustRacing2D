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
#include "mccamera.hh"
#include "mcobject.hh"
#include "mcmathutil.hh"

#include <cmath>

MCUint MCRectShape::m_typeID = MCShape::registerType();

MCRectShape::MCRectShape(MCShapeViewPtr view, MCFloat width, MCFloat height)
: MCShape(view)
, m_obbox(width / 2, height / 2, MCVector2dF())
, m_width(width)
, m_height(height)
{
    setRadius(std::sqrt(width * width + height * height) / 2);
}

MCEdgeF MCRectShape::edgeForSegment(const MCSegmentF & p) const
{
    // **** Try first a crossing lines method ****

    const MCSegmentF s0s1(m_obbox.vertex(0), m_obbox.vertex(1));
    const MCSegmentF s1s2(m_obbox.vertex(1), m_obbox.vertex(2));
    const MCSegmentF s2s3(m_obbox.vertex(2), m_obbox.vertex(3));
    const MCSegmentF s3s0(m_obbox.vertex(3), m_obbox.vertex(0));

    if (MCMathUtil::crosses(p, s0s1))
    {
        return MCEdgeF(m_obbox.vertex(1) - m_obbox.vertex(0), m_obbox.vertex(0));
    }
    else if (MCMathUtil::crosses(p, s1s2))
    {
        return MCEdgeF(m_obbox.vertex(2) - m_obbox.vertex(1), m_obbox.vertex(1));
    }
    else if (MCMathUtil::crosses(p, s2s3))
    {
        return MCEdgeF(m_obbox.vertex(3) - m_obbox.vertex(2), m_obbox.vertex(2));
    }
    else if (MCMathUtil::crosses(p, s3s0))
    {
        return MCEdgeF(m_obbox.vertex(0) - m_obbox.vertex(3), m_obbox.vertex(3));
    }

    // **** Sector method ****

    const MCVector2dF l(m_obbox.location());

    // Translate test point to obbox's coordinates
    const MCVector2dF x(p.vertex0 - l);

    // Cache vertices
    const MCVector2dF v0(m_obbox.vertex(0));
    const MCVector2dF v1(m_obbox.vertex(1));

    // Translate vertices to obbox's coordinates
    MCVector2dF a(v0 - l);
    MCVector2dF b(v1 - l);

    // **** Find the corresponding edge  ****

    if (x % a > 0 && b % x > 0)
    {
        return MCEdgeF(v1 - v0, v0);
    }

    const MCVector2dF v2(m_obbox.vertex(2));

    a = b;
    b = v2 - l;
    if (x % a > 0 && b % x > 0)
    {
        return MCEdgeF(v2 - v1, v1);
    }

    const MCVector2dF v3(m_obbox.vertex(3));

    a = b;
    b = v3 - l;
    if (x % a > 0 && b % x > 0)
    {
        return MCEdgeF(v3 - v2, v2);
    }

    return MCEdgeF(v0 - v3, v3);
}

void MCRectShape::translate(const MCVector3dF & p)
{
    MCShape::translate(p);
    m_obbox.translate(p);
}

void MCRectShape::rotate(MCFloat a)
{
    MCShape::rotate(a);
    m_obbox.rotate(a);
}

MCBBox<MCFloat> MCRectShape::bbox() const
{
    return m_obbox.bbox();
}

bool MCRectShape::contains(const MCVector2dF & point) const
{
    return m_obbox.contains(point);
}

MCFloat MCRectShape::interpenetrationDepth(const MCSegmentF & segment, MCVector2dF & contactNormal) const
{
    MCEdgeF edge(edgeForSegment(segment));
    contactNormal = this->contactNormal(segment, edge);
    return MCMathUtil::distanceFromVector(segment.vertex0 - edge.origin, edge.edge);
}

MCVector2dF MCRectShape::contactNormal(const MCSegmentF & segment) const
{
    // Get the crossing edge for p and build a normal vector from it
    static const MCVector3dF DOWN(0, 0, -1);
    MCVector3dF normal(MCVector3dF(edgeForSegment(segment).edge) % DOWN);
    return MCVector2dF(normal).normalized();
}

MCVector2dF MCRectShape::contactNormal(const MCSegmentF &, const MCEdgeF & edge) const
{
    // Get the crossing edge for p and build a normal vector from it
    static const MCVector3dF DOWN(0, 0, -1);
    MCVector3dF normal(MCVector3dF(edge.edge) % DOWN);
    return MCVector2dF(normal).normalized();
}

MCUint MCRectShape::typeID()
{
    return m_typeID;
}

MCUint MCRectShape::instanceTypeID() const
{
    return m_typeID;
}

const MCOBBoxF & MCRectShape::obbox() const
{
    return m_obbox;
}

void MCRectShape::resize(MCFloat width, MCFloat height)
{
    m_obbox = MCOBBoxF(width / 2, height / 2, location());
    m_obbox.rotate(angle());
    m_width = width;
    m_height = height;
}

void MCRectShape::render(MCCamera * camera)
{
    MCShape::render(camera);
}

MCFloat MCRectShape::width() const
{
    return m_width;
}

MCFloat MCRectShape::height() const
{
    return m_height;
}

MCRectShape::~MCRectShape()
{
}
