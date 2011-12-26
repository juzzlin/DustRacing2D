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
{
    delete m_pImpl;
}

bool MCContactResolverImpl::processRectRect(
    MCRectShape & shape1, MCRectShape & shape2)
{
    if (&shape1.parent() == &shape2.parent()) {
        return false;
    }

    const MCOBBox<MCFloat> & obbox1(shape1.obbox());

    MCVector2d<MCFloat> vertex;
    MCVector2d<MCFloat> contactNormal;

    MCFloat depth           = 0;
    bool depthIsSet         = false;
    bool contactNormalIsSet = false;
    bool collided           = false;

    // Loop thru all vertices of shape1 and generate contacts
    for (MCUint i = 0; i < 4; i++) {
        if (shape2.contains(obbox1.vertex(i))) {

            // Send collision event to owner of shape1 and generate a contact
            // if accepted.
            MCCollisionEvent ev1(shape2.parent());
            MCObject::sendEvent(shape1.parent(), ev1);

            vertex             = obbox1.vertex(i);
            depth              = 0;
            depthIsSet         = false;
            contactNormalIsSet = false;

            if (ev1.accepted()) {
                // TODO: contactPoint should be something else than vertex.
                depth =
                    shape2.interpenetrationDepth(vertex, shape1.location());
                depthIsSet         = true;
                contactNormal      = shape2.contactNormal(shape1.location());
                contactNormalIsSet = true;

                MCContact & contact = MCContact::create();
                contact.init(shape2.parent(), vertex, contactNormal, depth);
                shape1.parent().addContact(contact);

                collided = true;
            }

            // Send collision event to owner of shape2 and generate a contact
            // if accepted.
            MCCollisionEvent ev2(shape1.parent());
            MCObject::sendEvent(shape2.parent(), ev2);

            if (ev2.accepted()) {
                if (!depthIsSet) {
                    depth = shape2.interpenetrationDepth(vertex, shape1.location());
                }

                if (!contactNormalIsSet) {
                    contactNormal = shape2.contactNormal(shape1.location());
                }

                MCContact & contact = MCContact::create();
                contact.init(shape1.parent(), vertex, -contactNormal, depth);
                shape2.parent().addContact(contact);
            }
        }
    }

    return collided;
}

bool MCContactResolverImpl::processRectCircle(
    MCRectShape & shape1, MCCircleShape & shape2)
{
    if (&shape1.parent() == &shape2.parent()) {
        return false;
    }

    MCEdge<MCFloat> edge(shape1.edgeForPoint(shape2.location()));
    const MCVector2d<MCFloat> x(MCVector2d<MCFloat>(shape2.location()) - edge.origin);
    MCFloat dist = MCMathUtil::distanceFromVector(x, edge.edge);
    bool collided = false;

    if (dist < shape2.radius()) {

        collided = true;

        // Send collision event to owner of shape1 and generate a contact
        // if accepted.
        MCCollisionEvent ev1(shape2.parent());
        MCObject::sendEvent(shape1.parent(), ev1);

        // TODO: contactPoint should be something much more accurate than this..
        const MCVector2d<MCFloat> contactPoint(shape2.location());
        const MCFloat depth = shape2.radius() - dist;
        MCVector2d<MCFloat> contactNormal;
        bool contactNormalIsSet = false;

        if (ev1.accepted()) {
            contactNormal = shape1.contactNormal(shape2.location());
            contactNormalIsSet = true;

            MCContact & contact = MCContact::create();
            contact.init(shape2.parent(), contactPoint, -contactNormal, depth);
            shape1.parent().addContact(contact);
        }

        // Send collision event to owner of shape2 and generate a contact
        // if accepted.
        MCCollisionEvent ev2(shape1.parent());
        MCObject::sendEvent(shape2.parent(), ev2);

        if (ev2.accepted()) {
            if (!contactNormalIsSet) {
                contactNormal = shape1.contactNormal(shape2.location());
            }

            MCContact & contact = MCContact::create();
            contact.init(shape1.parent(), contactPoint, contactNormal, depth);
            shape2.parent().addContact(contact);
        }
    }

    return collided;
}

