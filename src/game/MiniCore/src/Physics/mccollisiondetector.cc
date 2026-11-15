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
#include "mcseparationevent.hh"
#include "mcshape.hh"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

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

namespace {

//! An oriented box in a form suitable for the separating axis test.
struct Box
{
    explicit Box(const MCRectShape & rect)
    {
        const auto & obbox = rect.obbox();
        center = obbox.location();
        // Vertices are in the order (-x, -y), (-x, y), (x, y), (x, -y) in the frame of the box
        axes[0] = (obbox.vertex(3) - obbox.vertex(0)).normalized();
        axes[1] = (obbox.vertex(1) - obbox.vertex(0)).normalized();
        half[0] = obbox.hx();
        half[1] = obbox.hy();
    }

    //! \return the half length of the projection of the box on the given axis.
    float projectedRadius(const MCVector2dF & axis) const
    {
        return half[0] * std::fabs(axes[0].dot(axis)) + half[1] * std::fabs(axes[1].dot(axis));
    }

    MCVector2dF center;

    MCVector2dF axes[2];

    float half[2];
};

} // namespace

bool MCCollisionDetector::processManifold(MCObject & object1, MCObject & object2, const Manifold & manifold)
{
    if (manifold.points.empty())
    {
        return false;
    }

    const auto & eventLocation = manifold.points.front().location;
    MCCollisionEvent ev1(object2, eventLocation);
    MCCollisionEvent ev2(object1, eventLocation);

    const bool areColliding = areCurrentlyColliding(object1, object2);
    if (!areColliding)
    {
        MCObject::sendEvent(object1, ev1);
        MCObject::sendEvent(object2, ev2);
    }

    m_currentCollisions[&object1].insert(&object2);

    if ((ev1.accepted() && ev2.accepted()) || areColliding)
    {
        if (!object1.isTriggerObject() && !object2.isTriggerObject())
        {
            for (auto && point : manifold.points)
            {
                MCContact & contact2 = MCContact::create();
                contact2.init(object1, point.location, manifold.normal, point.depth);
                object2.addContact(contact2);

                MCContact & contact1 = MCContact::create();
                contact1.init(object2, point.location, -manifold.normal, point.depth);
                object1.addContact(contact1);
            }
        }

        return true;
    }

    return false;
}

bool MCCollisionDetector::testRectAgainstRect(MCRectShape & rect1, MCRectShape & rect2)
{
    const Box box1(rect1);
    const Box box2(rect2);
    const MCVector2dF centerDelta(box2.center - box1.center);

    // Find the axis of least overlap. The axes of the first box are preferred on ties,
    // so that the choice of the reference face doesn't flicker between steps.
    float minOverlap = std::numeric_limits<float>::max();
    const Box * reference = nullptr;
    const Box * incident = nullptr;
    size_t referenceAxis = 0;
    for (auto && [owner, other] : { std::pair { &box1, &box2 }, std::pair { &box2, &box1 } })
    {
        for (size_t i = 0; i < 2; i++)
        {
            const auto & axis = owner->axes[i];
            const float overlap = box1.projectedRadius(axis) + box2.projectedRadius(axis) - std::fabs(centerDelta.dot(axis));
            if (overlap <= 0)
            {
                return false; // Separating axis found
            }

            if (overlap < minOverlap - 0.001f)
            {
                minOverlap = overlap;
                reference = owner;
                incident = other;
                referenceAxis = i;
            }
        }
    }

    // Normal of the reference face points towards the incident box
    MCVector2dF normal(reference->axes[referenceAxis]);
    if ((incident->center - reference->center).dot(normal) < 0)
    {
        normal = -normal;
    }

    const MCVector2dF faceCenter(reference->center + normal * reference->half[referenceAxis]);
    const MCVector2dF faceTangent(reference->axes[1 - referenceAxis]);
    const float faceHalfLength = reference->half[1 - referenceAxis];

    // Incident edge: the face of the incident box most anti-parallel to the normal
    size_t incidentAxis = 0;
    float minDot = std::numeric_limits<float>::max();
    float incidentSign = 1;
    for (size_t i = 0; i < 2; i++)
    {
        for (float sign : { -1.0f, 1.0f })
        {
            if (const float dot = (incident->axes[i] * sign).dot(normal); dot < minDot)
            {
                minDot = dot;
                incidentAxis = i;
                incidentSign = sign;
            }
        }
    }

    const MCVector2dF edgeCenter(incident->center + incident->axes[incidentAxis] * incidentSign * incident->half[incidentAxis]);
    const MCVector2dF edgeHalf(incident->axes[1 - incidentAxis] * incident->half[1 - incidentAxis]);
    MCVector2dF edge[2] = { edgeCenter - edgeHalf, edgeCenter + edgeHalf };

    // Clip the incident edge to the side planes of the reference face
    for (float side : { -1.0f, 1.0f })
    {
        const float distances[2] = {
            (edge[0] - faceCenter).dot(faceTangent) * side - faceHalfLength,
            (edge[1] - faceCenter).dot(faceTangent) * side - faceHalfLength
        };
        if (distances[0] > 0 && distances[1] > 0)
        {
            return false;
        }
        if (distances[0] > 0 || distances[1] > 0)
        {
            const size_t outside = distances[0] > 0 ? 0 : 1;
            const float t = distances[outside] / (distances[outside] - distances[1 - outside]);
            edge[outside] = edge[outside] + (edge[1 - outside] - edge[outside]) * t;
        }
    }

    // Points of the clipped edge below the reference face are the contact points
    Manifold manifold;
    for (auto && point : edge)
    {
        if (const float separation = (point - faceCenter).dot(normal); separation < 0)
        {
            manifold.points.push_back({ point, -separation });
        }
    }

    // The manifold normal pushes the second object out of the first one
    if (reference == &box1)
    {
        manifold.normal = normal;
        return processManifold(rect1.parent(), rect2.parent(), manifold);
    }

    manifold.normal = normal;
    return processManifold(rect2.parent(), rect1.parent(), manifold);
}

