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

#include "MCForceRegistryTest.hpp"
#include "../../Core/mcforcegenerator.hh"
#include "../../Core/mcforceregistry.hh"
#include "../../Core/mcobject.hh"
#include "../../Core/mcworld.hh"

class TestForceGenerator : public MCForceGenerator
{
public:

    TestForceGenerator()
    : m_updated(false)
    {}

    //! \reimp
    void updateForce(MCObject &)
    {
        m_updated = true;
    }

    bool m_updated;
};

MCForceRegistryTest::MCForceRegistryTest()
{
}

void MCForceRegistryTest::testAddUpdateRemove()
{
    MCForceRegistry dut;
    TestForceGenerator force;
    MCObject object("TestObject");
    MCWorld world;
    dut.addForceGenerator(force, object);
    dut.update();
    QVERIFY(force.m_updated == false);
    world.addObject(object);
    dut.update();
    QVERIFY(force.m_updated == true);
    force.m_updated = false;
    dut.removeForceGenerator(force, object);
    dut.update();
    QVERIFY(force.m_updated == false);
}

void MCForceRegistryTest::testUpdateWithEnable()
{
    MCForceRegistry dut;
    TestForceGenerator force;
    MCObject object("TestObject");
    MCWorld world;
    dut.addForceGenerator(force, object);
    world.addObject(object);
    dut.update();
    QVERIFY(force.m_updated == true);
    force.m_updated = false;
    force.enable(false);
    dut.update();
    QVERIFY(force.m_updated == false);
    force.enable(true);
    dut.update();
    QVERIFY(force.m_updated == true);
}

void MCForceRegistryTest::testClear()
{
    MCForceRegistry dut;
    TestForceGenerator force;
    MCObject object("TestObject");
    MCWorld world;
    dut.addForceGenerator(force, object);
    world.addObject(object);
    dut.clear();
    dut.update();
    QVERIFY(force.m_updated == false);
}

QTEST_MAIN(MCForceRegistryTest)
