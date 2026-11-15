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

#include "MCPhysicsComponentTest.hpp"

#include "../../Core/mcobject.hh"
#include "../../Core/mctrigonom.hh"
#include "../../Core/mcvector3d.hh"
#include "../../Core/mcworld.hh"
#include "../../Physics/mcphysicscomponent.hh"
#include "../../Physics/mcrectshape.hh"

#include <cmath>
#include <limits>

// Default damping factors defined in MCPhysicsComponent
static const float LINEAR_DAMPING = 0.999f;
static const float ANGULAR_DAMPING = 0.99f;

//! Create a world large enough that nothing hits the boundaries and without area walls.
static void setupWorld(MCWorld & world)
{
    world.setDimensions(-1000, 1000, -1000, 1000, -1000, 1000, 1.0f, false);
}

MCPhysicsComponentTest::MCPhysicsComponentTest()
{
}

void MCPhysicsComponentTest::testDefaults()
{
    MCWorld world;
    setupWorld(world);

    MCObject object("TestObject");
    const auto & physics = object.physicsComponent();

    QVERIFY(qFuzzyCompare(physics.restitution(), 0.5f));
    QVERIFY(qFuzzyIsNull(physics.mass()));
    QVERIFY(qFuzzyIsNull(physics.angularVelocity()));
    QVERIFY(qFuzzyIsNull(physics.xyFriction()));
    QVERIFY(!physics.isSleeping());
    QVERIFY(!physics.isStationary());
    QVERIFY(!physics.isIntegrating());
    QCOMPARE(physics.collisionTag(), 0);
    QCOMPARE(physics.neverCollideWithTag(), -1);
}

void MCPhysicsComponentTest::testRestitutionIsClamped()
{
    MCWorld world;
    setupWorld(world);

    MCObject object("TestObject");
    auto & physics = object.physicsComponent();

    physics.setRestitution(-1.0f);
    QVERIFY(qFuzzyIsNull(physics.restitution()));

    physics.setRestitution(2.0f);
    QVERIFY(qFuzzyCompare(physics.restitution(), 1.0f));

    physics.setRestitution(0.25f);
    QVERIFY(qFuzzyCompare(physics.restitution(), 0.25f));
}

void MCPhysicsComponentTest::testMassAndInverseMass()
{
    MCWorld world;
    setupWorld(world);

    MCObject object("TestObject");
    auto & physics = object.physicsComponent();

    physics.setMass(4.0f);

    QVERIFY(qFuzzyCompare(physics.mass(), 4.0f));
    QVERIFY(qFuzzyCompare(physics.invMass(), 0.25f));

    // setMass() also sets a default guess for the moment of inertia
    QVERIFY(qFuzzyCompare(physics.momentOfInertia(), 40.0f));

    // A zero mass yields a *huge* inverse mass instead of the conventional zero
    // (== immovable), so a single addForce() call blows the object up to infinity.
    // Note that this is the state of every freshly constructed physics component.
    physics.setMass(0.0f);
    QVERIFY(qFuzzyIsNull(physics.mass()));

    QEXPECT_FAIL("", "Zero mass should mean immovable (invMass == 0), not infinitely light", Continue);
    QVERIFY(qFuzzyIsNull(physics.invMass()));
}

void MCPhysicsComponentTest::testStationaryMass()
{
    MCWorld world;
    setupWorld(world);

    MCObject object("TestObject");
    auto & physics = object.physicsComponent();

    physics.setMass(5.0f, true);

    QVERIFY(physics.isStationary());
    QVERIFY(physics.isSleeping());
    QVERIFY(qFuzzyIsNull(physics.invMass()));
    QVERIFY(qFuzzyCompare(physics.mass(), std::numeric_limits<float>::max()));
    QVERIFY(qFuzzyIsNull(physics.invMomentOfInertia()));
}

