// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2014 Jussi Lind <jussi.lind@iki.fi>
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

#include "MCWorldTest.hpp"
#include "../../Core/mcobject.hh"
#include "../../Core/mcworld.hh"
#include "../../Physics/mccircleshape.hh"
#include "../../Physics/mccollisionevent.hh"
#include "../../Physics/mcphysicscomponent.hh"
#include "../../Physics/mcrectshape.hh"
#include "../../Physics/mcseparationevent.hh"

class TestObject : public MCObject
{
public:
    TestObject()
      : MCObject("TEST_OBJECT")
    {
    }

    virtual void collisionEvent(MCCollisionEvent & event)
    {
        collisionEventsReceived++;
        event.accept();
    }

    int collisionEventsReceived = 0;

    virtual void separationEvent(MCSeparationEvent & event)
    {
        separationEventsReceived++;
        event.accept();
    }

    int separationEventsReceived = 0;
};

MCWorldTest::MCWorldTest()
{
}

void MCWorldTest::testAddToWorld()
{
    MCWorld world;
    MCObject object("test");
    QVERIFY(object.index() == -1);
    world.addObject(object);
    QVERIFY(object.index() >= 0);
    QVERIFY(world.objectCount() == 5); // 5 includes built-in walls

    world.removeObject(object); // Lazy removal
    QVERIFY(object.index() >= 0);
    world.stepTime(1);
    QVERIFY(object.index() == -1);
    QVERIFY(world.objectCount() == 4);

    world.addObject(object);
    QVERIFY(object.index() >= 0);
    QVERIFY(world.objectCount() == 5);

    world.removeObjectNow(object); // Immediate removal
    QVERIFY(object.index() == -1);
    QVERIFY(world.objectCount() == 4);
}

void MCWorldTest::testInstance()
{
    QVERIFY(MCWorld::hasInstance() == false);
    {
        MCWorld world;
        QVERIFY(MCWorld::hasInstance() == true);
        QVERIFY(&MCWorld::instance() == &world);
    }
    QVERIFY(MCWorld::hasInstance() == false);
}

void MCWorldTest::testSetDimensions()
{
    const float minX = 0;
    const float maxX = 1024;
    const float minY = 0;
    const float maxY = 768;
    const float minZ = 0;
    const float maxZ = 1000;
    const float metersPerUnit = 0.4f;

    MCWorld world;
    world.setDimensions(minX, maxX, minY, maxY, minZ, maxZ, metersPerUnit);

    QVERIFY(qFuzzyCompare(world.minX(), minX));
    QVERIFY(qFuzzyCompare(world.maxX(), maxX));
    QVERIFY(qFuzzyCompare(world.minY(), minY));
    QVERIFY(qFuzzyCompare(world.maxY(), maxY));
    QVERIFY(qFuzzyCompare(world.minZ(), minZ));
    QVERIFY(qFuzzyCompare(world.maxZ(), maxZ));

    QVERIFY(qFuzzyCompare(world.metersPerUnit(), metersPerUnit));
}

void MCWorldTest::testCollisionEvent_RectRect()
{
    MCWorld world;
    world.setDimensions(-10, 10, -10, 10, -10, 10, 1, false);

    TestObject object1;
    object1.setShape(MCShapePtr(new MCRectShape(nullptr, 2.0, 2.0)));
    object1.physicsComponent().preventSleeping(true); // Need to prevent sleeping, because we are testing with zero velocity

    TestObject object2;
    object2.setShape(MCShapePtr(new MCRectShape(nullptr, 2.0, 2.0)));
    object2.physicsComponent().preventSleeping(true);

    world.addObject(object1);
    world.addObject(object2);

    object1.translate(MCVector3dF(-0.5f, 0.0f));
    object2.translate(MCVector3dF(0.5f, 0.5f));

    world.stepTime(1);

    QCOMPARE(object1.collisionEventsReceived, 1);
    QCOMPARE(object2.collisionEventsReceived, 1);

    object1.collisionEventsReceived = false;
    object2.collisionEventsReceived = false;

    world.stepTime(1);

    QVERIFY(!object1.collisionEventsReceived);
    QVERIFY(!object2.collisionEventsReceived);

    object1.translate(MCVector3dF(-1.5f, 0.0f));
    object2.translate(MCVector3dF(1.5f, 0.5f));

    world.stepTime(1);

    QCOMPARE(object1.collisionEventsReceived, 0);
    QCOMPARE(object2.collisionEventsReceived, 0);

    QCOMPARE(object1.separationEventsReceived, 1);
    QCOMPARE(object2.separationEventsReceived, 1);

    object1.translate(MCVector3dF(-0.5f, 0.0f));
    object2.translate(MCVector3dF(0.5f, 0.5f));

    world.stepTime(1);

    QCOMPARE(object1.collisionEventsReceived, 1);
    QCOMPARE(object2.collisionEventsReceived, 1);
}

