// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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

#include "MCCollisionDetectorTest.hpp"

#include "../../Core/mcobject.hh"
#include "../../Core/mcvector3d.hh"
#include "../../Core/mcworld.hh"
#include "../../Physics/mccircleshape.hh"
#include "../../Physics/mccollisiondetector.hh"
#include "../../Physics/mccontact.hh"
#include "../../Physics/mcobjectgrid.hh"
#include "../../Physics/mcphysicscomponent.hh"
#include "../../Physics/mcrectshape.hh"

#include <cmath>
#include <memory>
#include <vector>

namespace {

const float TOLERANCE = 0.01f;

//! Runs the narrow phase for the given objects and leaves the contacts to the objects.
class Scene
{
public:
    Scene()
      : m_grid(-1000, -1000, 1000, 1000, 16, 16)
    {
        // Objects need a world instance, although they aren't added to it
        m_world.setDimensions(-1000, 1000, -1000, 1000, -1000, 1000, 1.0f, false);
    }

    MCObject & addBox(float width, float height, MCVector2dF location, float angle = 0, bool stationary = false)
    {
        return add(std::make_shared<MCRectShape>(nullptr, width, height), location, angle, stationary);
    }

    MCObject & addCircle(float radius, MCVector2dF location)
    {
        return add(std::make_shared<MCCircleShape>(nullptr, radius), location, 0, false);
    }

    void detect()
    {
        m_detector.detectCollisions(m_grid);
    }

    ~Scene()
    {
        for (auto && object : m_objects)
        {
            object->deleteContacts();
        }
    }

private:
    MCWorld m_world;

    MCObject & add(MCShapePtr shape, MCVector2dF location, float angle, bool stationary)
    {
        const auto object = std::make_shared<MCObject>(shape, "TestObject");
        object->setIsPhysicsObject(true);
        object->physicsComponent().setMass(1, stationary);
        object->rotate(angle);
        object->translate(MCVector3dF(location));
        m_grid.insert(*object);
        m_objects.push_back(object);
        return *object;
    }

    MCObjectGrid m_grid;

    MCCollisionDetector m_detector;


    std::vector<std::shared_ptr<MCObject>> m_objects;
};

//! \return contacts of the object with the other object.
std::vector<MCContact *> contacts(MCObject & object, MCObject & other)
{
    const auto iter = object.contacts().find(&other);
    return iter == object.contacts().end() ? std::vector<MCContact *> {} : iter->second;
}

//! \return the deepest contact of the object with the other object, or nullptr.
MCContact * deepest(MCObject & object, MCObject & other)
{
    MCContact * result = nullptr;
    for (auto && contact : contacts(object, other))
    {
        if (!result || contact->interpenetrationDepth() > result->interpenetrationDepth())
        {
            result = contact;
        }
    }
    return result;
}

bool equals(const MCVector2dF & a, const MCVector2dF & b)
{
    return std::fabs(a.i() - b.i()) < TOLERANCE && std::fabs(a.j() - b.j()) < TOLERANCE;
}

} // namespace

void MCCollisionDetectorTest::testSeparatedBoxesDoNotCollide()
{
    Scene scene;
    auto && a = scene.addBox(4, 4, { 0, 0 });
    auto && b = scene.addBox(4, 4, { 4.5f, 0 });
    scene.detect();

    QVERIFY(contacts(a, b).empty());
    QVERIFY(contacts(b, a).empty());
}

void MCCollisionDetectorTest::testRotatedBoxesWithOverlappingBoundingBoxesDoNotCollide()
{
    // Diagonal neighbors whose axis aligned bounding boxes overlap, but the boxes themselves don't
    Scene scene;
    auto && a = scene.addBox(10, 2, { 0, 0 }, 45);
    auto && b = scene.addBox(10, 2, { 3, -3 }, 45);
    scene.detect();

    QVERIFY(contacts(a, b).empty());
    QVERIFY(contacts(b, a).empty());
}

void MCCollisionDetectorTest::testCornerContactNormalPointsOutOfTheOtherBox()
{
    // A small box rotated 45 degrees touches the top face of a wide wall with its lowest corner.
    Scene scene;
    auto && wall = scene.addBox(100, 10, { 0, 0 }, 0, true);
    auto && box = scene.addBox(4, 4, { 0, 5 + std::sqrt(8.0f) - 0.5f }, 45);
    scene.detect();

    const auto contact = deepest(box, wall);
    QVERIFY(contact);
    QVERIFY(equals(contact->contactNormal(), { 0, 1 }));
    QVERIFY(std::fabs(contact->interpenetrationDepth() - 0.5f) < TOLERANCE);
}

void MCCollisionDetectorTest::testCornerContactHasOneContactPoint()
{
    Scene scene;
    auto && wall = scene.addBox(100, 10, { 0, 0 }, 0, true);
    auto && box = scene.addBox(4, 4, { 0, 5 + std::sqrt(8.0f) - 0.5f }, 45);
    scene.detect();

    QCOMPARE(contacts(box, wall).size(), size_t { 1 });
    QVERIFY(equals(contacts(box, wall).front()->contactPoint(), { 0, 4.75f }) || equals(contacts(box, wall).front()->contactPoint(), { 0, 4.5f }));
}