void MCPhysicsComponentTest::testMomentOfInertia()
{
    MCWorld world;
    setupWorld(world);

    MCObject object("TestObject");
    auto & physics = object.physicsComponent();

    physics.setMomentOfInertia(8.0f);
    QVERIFY(qFuzzyCompare(physics.momentOfInertia(), 8.0f));
    QVERIFY(qFuzzyCompare(physics.invMomentOfInertia(), 0.125f));

    physics.setMomentOfInertia(0.0f);
    QVERIFY(qFuzzyIsNull(physics.momentOfInertia()));
    QVERIFY(qFuzzyCompare(physics.invMomentOfInertia(), std::numeric_limits<float>::max()));
}

void MCPhysicsComponentTest::testForceCausesAcceleration()
{
    MCWorld world;
    setupWorld(world);

    MCObject object("TestObject");
    object.addToWorld();

    auto & physics = object.physicsComponent();
    physics.setMass(2.0f);
    physics.addForce(MCVector3dF(4.0f, 0, 0));

    // a = F / m = 2, dt = 1 s
    world.stepTime(1000);

    QVERIFY(qFuzzyCompare(physics.velocity().i(), 2.0f * LINEAR_DAMPING));
    QVERIFY(qFuzzyIsNull(physics.velocity().j()));
}

void MCPhysicsComponentTest::testForcesAreClearedAfterIntegration()
{
    MCWorld world;
    setupWorld(world);

    MCObject object("TestObject");
    object.addToWorld();

    auto & physics = object.physicsComponent();
    physics.setMass(2.0f);
    physics.addForce(MCVector3dF(4.0f, 0, 0));

    world.stepTime(1000);
    const float velocityAfterForce = physics.velocity().i();

    // The force was added for a single frame only: the second step must not accelerate further.
    world.stepTime(1000);

    QVERIFY(qFuzzyCompare(physics.velocity().i(), velocityAfterForce * LINEAR_DAMPING));
}

void MCPhysicsComponentTest::testForcesAreClearedWhenBelowSleepLimit()
{
    MCWorld world;
    setupWorld(world);

    MCObject object("TestObject");
    object.addToWorld();

    auto & physics = object.physicsComponent();
    physics.setMass(1.0f);

    // Sleep limits are raised so that the object stays in the "too slow to move" branch
    // of MCPhysicsComponent::integrate(). addForce() keeps resetting the sleep counter,
    // so the object never actually falls asleep.
    physics.setSleepLimits(1.0f, 1.0f);

    physics.addForce(MCVector3dF(0.1f, 0, 0));
    world.stepTime(100);

    const float velocityAfterFirstStep = 0.1f * 0.1f * LINEAR_DAMPING;
    QVERIFY(qFuzzyCompare(physics.velocity().i(), velocityAfterFirstStep));

    physics.addForce(MCVector3dF(0.1f, 0, 0));
    world.stepTime(100);

    // The slow branch never calls m_forces.setZero(), so the force from the first step
    // is still in the accumulator and gets applied a second time.
    const float expected = (velocityAfterFirstStep + 0.1f * 0.1f) * LINEAR_DAMPING;

    QVERIFY(qFuzzyCompare(physics.velocity().i(), expected));
}

void MCPhysicsComponentTest::testTorqueCausesAngularAcceleration()
{
    MCWorld world;
    setupWorld(world);

    MCObject object("TestObject");
    object.setShape(MCShapePtr(new MCRectShape(nullptr, 10, 10)));
    object.addToWorld();

    auto & physics = object.physicsComponent();
    physics.setMomentOfInertia(2.0f);
    physics.addTorque(4.0f);

    // alpha = T / I = 2, dt = 1 s
    world.stepTime(1000);

    QVERIFY(qFuzzyCompare(physics.angularVelocity(), 2.0f * ANGULAR_DAMPING));

    // Torque is cleared by integrateAngular() on every step.
    const float angularVelocity = physics.angularVelocity();
    world.stepTime(1000);
    QVERIFY(qFuzzyCompare(physics.angularVelocity(), angularVelocity * ANGULAR_DAMPING));
}

