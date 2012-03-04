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