void MCCollisionDetectorTest::testFlatContactHasTwoContactPoints()
{
    // A box lies flat on the top face of a wall: the contact is a whole edge,
    // which is represented by its two end points.
    Scene scene;
    auto && wall = scene.addBox(100, 10, { 0, 0 }, 0, true);
    auto && box = scene.addBox(8, 4, { 0, 6.5f });
    scene.detect();

    const auto boxContacts = contacts(box, wall);
    QCOMPARE(boxContacts.size(), size_t { 2 });
    for (auto && contact : boxContacts)
    {
        QVERIFY(equals(contact->contactNormal(), { 0, 1 }));
        QVERIFY(std::fabs(contact->interpenetrationDepth() - 0.5f) < TOLERANCE);
    }

    // The points are at both ends of the contact edge
    QVERIFY(std::fabs(std::fabs(boxContacts.at(0)->contactPoint().i() - boxContacts.at(1)->contactPoint().i()) - 8.0f) < TOLERANCE);
}

void MCCollisionDetectorTest::testDeepPenetrationUsesTheAxisOfLeastOverlap()
{
    // The center of the box is already inside the wall, but the box is clearly closest
    // to getting out through the top face of the wall.
    Scene scene;
    auto && wall = scene.addBox(100, 10, { 0, 0 }, 0, true);
    auto && box = scene.addBox(4, 4, { 10, 4 });
    scene.detect();

    const auto contact = deepest(box, wall);
    QVERIFY(contact);
    QVERIFY(equals(contact->contactNormal(), { 0, 1 }));
    QVERIFY(std::fabs(contact->interpenetrationDepth() - 3.0f) < TOLERANCE);
}

void MCCollisionDetectorTest::testCrossingBoxesWithoutCornersInsideCollide()
{
    // Two long, thin boxes crossing like a plus sign: no corner of either box is inside
    // the other one, but they clearly overlap.
    Scene scene;
    auto && a = scene.addBox(20, 2, { 0, 0 });
    auto && b = scene.addBox(2, 20, { 0, 0.5f });
    scene.detect();

    QEXPECT_FAIL("", "Only corners inside the other box are detected", Abort);
    QVERIFY(!contacts(a, b).empty());
    QVERIFY(!contacts(b, a).empty());
}

void MCCollisionDetectorTest::testContactsAreGeneratedForBothBoxes()
{
    Scene scene;
    auto && wall = scene.addBox(100, 10, { 0, 0 }, 0, true);
    auto && box = scene.addBox(8, 4, { 0, 6.5f });
    scene.detect();

    // The contacts of the wall are the same, but with the normals pointing the other way
    const auto wallContacts = contacts(wall, box);
    QCOMPARE(wallContacts.size(), contacts(box, wall).size());
    for (auto && contact : wallContacts)
    {
        QVERIFY(equals(contact->contactNormal(), { 0, -1 }));
    }
}

void MCCollisionDetectorTest::testCircleAgainstBoxEdge()
{
    Scene scene;
    auto && wall = scene.addBox(100, 10, { 0, 0 }, 0, true);
    auto && circle = scene.addCircle(3, { 20, 7 });
    scene.detect();

    QEXPECT_FAIL("", "Circles are tested only against points towards the corners and the center of the box", Abort);
    const auto contact = deepest(circle, wall);
    QVERIFY(contact);
    QVERIFY(equals(contact->contactNormal(), { 0, 1 }));
    QVERIFY(std::fabs(contact->interpenetrationDepth() - 1.0f) < TOLERANCE);
    QVERIFY(equals(contact->contactPoint(), { 20, 5 }) || equals(contact->contactPoint(), { 20, 4 }));
}

void MCCollisionDetectorTest::testCircleAgainstBoxCorner()
{
    // The circle touches the upper right corner of the box diagonally
    Scene scene;
    auto && box = scene.addBox(10, 10, { 0, 0 }, 0, true);
    const float offset = 5 + 2.5f / std::sqrt(2.0f);
    auto && circle = scene.addCircle(3, { offset, offset });
    scene.detect();

    const auto contact = deepest(circle, box);
    QVERIFY(contact);
    const float diagonal = 1.0f / std::sqrt(2.0f);
    QEXPECT_FAIL("", "Circle against box contacts are approximate", Abort);
    QVERIFY(equals(contact->contactNormal(), { diagonal, diagonal }));
    QVERIFY(std::fabs(contact->interpenetrationDepth() - 0.5f) < TOLERANCE);
}

void MCCollisionDetectorTest::testCircleAgainstCircle()
{
    Scene scene;
    auto && a = scene.addCircle(3, { 0, 0 });
    auto && b = scene.addCircle(2, { 4, 0 });
    scene.detect();

    QEXPECT_FAIL("", "The contact of the first circle refers to the circle itself", Abort);
    const auto contact = deepest(b, a);
    QVERIFY(contact);
    QVERIFY(equals(contact->contactNormal(), { 1, 0 }));
    QVERIFY(std::fabs(contact->interpenetrationDepth() - 1.0f) < TOLERANCE);
}

QTEST_GUILESS_MAIN(MCCollisionDetectorTest)