void MCPhysicsComponentTest::testOffCenterForceCausesTorque()
{
    MCWorld world;
    setupWorld(world);

    MCObject object("TestObject");
    object.setShape(MCShapePtr(new MCRectShape(nullptr, 10, 10)));
    object.addToWorld();

    auto & physics = object.physicsComponent();
    physics.setMass(1.0f);
    physics.setMomentOfInertia(1.0f);

    // A force perpendicular to a lever arm of 2 units => torque of 2 Nm.
    physics.addForce(MCVector3dF(0, 1.0f, 0), object.location() + MCVector3dF(2.0f, 0, 0));

    world.stepTime(1000);

    QVERIFY(qFuzzyCompare(physics.angularVelocity(), 2.0f * ANGULAR_DAMPING));

    // The linear part of the force is applied in full as well.
    QVERIFY(qFuzzyCompare(physics.velocity().j(), 1.0f * LINEAR_DAMPING));
}

void MCPhysicsComponentTest::testLinearImpulseIsAVelocityDelta()
{
    MCWorld world;
    setupWorld(world);

    // A linear impulse is documented as "the velocity component of the impulse",
    // i.e. it is *not* scaled by the inverse mass. Heavy and light objects react alike.
    MCObject light("Light");
    light.addToWorld();
    light.physicsComponent().setMass(1.0f);
    light.physicsComponent().addImpulse(MCVector3dF(1.0f, 0, 0));

    MCObject heavy("Heavy");
    heavy.addToWorld();
    heavy.physicsComponent().setMass(1000.0f);
    heavy.physicsComponent().addImpulse(MCVector3dF(1.0f, 0, 0));

    world.stepTime(1000);

    QVERIFY(qFuzzyCompare(light.physicsComponent().velocity().i(), 1.0f * LINEAR_DAMPING));
    QVERIFY(qFuzzyCompare(heavy.physicsComponent().velocity().i(), 1.0f * LINEAR_DAMPING));
}

void MCPhysicsComponentTest::testAngularImpulseIgnoresMomentOfInertia()
{
    MCWorld world;
    setupWorld(world);

    MCObject light("Light");
    light.setShape(MCShapePtr(new MCRectShape(nullptr, 10, 10)));
    light.addToWorld();
    light.physicsComponent().setMomentOfInertia(1.0f);
    light.physicsComponent().addAngularImpulse(1.0f);

    MCObject heavy("Heavy");
    heavy.setShape(MCShapePtr(new MCRectShape(nullptr, 10, 10)));
    heavy.addToWorld();
    heavy.physicsComponent().setMomentOfInertia(100.0f);
    heavy.physicsComponent().addAngularImpulse(1.0f);

    world.stepTime(1000);

    QVERIFY(qFuzzyCompare(light.physicsComponent().angularVelocity(), 1.0f * ANGULAR_DAMPING));

    // integrateAngular() adds m_angularImpulse straight to the angular velocity without
    // scaling it by the inverse moment of inertia, so a hundred times heavier object
    // spins up exactly as fast. Contrast with addTorque(), which does scale correctly.
    QEXPECT_FAIL("", "Angular impulses are not scaled by the inverse moment of inertia", Continue);
    QVERIFY(qFuzzyCompare(heavy.physicsComponent().angularVelocity(), 0.01f * ANGULAR_DAMPING));
}

