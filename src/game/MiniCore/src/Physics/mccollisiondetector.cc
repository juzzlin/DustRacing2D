// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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
#include "mcseparationevent.hh"

MCCollisionDetector::MCCollisionDetector()
{}

bool MCCollisionDetector::areCurrentlyColliding(MCObject & object1, MCObject & object2)
{
    auto && iter = m_currentCollisions.find(&object1);
    return iter != m_currentCollisions.end() && iter->second.count(&object2);
}

void MCCollisionDetector::clear()
{
    m_currentCollisions.clear();
}

void MCCollisionDetector::remove(MCObject & object)
{
    auto && outer = m_currentCollisions.find(&object);
    if (outer != m_currentCollisions.end())
    {
        for (auto && outerLinked : outer->second)
        {
            if (m_currentCollisions.count(outerLinked))
            {
                m_currentCollisions[outerLinked].erase(outer->first);
            }
        }

        m_currentCollisions.erase(outer);
    }
}

bool MCCollisionDetector::testRectAgainstRect(MCRectShape & rect1, MCRectShape & rect2)
{
    const MCOBBox<float> & obbox1(rect1.obbox());

    bool collided = false;

    // Loop thru all vertices of rect1 and generate contacts for colliding vertices.
    for (unsigned int i = 0; i < 4; i++)
    {
        if (rect2.contains(obbox1.vertex(i)))
        {
            const bool triggerObjectInvolved = rect1.parent().isTriggerObject() || rect2.parent().isTriggerObject();

            MCCollisionEvent ev1(rect2.parent(), obbox1.vertex(i));
            MCCollisionEvent ev2(rect1.parent(), obbox1.vertex(i));

            const bool areColliding = areCurrentlyColliding(rect1.parent(), rect2.parent());
            if (!areColliding)
            {
                MCObject::sendEvent(rect1.parent(), ev1);
                MCObject::sendEvent(rect2.parent(), ev2);

                m_currentCollisions[&rect1.parent()].insert(&rect2.parent());
            }

            if ((ev1.accepted() && ev2.accepted()) || areColliding)
            {
                if (!triggerObjectInvolved)
                {
                    MCVector2dF contactNormal;
                    MCVector2dF vertex = obbox1.vertex(i);
                    float depth = rect2.interpenetrationDepth(
                        MCSegment<float>(vertex, rect1.location()), contactNormal);

                    {
                        MCContact & contact = MCContact::create();
                        contact.init(rect2.parent(), vertex, contactNormal, depth);
                        rect1.parent().addContact(contact);
                    }

                    {
                        MCContact & contact = MCContact::create();
                        contact.init(rect1.parent(), vertex, -contactNormal, depth);
                        rect2.parent().addContact(contact);
                    }
                }

                collided = true;
            }

            // Don't break here in the case of a collision, because we don't know
            // yet which contact is the deepest. MCImpulseGenerator handles that.
        }
    }

    return collided;
}

bool MCCollisionDetector::testRectAgainstCircle(MCRectShape & rect, MCCircleShape & circle)
{
    bool collided = false;

    const MCOBBox<float> & obbox(rect.obbox());

    // Loop through all vertices of the rect and find possible contact points with
    // the circle. This algorithm is not perfectly accurate, but will do the job.
    for (unsigned int i = 0; i < 5; i++)
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
            const bool triggerObjectInvolved = rect.parent().isTriggerObject() || circle.parent().isTriggerObject();

            MCCollisionEvent ev1(rect.parent(), circleVertex);
            MCCollisionEvent ev2(circle.parent(), circleVertex);

            const bool areColliding = areCurrentlyColliding(rect.parent(), circle.parent());
            if (!areColliding)
            {
                MCObject::sendEvent(circle.parent(), ev1);
                MCObject::sendEvent(rect.parent(), ev2);

                m_currentCollisions[&rect.parent()].insert(&circle.parent());
            }

            if ((ev1.accepted() && ev2.accepted()) || areColliding)
            {
                if (!triggerObjectInvolved)
                {
                    MCVector2dF contactNormal;
                    float depth = rect.interpenetrationDepth(
                        MCSegment<float>(circleVertex, circle.location()), contactNormal);

                    {
                        MCContact & contact = MCContact::create();
                        contact.init(rect.parent(), circleVertex, contactNormal, depth);
                        circle.parent().addContact(contact);
                    }

                    {
                        MCContact & contact = MCContact::create();
                        contact.init(circle.parent(), circleVertex, -contactNormal, depth);
                        rect.parent().addContact(contact);
                    }
                }

                collided = true;
            }

            // Don't break here in the case of a collision, because we don't know
            // yet which contact is the deepest. MCImpulseGenerator handles that.
        }
    }

    return collided;
}

