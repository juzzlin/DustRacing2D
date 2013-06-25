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

#include "mccollisiondetector.hh"
#include "mccontact.hh"
#include "mcobject.hh"
#include "mcsegment.hh"
#include "mcshape.hh"
#include "mccircleshape.hh"
#include "mcrectshape.hh"
#include "mccollisionevent.hh"

#include <cassert>

MCCollisionDetector::MCCollisionDetector()
: m_enableCollisionEvents(false)
{}

void MCCollisionDetector::enableCollisionEvents(bool enable)
{
    m_enableCollisionEvents = enable;
}

MCCollisionDetector::~MCCollisionDetector()
{
}

bool MCCollisionDetector::testRectAgainstRect(
    MCRectShape & shape1, MCRectShape & shape2)
{
    if (&shape1.parent() == &shape2.parent())
    {
        return false;
    }

    const MCOBBox<MCFloat> & obbox1(shape1.obbox());

    MCVector2dF vertex;
    MCVector2dF contactNormal;

    MCFloat depth   = 0;
    bool depthIsSet = false;
    bool collided   = false;

    // Loop thru all vertices of shape1 and generate contacts
    for (MCUint i = 0; i < 4; i++)
    {
        if (shape2.contains(obbox1.vertex(i)))
        {
            // Send collision event to owner of shape1 and generate a contact
            // if accepted.
            MCCollisionEvent ev1(shape2.parent(), obbox1.vertex(i));

            if (m_enableCollisionEvents)
            {
                MCObject::sendEvent(shape1.parent(), ev1);
            }

            vertex     = obbox1.vertex(i);
            depth      = 0;
            depthIsSet = false;

            if (!m_enableCollisionEvents || ev1.accepted())
            {
                depth = shape2.interpenetrationDepth(
                    MCSegment<MCFloat>(vertex, shape1.location()), contactNormal);
                depthIsSet = true;

                MCContact & contact = MCContact::create();
                contact.init(shape2.parent(), vertex, contactNormal, depth);
                shape1.parent().addContact(contact);

                collided = true;
            }

            // Send collision event to owner of shape2 and generate a contact
            // if accepted.
            MCCollisionEvent ev2(shape1.parent(), obbox1.vertex(i));
            if (m_enableCollisionEvents)
            {
                MCObject::sendEvent(shape2.parent(), ev2);
            }

            if (!m_enableCollisionEvents || ev2.accepted())
            {
                if (!depthIsSet)
                {
                    depth = shape2.interpenetrationDepth(
                        MCSegment<MCFloat>(vertex, shape1.location()), contactNormal);
                }

                MCContact & contact = MCContact::create();
                contact.init(shape1.parent(), vertex, -contactNormal, depth);
                shape2.parent().addContact(contact);
            }
        }
    }

    return collided;
}

bool MCCollisionDetector::processPossibleCollision(
    MCObject & object1, MCObject & object2)
{
    if (&object1 == &object2)
    {
        return false;
    }

    // Check that both objects contain a shape
    if (object1.shape() && object2.shape())
    {
        const MCUint id1 = object1.shape()->instanceTypeID();
        const MCUint id2 = object2.shape()->instanceTypeID();

        // Both rects ?
        if (id1 == MCRectShape::typeID() && id2 == MCRectShape::typeID())
        {
            // We must test first object1 against object2 and then
            // the other way around.
            const bool shape1shape2 = testRectAgainstRect(
                // Static cast because we know the types now.
                *static_cast<MCRectShape *>(object1.shape()),
                *static_cast<MCRectShape *>(object2.shape()));

            const bool shape2shape1 = testRectAgainstRect(
                // Static cast because we know the types now.
                *static_cast<MCRectShape *>(object2.shape()),
                *static_cast<MCRectShape *>(object1.shape()));

            return shape1shape2 || shape2shape1;
        }
    }

    return false;
}

MCUint MCCollisionDetector::detectCollisions(MCObjectTree & objectTree)
{
    static MCObjectTree::CollisionVector possibleCollisions;
    objectTree.getBBoxCollisions(possibleCollisions);

    // Check collisions for all registered objects
    MCUint numCollisions = 0;
    auto iter = possibleCollisions.begin();
    const auto end = possibleCollisions.end();
    for (;iter != end; iter++)
    {
        MCObject * obj1(iter->first);
        auto iter2 = iter->second.begin();
        const auto end2 = iter->second.end();
        for (;iter2 != end2; iter2++)
        {
            MCObject * obj2(*iter2);

            if (obj1->physicsObject() && !obj1->bypassCollisions() &&
                obj2->physicsObject() && !obj2->bypassCollisions())
            {
                if (processPossibleCollision(*obj1, *obj2))
                {
                    numCollisions++;
                }
            }
        }
    }

    return numCollisions;
}