void MCPhysicsComponentTest::testOffCenterImpulseIgnoresLeverArm()
{
    MCWorld world;
    setupWorld(world);

    MCObject nearHit("NearHit");
    nearHit.setShape(MCShapePtr(new MCRectShape(nullptr, 10, 10)));
    nearHit.addToWorld();
    nearHit.physicsComponent().setMomentOfInertia(1.0f);
    nearHit.physicsComponent().addImpulse(MCVector3dF(0, 1.0f, 0), nearHit.location() + MCVector3dF(1.0f, 0, 0));

    MCObject farHit("FarHit");
    farHit.setShape(MCShapePtr(new MCRectShape(nullptr, 10, 10)));
    farHit.addToWorld();
    farHit.physicsComponent().setMomentOfInertia(1.0f);
    farHit.physicsComponent().addImpulse(MCVector3dF(0, 1.0f, 0), farHit.location() + MCVector3dF(10.0f, 0, 0));

    world.stepTime(1000);

    QVERIFY(qFuzzyCompare(nearHit.physicsComponent().angularVelocity(), 1.0f * ANGULAR_DAMPING));

    // addImpulse() computes (impulse x arm) and then divides by |arm|, which cancels the
    // lever arm out completely: hitting the very edge of an object spins it exactly as
    // much as hitting right next to its center of mass.
    QEXPECT_FAIL("", "The lever arm is divided out of the angular impulse", Continue);
    QVERIFY(qFuzzyCompare(farHit.physicsComponent().angularVelocity(), 10.0f * ANGULAR_DAMPING));
}

void MCPhysicsComponentTest::testMaxSpeedIsClamped()
{
    MCWorld world;
    setupWorld(world);

    MCObject axisAligned("AxisAligned");
    axisAligned.addToWorld();
    axisAligned.physicsComponent().setLinearDamping(1.0f);
    axisAligned.physicsComponent().setMaxSpeed(2.0f);
    axisAligned.physicsComponent().setVelocity(MCVector3dF(10.0f, 0, 0));

    MCObject diagonal("Diagonal");
    diagonal.addToWorld();
    diagonal.physicsComponent().setLinearDamping(1.0f);
    diagonal.physicsComponent().setMaxSpeed(2.0f);
    diagonal.physicsComponent().setVelocity(MCVector3dF(10.0f, 10.0f, 0));

    world.stepTime(1000);

    QVERIFY(qFuzzyCompare(axisAligned.physicsComponent().velocity().length(), 2.0f));

    // The clamp uses lengthFast(), which is the "max + min / 2" approximation. That
    // over-estimates the length of a diagonal vector by ~6 %, so the effective maximum
    // speed depends on the direction of travel.
    QEXPECT_FAIL("", "clampFast() uses an approximate length, so max speed is direction dependent", Continue);
    QVERIFY(qFuzzyCompare(diagonal.physicsComponent().velocity().length(), 2.0f));
}

void MCPhysicsComponentTest::testSleepOnLowLinearVelocity()
{
    MCWorld world;
    setupWorld(world);

    MCObject object("TestObject");
    object.addToWorld();
    object.physicsComponent().setVelocity(MCVector3dF(0.001f, 0, 0));

    QVERIFY(!object.physicsComponent().isSleeping());

    world.stepTime(1);
    world.stepTime(1);

    QVERIFY(object.physicsComponent().isSleeping());
}

void MCPhysicsComponentTest::testSleepIgnoresNegativeAngularVelocity()
{
    MCWorld world;
    setupWorld(world);

    MCObject counterClockwise("CounterClockwise");
    counterClockwise.setShape(MCShapePtr(new MCRectShape(nullptr, 10, 10)));
    counterClockwise.addToWorld();
    counterClockwise.physicsComponent().setMomentOfInertia(1.0f);
    counterClockwise.physicsComponent().setAngularVelocity(5.0f);

    MCObject clockwise("Clockwise");
    clockwise.setShape(MCShapePtr(new MCRectShape(nullptr, 10, 10)));
    clockwise.addToWorld();
    clockwise.physicsComponent().setMomentOfInertia(1.0f);
    clockwise.physicsComponent().setAngularVelocity(-5.0f);

    world.stepTime(100);

    QVERIFY(!qFuzzyIsNull(counterClockwise.angle()));

    // The sleep test in integrate() reads "m_angularVelocity < m_angularSleepLimit"
    // without taking the absolute value, so any object spinning clockwise passes it.
    // Its rotation is silently dropped and it freezes mid-spin.
    QEXPECT_FAIL("", "The angular sleep limit is compared without std::abs()", Abort);
    QVERIFY(!qFuzzyIsNull(clockwise.angle()));
}

