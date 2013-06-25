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

bool MCCollisionDetector::testRectAgainstRect(MCRectShape & rect1, MCRectShape & rect2)
{
    if (&rect1.parent() == &rect2.parent())
    {
        return false;
    }

    const MCOBBox<MCFloat> & obbox1(rect1.obbox());

    bool collided = false;

    // Loop thru all vertices of rect1 and generate contacts for colliding vertices.
    for (MCUint i = 0; i < 4; i++)
    {
        if (rect2.contains(obbox1.vertex(i)))
        {
            // Send collision event to owner of rect1 and generate a contact if accepted.
            MCCollisionEvent ev1(rect2.parent(), obbox1.vertex(i));

            if (m_enableCollisionEvents)
            {
                MCObject::sendEvent(rect1.parent(), ev1);
            }

            MCVector2dF contactNormal;
            MCVector2dF vertex = obbox1.vertex(i);
            MCFloat depth = rect2.interpenetrationDepth(
                MCSegment<MCFloat>(vertex, rect1.location()), contactNormal);

            if (!m_enableCollisionEvents || ev1.accepted())
            {
                MCContact & contact = MCContact::create();
                contact.init(rect2.parent(), vertex, contactNormal, depth);
                rect1.parent().addContact(contact);
                collided = true;
            }

            // Send collision event to owner of rect2 and generate a contact if accepted.
            MCCollisionEvent ev2(rect1.parent(), obbox1.vertex(i));
            if (m_enableCollisionEvents)
            {
                MCObject::sendEvent(rect2.parent(), ev2);
            }

            if (!m_enableCollisionEvents || ev2.accepted())
            {
                MCContact & contact = MCContact::create();
                contact.init(rect1.parent(), vertex, -contactNormal, depth);
                rect2.parent().addContact(contact);
            }

            // Don't break here in the case of a collision, because we don't know
            // yet which contact is the deepest. MCImpulseGenerator handles that.
        }
    }

    return collided;
}

bool MCCollisionDetector::testRectAgainstCircle(MCRectShape & rect, MCCircleShape & circle)
{
    if (&rect.parent() == &circle.parent())
    {
        return false;
    }

    bool collided = false;

    const MCOBBox<MCFloat> & obbox(rect.obbox());

    // Loop through all vertices of the rect and find possible contact points with
    // the circle. This algorithm is not perfectly accurate, but will do the job.
    for (MCUint i = 0; i < 5; i++)
    {
        MCVector2dF rectVertex;
        if (i < 4)
        {
            rectVertex = obbox.vertex(i);
        }
        else
        {
            rectVertex = rect.location();
        }

        MCVector2dF circleVertex(rectVertex - MCVector2dF(circle.location()));
        circleVertex.clampFast(circle.radius());
        circleVertex += MCVector2dF(circle.location());
        if (rect.contains(circleVertex))
        {
            // Send collision event to owner of circle and generate a contact if accepted.
            MCCollisionEvent ev1(rect.parent(), circleVertex);
            if (m_enableCollisionEvents)
            {
                MCObject::sendEvent(circle.parent(), ev1);
            }

            MCVector2dF contactNormal;
            MCFloat depth = rect.interpenetrationDepth(
                MCSegment<MCFloat>(circleVertex, circle.location()), contactNormal);

            if (!m_enableCollisionEvents || ev1.accepted())
            {
                MCContact & contact = MCContact::create();
                contact.init(rect.parent(), circleVertex, contactNormal, depth);
                circle.parent().addContact(contact);
                collided = true;
            }

            // Send collision event to owner of rect and generate a contact if accepted.
            MCCollisionEvent ev2(circle.parent(), circleVertex);
            if (m_enableCollisionEvents)
            {
                MCObject::sendEvent(rect.parent(), ev2);
            }

            if (!m_enableCollisionEvents || ev2.accepted())
            {
                MCContact & contact = MCContact::create();
                contact.init(circle.parent(), circleVertex, -contactNormal, depth);
                rect.parent().addContact(contact);
            }

            // Don't break here in the case of a collision, because we don't know
            // yet which contact is the deepest. MCImpulseGenerator handles that.
        }
    }

    return collided;
}

bool MCCollisionDetector::testCircleAgainstCircle(MCCircleShape & circle1, MCCircleShape & circle2)
{
    if (&circle1.parent() == &circle2.parent())
    {
        return false;
    }

    bool collided = false;
    const MCVector2dF diff = circle2.location() - circle1.location();
    const MCFloat dist = diff.lengthFast();
    const MCVector2dF circleVertex(MCVector2dF(circle1.location()) + diff.normalizedFast() * circle1.radius());

    if (dist < circle1.radius() + circle2.radius())
    {
        // Send collision event to owner of circle2 and generate a contact if accepted.
        MCCollisionEvent ev1(circle1.parent(), circleVertex);
        if (m_enableCollisionEvents)
        {
            MCObject::sendEvent(circle2.parent(), ev1);
        }

        MCVector2dF contactNormal;
        MCFloat depth = circle2.interpenetrationDepth(
            MCSegment<MCFloat>(circleVertex, circle1.location()), contactNormal);

        if (!m_enableCollisionEvents || ev1.accepted())
        {
            MCContact & contact = MCContact::create();
            contact.init(circle1.parent(), circleVertex, -contactNormal, depth);
            circle2.parent().addContact(contact);
            collided = true;
        }

        // Send collision event to owner of circle1 and generate a contact if accepted.
        MCCollisionEvent ev2(circle2.parent(), circleVertex);
        if (m_enableCollisionEvents)
        {
            MCObject::sendEvent(circle1.parent(), ev2);
        }

        if (!m_enableCollisionEvents || ev2.accepted())
        {
            MCContact & contact = MCContact::create();
            contact.init(circle1.parent(), circleVertex, contactNormal, depth);
            circle1.parent().addContact(contact);
            collided = true;
        }
    }

    return collided;
}

bool MCCollisionDetector::processPossibleCollision(MCObject & object1, MCObject & object2)
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

        // Rect against rect
        if (id1 == MCRectShape::typeID() && id2 == MCRectShape::typeID())
        {
            // We must test first object1 against object2 and then
            // the other way around.
            if (testRectAgainstRect(
                // Static cast because we know the types now.
                *static_cast<MCRectShape *>(object1.shape()),
                *static_cast<MCRectShape *>(object2.shape())))
            {
                return true;
            }
            else if (testRectAgainstRect(
                // Static cast because we know the types now.
                *static_cast<MCRectShape *>(object2.shape()),
                *static_cast<MCRectShape *>(object1.shape())))
            {
                return true;
            }
        }
        // Rect against circle
        else if (id1 == MCRectShape::typeID() && id2 == MCCircleShape::typeID())
        {
            // Static cast because we know the types now.
            return testRectAgainstCircle(
                *static_cast<MCRectShape *>(object1.shape()),
                *static_cast<MCCircleShape *>(object2.shape()));
        }
        // Circle against circle
        else if (id1 == MCCircleShape::typeID() && id2 == MCCircleShape::typeID())
        {
            // Static cast because we know the types now.
            return testCircleAgainstCircle(
                *static_cast<MCCircleShape *>(object1.shape()),
                *static_cast<MCCircleShape *>(object2.shape()));
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