void MCWorldTest::testCollisionEvent_RectCircle()
{
    MCWorld world;
    world.setDimensions(-10, 10, -10, 10, -10, 10, 1, false);

    TestObject object1;
    object1.setShape(MCShapePtr(new MCRectShape(nullptr, 2.0, 2.0)));
    object1.physicsComponent().preventSleeping(true); // Need to prevent sleeping, because we are testing with zero velocity

    TestObject object2;
    object2.setShape(MCShapePtr(new MCCircleShape(nullptr, 1.0)));
    object2.physicsComponent().preventSleeping(true);

    world.addObject(object1);
    world.addObject(object2);

    object1.translate(MCVector3dF(-0.5, 0.0));
    object2.translate(MCVector3dF(0.5, 0.0));

    world.stepTime(1);

    QCOMPARE(object1.collisionEventsReceived, 1);
    QCOMPARE(object2.collisionEventsReceived, 1);

    object1.collisionEventsReceived = false;
    object2.collisionEventsReceived = false;

    world.stepTime(1);

    QCOMPARE(object1.collisionEventsReceived, 0);
    QCOMPARE(object2.collisionEventsReceived, 0);

    object1.translate(MCVector3dF(-1.5, 0.0));
    object2.translate(MCVector3dF(1.5, 0.0));

    world.stepTime(1);

    QCOMPARE(object1.collisionEventsReceived, 0);
    QCOMPARE(object2.collisionEventsReceived, 0);

    QCOMPARE(object1.separationEventsReceived, 1);
    QCOMPARE(object2.separationEventsReceived, 1);

    object1.translate(MCVector3dF(-0.5, 0.0));
    object2.translate(MCVector3dF(0.5, 0.0));

    world.stepTime(1);

    QCOMPARE(object1.collisionEventsReceived, 1);
    QCOMPARE(object2.collisionEventsReceived, 1);
}

void MCWorldTest::testCollisionEvent_CircleCircle()
{
    MCWorld world;
    world.setDimensions(-10, 10, -10, 10, -10, 10, 1, false);

    TestObject object1;
    object1.setShape(MCShapePtr(new MCCircleShape(nullptr, 1.0)));
    object1.physicsComponent().preventSleeping(true); // Need to prevent sleeping, because we are testing with zero velocity

    TestObject object2;
    object2.setShape(MCShapePtr(new MCCircleShape(nullptr, 1.0)));
    object2.physicsComponent().preventSleeping(true);

    world.addObject(object1);
    world.addObject(object2);

    object1.translate(MCVector3dF(-0.5, 0.0));
    object2.translate(MCVector3dF(0.5, 0.0));

    world.stepTime(1);

    QCOMPARE(object1.collisionEventsReceived, 1);
    QCOMPARE(object2.collisionEventsReceived, 1);

    object1.collisionEventsReceived = false;
    object2.collisionEventsReceived = false;

    world.stepTime(1);

    QCOMPARE(object1.collisionEventsReceived, 0);
    QCOMPARE(object2.collisionEventsReceived, 0);

    object1.translate(MCVector3dF(-1.5, 0.0));
    object2.translate(MCVector3dF(1.5, 0.0));

    world.stepTime(1);

    QCOMPARE(object1.collisionEventsReceived, 0);
    QCOMPARE(object2.collisionEventsReceived, 0);

    QCOMPARE(object1.separationEventsReceived, 1);
    QCOMPARE(object2.separationEventsReceived, 1);

    object1.translate(MCVector3dF(-0.5, 0.0));
    object2.translate(MCVector3dF(0.5, 0.0));

    world.stepTime(1);

    QCOMPARE(object1.collisionEventsReceived, 1);
    QCOMPARE(object2.collisionEventsReceived, 1);
}

void MCWorldTest::testSleepingObjectRemovalFromIntegration()
{
    MCWorld world;
    MCObject object("test");
    QVERIFY(object.index() == -1);
    world.addObject(object);
    QVERIFY(object.index() >= 0);
    QVERIFY(world.objectCount() == 5); // 5 includes built-in walls

    object.physicsComponent().toggleSleep(true);
    QVERIFY(world.objectCount() == 4);

    object.physicsComponent().toggleSleep(false);
    QVERIFY(world.objectCount() == 5);
}

QTEST_GUILESS_MAIN(MCWorldTest)