void MCPhysicsComponentTest::testSpeedIsSignedAndLongitudinal()
{
    MCWorld world;
    setupWorld(world);

    MCObject object("TestObject");
    object.addToWorld();

    // speed() projects the velocity onto the heading: it is signed and it ignores
    // any sideways component. It is not the magnitude of the velocity.
    object.physicsComponent().setVelocity(MCVector3dF(-3.0f, 0, 0));
    QVERIFY(qFuzzyCompare(object.physicsComponent().speed(), -3.0f));

    // Note: not exactly zero, because MCTrigonom::sin(0) is not exactly zero.
    // See MCTrigonomTest::testCardinalAnglesAreExact().
    object.physicsComponent().setVelocity(MCVector3dF(0, 5.0f, 0));
    QVERIFY(std::fabs(object.physicsComponent().speed()) < 0.001f);

    object.rotate(90);
    QVERIFY(std::fabs(object.physicsComponent().speed() - 5.0f) < 0.001f);
}

void MCPhysicsComponentTest::testPositionIntegrationIsTimeStepIndependent()
{
    MCWorld world;
    setupWorld(world);

    MCObject oneBigStep("OneBigStep");
    oneBigStep.addToWorld();
    oneBigStep.physicsComponent().setLinearDamping(1.0f);
    oneBigStep.physicsComponent().setVelocity(MCVector3dF(1.0f, 0, 0));

    MCObject tenSmallSteps("TenSmallSteps");
    tenSmallSteps.addToWorld();
    tenSmallSteps.physicsComponent().setLinearDamping(1.0f);
    tenSmallSteps.physicsComponent().setVelocity(MCVector3dF(1.0f, 0, 0));

    world.stepTime(1000);
    for (int i = 0; i < 9; i++)
    {
        // Only the second object gets the remaining nine 100 ms steps.
        oneBigStep.physicsComponent().toggleSleep(true);
        world.stepTime(100);
        oneBigStep.physicsComponent().toggleSleep(false);
    }

    // integrate() does "translate(location + velocity)" without multiplying the velocity
    // by the time step, so the distance travelled depends on the *number of frames*, not
    // on the elapsed time. Since Game switched to a wall-clock time step this means the
    // whole simulation (speeds, accelerations, the speedometer, every speed threshold in
    // Car and AI) scales with the frame rate.
    QEXPECT_FAIL("", "Position is integrated per frame instead of per second", Continue);
    QVERIFY(qFuzzyCompare(oneBigStep.location().i(), tenSmallSteps.location().i()));
}

void MCPhysicsComponentTest::testResetPropagatesToChildren()
{
    MCWorld world;
    setupWorld(world);

    MCObject parent("Parent");
    MCObjectPtr child(new MCObject("Child"));
    parent.addChildObject(child, MCVector3dF(1.0f, 0, 0));
    parent.addToWorld();

    parent.physicsComponent().setVelocity(MCVector3dF(1.0f, 2.0f, 0));
    child->physicsComponent().setVelocity(MCVector3dF(3.0f, 4.0f, 0));
    parent.physicsComponent().setAcceleration(MCVector3dF(5.0f, 0, 0));

    parent.physicsComponent().reset();

    QVERIFY(qFuzzyIsNull(parent.physicsComponent().velocity().lengthSquared()));
    QVERIFY(qFuzzyIsNull(child->physicsComponent().velocity().lengthSquared()));

    // reset() also wipes the *constant* acceleration, which setAcceleration() documents
    // as the way to give an object its own gravity. Falling asleep for a single frame
    // therefore removes that gravity permanently.
    QEXPECT_FAIL("", "reset() clears the constant acceleration set via setAcceleration()", Continue);
    QVERIFY(qFuzzyCompare(parent.physicsComponent().acceleration().i(), 5.0f));
}

QTEST_GUILESS_MAIN(MCPhysicsComponentTest)
