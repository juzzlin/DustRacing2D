// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "MCObjectTest.hpp"
#include "../../Physics/mcforcegenerator.hh"
#include "../../Physics/mcforceregistry.hh"
#include "../../Core/mcworld.hh"
#include "../../Core/mcobject.hh"
#include "../../Core/mctimerevent.hh"

class TestObject : public MCObject
{
public:

    TestObject()
    : MCObject("TEST_OBJECT")
    , m_timerEventReceived(false)
    {
    }

    virtual bool event(MCEvent & event)
    {
        if (event.instanceTypeID() == MCTimerEvent::typeID())
        {
            m_timerEventReceived = true;
            return true;
        }

        return false;
    }

    bool m_timerEventReceived;
};

MCObjectTest::MCObjectTest()
{
}

void MCObjectTest::testMass()
{
    MCObject object("TestObject");
    QVERIFY(qFuzzyCompare(object.mass(), MCFloat(0.0)));

    object.setMass(MCFloat(1000.0));
    QVERIFY(qFuzzyCompare(object.mass(), MCFloat(1000.0)));
    QVERIFY(qFuzzyCompare(object.invMass(), MCFloat(1.0 / 1000.0)));

    object.setMass(1, true);
    QVERIFY(object.stationary());
    QVERIFY(object.sleeping());
    QVERIFY(qFuzzyCompare(object.invMass(), MCFloat(0.0)));
}

void MCObjectTest::testTimerEvent()
{
    TestObject testObject1, testObject2;
    QVERIFY(!testObject1.m_timerEventReceived);
    QVERIFY(!testObject2.m_timerEventReceived);

    testObject1.m_timerEventReceived = false;
    testObject2.m_timerEventReceived = false;
    MCTimerEvent timerEvent(100);
    MCObject::sendTimerEvent(timerEvent);
    QVERIFY(!testObject1.m_timerEventReceived);
    QVERIFY(!testObject2.m_timerEventReceived);

    testObject1.m_timerEventReceived = false;
    testObject2.m_timerEventReceived = false;
    MCObject::subscribeTimerEvent(testObject1);
    MCObject::subscribeTimerEvent(testObject2);
    MCObject::sendTimerEvent(timerEvent);
    QVERIFY(testObject1.m_timerEventReceived);
    QVERIFY(testObject2.m_timerEventReceived);

    testObject1.m_timerEventReceived = false;
    testObject2.m_timerEventReceived = false;
    MCObject::unsubscribeTimerEvent(testObject1);
    MCObject::unsubscribeTimerEvent(testObject2);
    MCObject::sendTimerEvent(timerEvent);
    QVERIFY(!testObject1.m_timerEventReceived);
    QVERIFY(!testObject2.m_timerEventReceived);
}

QTEST_MAIN(MCObjectTest)
