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

#include "mccontactresolver.hh"
#include "mccontactresolverimpl.hh"
#include "mccontact.hh"
#include "mcobject.hh"
#include "mcshape.hh"
#include "mcrectshape.hh"
#include "mccircleshape.hh"
#include "mccollisionevent.hh"

MCContactResolverImpl::MCContactResolverImpl()
{}

MCContactResolverImpl::~MCContactResolverImpl()
{}

MCContactResolver::MCContactResolver() :
    m_pImpl(new MCContactResolverImpl)
{}

MCContactResolver::~MCContactResolver()
{}

bool MCContactResolverImpl::processRectRect(MCRectShape * p1, MCRectShape * p2)
{
    if (p1->parent() && p2->parent() && (p1->parent() == p2->parent())) {
        return false;
    }

    const MCOBBox<MCFloat> & obbox1(p1->obbox());
    bool collided = false;

    MCVector2d<MCFloat> vertex;
    MCVector2d<MCFloat> contactNormal;

    MCFloat depth;
    bool depthIsSet;
    bool contactNormalIsSet;

    // Loop thru all vertices and generate contacts
    for (UINT i = 0; i < 4; i++) {
        if (p2->contains(obbox1.vertex(i))) {

            // Send collision event to owner of p1
            MCCollisionEvent ev1(p2->parent());
            MCObject::sendEvent(p1->parent(), &ev1);

            vertex = obbox1.vertex(i);
            depth = 0;
            depthIsSet = false;
            contactNormalIsSet = false;

            if (ev1.accepted()) {
                // TODO: contactPoint should be something else than vertex.
                depth = p2->interpenetrationDepth(vertex, p1->location());
                depthIsSet = true;
                contactNormal = p2->contactNormal(p1->location());
                contactNormalIsSet = true;

                MCContact * pContact = MCContact::create();
                pContact->init(p2->parent(), vertex, contactNormal, depth);
                p1->parent()->addContact(pContact);
            }

            // Send collision event to owner of p2
            MCCollisionEvent ev2(p1->parent());
            MCObject::sendEvent(p2->parent(), &ev2);

            if (ev2.accepted()) {
                if (!depthIsSet) {
                    depth = p2->interpenetrationDepth(vertex, p1->location());
                }

                if (!contactNormalIsSet) {
                    contactNormal = p2->contactNormal(p1->location());
                }

                MCContact * pContact = MCContact::create();
                pContact->init(p1->parent(), vertex, -contactNormal, depth);
                p2->parent()->addContact(pContact);
            }

            return true;
        }
    }

    return collided;
}

bool MCContactResolverImpl::processRectCircle(MCRectShape * p1, MCCircleShape * p2)
{
    if (p1->parent() && p2->parent() && (p1->parent() == p2->parent())) {
        return false;
    }

    MCEdge<MCFloat> edge(p1->edgeForPoint(p2->location()));
    const MCVector2d<MCFloat> x(MCVector2d<MCFloat>(p2->location()) - edge.origin);
    const MCFloat dist = MCMathUtil::distanceFromVector(x, edge.edge);
    bool collided = false;

    if (dist < p2->radius()) {

        collided = true;

        // Send collision event to owner of p1
        MCCollisionEvent ev1(p2->parent());
        MCObject::sendEvent(p1->parent(), &ev1);

        // TODO: contactPoint should be something much more accurate than this..
        const MCVector2d<MCFloat> contactPoint(p2->location());
        const MCFloat depth = p2->radius() - dist;
        MCVector2d<MCFloat> contactNormal;
        bool contactNormalIsSet = false;

        if (ev1.accepted()) {
            contactNormal = p1->contactNormal(p2->location());
            contactNormalIsSet = true;

            MCContact * pContact = MCContact::create();
            pContact->init(p2->parent(), contactPoint, -contactNormal, depth);
            p1->parent()->addContact(pContact);
        }

        // Send collision event to owner of p2
        MCCollisionEvent ev2(p1->parent());
        MCObject::sendEvent(p2->parent(), &ev2);

        if (ev2.accepted()) {
            if (!contactNormalIsSet) {
                contactNormal = p1->contactNormal(p2->location());
            }

            MCContact * pContact = MCContact::create();
            pContact->init(p1->parent(), contactPoint, contactNormal, depth);
            p2->parent()->addContact(pContact);
        }
    }

    return collided;
}

bool MCContactResolverImpl::processCircleCircle(MCCircleShape * p1, MCCircleShape * p2)
{
    if (p1->parent() && p2->parent() && (p1->parent() == p2->parent())) {
        return false;
    }

    MCVector2d<MCFloat> d(p2->location() - p1->location());
    const MCVector2d<MCFloat> contactPoint(MCVector2d<MCFloat>(p1->location()) + d / 2);
    MCFloat depth = d.lengthFast() - p1->radius() - p2->radius();

    if (depth < 0) {

        depth = -depth;

        // Send collision event to owner of p1
        MCCollisionEvent ev1(p2->parent());
        MCObject::sendEvent(p1->parent(), &ev1);

        if (ev1.accepted()) {
            const MCVector2d<MCFloat> contactNormal(-d.normalizedFast());
            MCContact * pContact = MCContact::create();
            pContact->init(p2->parent(), contactPoint, contactNormal, depth);
            p1->parent()->addContact(pContact);
        }

        // Send collision event to owner of p2
        MCCollisionEvent ev2(p1->parent());
        MCObject::sendEvent(p2->parent(), &ev2);

        if (ev2.accepted()) {
            const MCVector2d<MCFloat> contactNormal(d.normalizedFast());
            MCContact * pContact = MCContact::create();
            pContact->init(p2->parent(), contactPoint, contactNormal, depth);
            p1->parent()->addContact(pContact);
        }

        return true;
    }

    return false;
}

bool MCContactResolver::processPossibleCollision(MCObject * p1, MCObject * p2)
{
    if (p1 == p2) {
        return false;
    }

    // Check that both objects contain a shape
    if (p1->shape() && p2->shape()) {

        // Store id's
        const UINT id1 = p1->shape()->instanceTypeID();
        const UINT id2 = p2->shape()->instanceTypeID();

        // Both rects ?
        if (id1 == MCRectShape::typeID() && id2 == MCRectShape::typeID()) {

            // Static cast because we know the types now
            return m_pImpl->processRectRect(static_cast<MCRectShape *>(p1->shape()),
                                            static_cast<MCRectShape *>(p2->shape()));
        }

        // Both circles ?
        if (id1 == MCCircleShape::typeID() && id2 == MCCircleShape::typeID()) {
            // Static cast because we know the types now
            return m_pImpl->processCircleCircle(static_cast<MCCircleShape *>(p1->shape()),
                                                static_cast<MCCircleShape *>(p2->shape()));
        }

        // Test id1 first to match a corresponding method
        if (id1 == MCRectShape::typeID()) {
            if (id2 == MCCircleShape::typeID()) {
                // Static cast because we know the types now
                return m_pImpl->processRectCircle(static_cast<MCRectShape *>(p1->shape()),
                                                  static_cast<MCCircleShape *>(p2->shape()));
            }
        }

        // Test the other way around
        if (id2 == MCRectShape::typeID()) {
            if (id1 == MCCircleShape::typeID()) {
                // Static cast because we know the types now
                return m_pImpl->processRectCircle(static_cast<MCRectShape *>(p2->shape()),
                                                  static_cast<MCCircleShape *>(p1->shape()));
            }
        }
    }

    return false;
}
