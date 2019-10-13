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
#include "mccircleshape.hh"
#include "mccollisionevent.hh"
#include "mccontact.hh"
#include "mcobject.hh"
#include "mcrectshape.hh"
#include "mcsegment.hh"
#include "mcseparationevent.hh"
#include "mcshape.hh"

MCCollisionDetector::MCCollisionDetector()
{
}

bool MCCollisionDetector::areCurrentlyColliding(MCObject & object1, MCObject & object2)
{
    auto && iter = m_currentCollisions.find(&object1);
    if (iter != m_currentCollisions.end() && iter->second.count(&object2))
    {
        return true;
    }

    iter = m_currentCollisions.find(&object2);
    return iter != m_currentCollisions.end() && iter->second.count(&object1);
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
            }

            m_currentCollisions[&rect1.parent()].insert(&rect2.parent());

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
            }

            m_currentCollisions[&rect.parent()].insert(&circle.parent());

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
        }

        m_currentCollisions[&circle1.parent()].insert(&circle2.parent());

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
        // Test is not symmetric: must be done both ways
        return testRectAgainstRect(
                 *static_cast<MCRectShape *>(object1.shape().get()),
                 *static_cast<MCRectShape *>(object2.shape().get()))
          || testRectAgainstRect(
                 *static_cast<MCRectShape *>(object2.shape().get()),
                 *static_cast<MCRectShape *>(object1.shape().get()));
    }
    // Rect against circle: Case 1
    else if (type1 == MCShape::Type::Rect && type2 == MCShape::Type::Circle)
    {
        return testRectAgainstCircle(
          *static_cast<MCRectShape *>(object1.shape().get()),
          *static_cast<MCCircleShape *>(object2.shape().get()));
    }
    // Rect against circle: Case 2
    else if (type2 == MCShape::Type::Rect && type1 == MCShape::Type::Circle)
    {
        return testRectAgainstCircle(
          *static_cast<MCRectShape *>(object2.shape().get()),
          *static_cast<MCCircleShape *>(object1.shape().get()));
    }
    // Circle against circle
    else if (type1 == MCShape::Type::Circle && type2 == MCShape::Type::Circle)
    {
        // This test is symmetric
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

    // For completeness iterate here if no possible collisions, but there still
    // are old collisions. This can happen if colliding objects gets separated by
    // directly translating them elsewhere.
    if (!numCollisions && !m_currentCollisions.empty())
    {
        return iterateCurrentCollisions();
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
                removedCollisions.push_back({ outer.first, inner });
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
        m_currentCollisions[collisionPair.second].erase(collisionPair.first);

        MCSeparationEvent ev1(*collisionPair.second);
        collisionPair.first->event(ev1);

        MCSeparationEvent ev2(*collisionPair.first);
        collisionPair.second->event(ev2);
    }

    return numCollisions;
}