bool MCContactResolverImpl::processCircleCircle(
    MCCircleShape & shape1, MCCircleShape & shape2)
{
    if (&shape1.parent() == &shape2.parent()) {
        return false;
    }

    MCVector2d<MCFloat> d(shape2.location() - shape1.location());
    const MCVector2d<MCFloat> contactPoint(
        MCVector2d<MCFloat>(shape1.location()) + d / 2);
    MCFloat depth = d.lengthFast() - shape1.radius() - shape2.radius();

    if (depth < 0) {
        depth = -depth;

        // Send collision event to owner of shape1 and generate a contact
        // if accepted.
        MCCollisionEvent ev1(shape2.parent());
        MCObject::sendEvent(shape1.parent(), ev1);

        if (ev1.accepted()) {
            const MCVector2d<MCFloat> contactNormal(-d.normalizedFast());
            MCContact & contact = MCContact::create();
            contact.init(shape2.parent(), contactPoint, contactNormal, depth);
            shape1.parent().addContact(contact);
        }

        // Send collision event to owner of shape2 and generate a contact
        // if accepted.
        MCCollisionEvent ev2(shape1.parent());
        MCObject::sendEvent(shape2.parent(), ev2);

        if (ev2.accepted()) {
            const MCVector2d<MCFloat> contactNormal(d.normalizedFast());
            MCContact & contact = MCContact::create();
            contact.init(shape2.parent(), contactPoint, contactNormal, depth);
            shape1.parent().addContact(contact);
        }

        return true;
    }

    return false;
}

bool MCContactResolver::processPossibleCollision(
    MCObject & object1, MCObject & object2)
{
    if (&object1 == &object2) {
        return false;
    }

    // Check that both objects contain a shape
    if (object1.shape() && object2.shape()) {

        // Store id's
        const MCUint id1 = object1.shape()->instanceTypeID();
        const MCUint id2 = object2.shape()->instanceTypeID();

        // Both rects ?
        if (id1 == MCRectShape::typeID() && id2 == MCRectShape::typeID()) {
            // We must test first object1 against object2 and then
            // the other way around.
            const bool shape1shape2 = m_pImpl->processRectRect(
                // Static cast because we know the types now.
                *static_cast<MCRectShape *>(object1.shape()),
                *static_cast<MCRectShape *>(object2.shape()));
            const bool shape2shape1 = m_pImpl->processRectRect(
                // Static cast because we know the types now.
                *static_cast<MCRectShape *>(object2.shape()),
                *static_cast<MCRectShape *>(object1.shape()));
            return shape1shape2 || shape2shape1;
        }

        // Both circles ?
        if (id1 == MCCircleShape::typeID() && id2 == MCCircleShape::typeID()) {
            // This test needs to be run only once for both objects.
            return m_pImpl->processCircleCircle(
                // Static cast because we know the types now
                *static_cast<MCCircleShape *>(object1.shape()),
                *static_cast<MCCircleShape *>(object2.shape()));
        }

        // Test id1 first to match a corresponding method
        if (id1 == MCRectShape::typeID()) {
            if (id2 == MCCircleShape::typeID()) {
                return m_pImpl->processRectCircle(
                    // Static cast because we know the types now
                    *static_cast<MCRectShape *>(object1.shape()),
                    *static_cast<MCCircleShape *>(object2.shape()));
            }
        }

        // Test the other way around
        if (id2 == MCRectShape::typeID()) {
            if (id1 == MCCircleShape::typeID()) {
                return m_pImpl->processRectCircle(
                    // Static cast because we know the types now
                    *static_cast<MCRectShape *>(object2.shape()),
                    *static_cast<MCCircleShape *>(object1.shape()));
            }
        }
    }

    return false;
}