bool MCCollisionDetector::testRectAgainstCircle(MCRectShape & rect, MCCircleShape & circle)
{
    const Box box(rect);
    const MCVector2dF circleCenter(circle.location());
    const MCVector2dF local(circleCenter - box.center);
    const float x = local.dot(box.axes[0]);
    const float y = local.dot(box.axes[1]);

    Manifold manifold;
    if (std::fabs(x) <= box.half[0] && std::fabs(y) <= box.half[1])
    {
        // The center of the circle is inside the box: push it out through the nearest face
        const float distanceX = box.half[0] - std::fabs(x);
        const float distanceY = box.half[1] - std::fabs(y);
        const size_t axis = distanceX < distanceY ? 0 : 1;
        const float sign = (axis == 0 ? x : y) < 0 ? -1.0f : 1.0f;
        manifold.normal = box.axes[axis] * sign;
        const float distance = axis == 0 ? distanceX : distanceY;
        manifold.points.push_back({ circleCenter + manifold.normal * distance, circle.radius() + distance });
    }
    else
    {
        const float closestX = std::clamp(x, -box.half[0], box.half[0]);
        const float closestY = std::clamp(y, -box.half[1], box.half[1]);
        const MCVector2dF closest(box.center + box.axes[0] * closestX + box.axes[1] * closestY);
        const MCVector2dF delta(circleCenter - closest);
        const float distance = delta.length();
        if (distance >= circle.radius())
        {
            return false;
        }

        manifold.normal = delta * (1.0f / distance);
        manifold.points.push_back({ closest, circle.radius() - distance });
    }

    return processManifold(rect.parent(), circle.parent(), manifold);
}

bool MCCollisionDetector::testCircleAgainstCircle(MCCircleShape & circle1, MCCircleShape & circle2)
{
    const MCVector2dF delta(MCVector2dF(circle2.location()) - MCVector2dF(circle1.location()));
    const float distance = delta.length();
    const float overlap = circle1.radius() + circle2.radius() - distance;
    if (overlap <= 0)
    {
        return false;
    }

    Manifold manifold;
    manifold.normal = distance > 0 ? delta * (1.0f / distance) : MCVector2dF(1, 0);
    manifold.points.push_back({ MCVector2dF(circle1.location()) + manifold.normal * (circle1.radius() - overlap / 2), overlap });
    return processManifold(circle1.parent(), circle2.parent(), manifold);
}

bool MCCollisionDetector::processPossibleCollision(MCObject & object1, MCObject & object2)
{
    const auto type1 = object1.shape()->type();
    const auto type2 = object2.shape()->type();

    // Rect against rect
    if (type1 == MCShape::Type::Rect && type2 == MCShape::Type::Rect)
    {
        return testRectAgainstRect(
          *static_cast<MCRectShape *>(object1.shape().get()),
          *static_cast<MCRectShape *>(object2.shape().get()));
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