bool MCCollisionDetector::testCircleAgainstCircle(MCCircleShape & circle1, MCCircleShape & circle2)
{
    bool collided = false;
    MCVector2dF contactNormal;
    const float depth = circle2.interpenetrationDepth(circle1, contactNormal);
    const MCVector2dF contactPoint(MCVector2dF(circle1.location()) - contactNormal * circle1.radius());

    if (depth > 0)
    {
        const bool triggerObjectInvolved = circle1.parent().isTriggerObject() || circle2.parent().isTriggerObject();

        MCCollisionEvent ev1(circle1.parent(), contactPoint);
        MCCollisionEvent ev2(circle2.parent(), contactPoint);

        const bool areColliding = areCurrentlyColliding(circle1.parent(), circle2.parent());
        if (!areColliding)
        {
            MCObject::sendEvent(circle2.parent(), ev1);
            MCObject::sendEvent(circle1.parent(), ev2);

            m_currentCollisions[&circle1.parent()].insert(&circle2.parent());
        }

        if ((ev1.accepted() && ev2.accepted()) || areColliding)
        {
            if (!triggerObjectInvolved)
            {
                {
                    MCContact & contact = MCContact::create();
                    contact.init(circle1.parent(), contactPoint, -contactNormal, depth);
                    circle2.parent().addContact(contact);
                }

                {
                    MCContact & contact = MCContact::create();
                    contact.init(circle1.parent(), contactPoint, contactNormal, depth);
                    circle1.parent().addContact(contact);
                }
            }

            collided = true;
        }
    }

    return collided;
}

bool MCCollisionDetector::processPossibleCollision(MCObject & object1, MCObject & object2)
{
    const auto type1 = object1.shape()->type();
    const auto type2 = object2.shape()->type();

    // Rect against rect
    if (type1 == MCShape::Type::Rect && type2 == MCShape::Type::Rect)
    {
        // We must test first object1 against object2 and then
        // the other way around.
        if (testRectAgainstRect(
            // Static cast because we know the types now.
            *static_cast<MCRectShape *>(object1.shape().get()),
            *static_cast<MCRectShape *>(object2.shape().get())))
        {
            return true;
        }
        else if (testRectAgainstRect(
            // Static cast because we know the types now.
            *static_cast<MCRectShape *>(object2.shape().get()),
            *static_cast<MCRectShape *>(object1.shape().get())))
        {
            return true;
        }
    }
    // Rect against circle
    else if (type1 == MCShape::Type::Rect && type2 == MCShape::Type::Circle)
    {
        // Static cast because we know the types now.
        return testRectAgainstCircle(
            *static_cast<MCRectShape *>(object1.shape().get()),
            *static_cast<MCCircleShape *>(object2.shape().get()));
    }
    // Circle against circle
    else if (type1 == MCShape::Type::Circle && type2 == MCShape::Type::Circle)
    {
        // Static cast because we know the types now.
        return testCircleAgainstCircle(
            *static_cast<MCCircleShape *>(object1.shape().get()),
            *static_cast<MCCircleShape *>(object2.shape().get()));
    }

    return false;
}

unsigned int MCCollisionDetector::detectCollisions(MCObjectGrid & objectGrid)
{
    unsigned int numCollisions = 0;

    for (auto && iter : objectGrid.getPossibleCollisions())
    {
        numCollisions += processPossibleCollision(*iter.first, *iter.second);
    }

    return numCollisions;
}

unsigned int MCCollisionDetector::iterateCurrentCollisions()
{
    unsigned int numCollisions = 0;

    std::vector<std::pair<MCObject *, MCObject *>> removedCollisions;

    for (auto && outer : m_currentCollisions)
    {
        for (auto && inner : outer.second)
        {
            if (!processPossibleCollision(*outer.first, *inner))
            {
                removedCollisions.push_back({outer.first, inner});
            }
            else
            {
                numCollisions++;
            }
        }
    }

    for (auto && collisionPair : removedCollisions)
    {
        m_currentCollisions[collisionPair.first].erase(collisionPair.second);

        MCSeparationEvent event(*collisionPair.second);
        collisionPair.first->event(event);
    }

    return numCollisions;
}

