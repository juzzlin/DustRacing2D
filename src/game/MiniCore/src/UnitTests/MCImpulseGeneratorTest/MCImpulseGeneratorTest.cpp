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

#include "MCImpulseGeneratorTest.hpp"

#include "../../Core/mcobject.hh"
#include "../../Core/mcvector3d.hh"
#include "../../Core/mcworld.hh"
#include "../../Physics/mccircleshape.hh"
#include "../../Physics/mccollisiondetector.hh"
#include "../../Physics/mccontact.hh"
#include "../../Physics/mcphysicscomponent.hh"
#include "../../Physics/mcrectshape.hh"

#include <cmath>

//! Tolerance used when comparing velocities that went through the contact solver.
static const float TOLERANCE = 0.0001f;

//! Create a world large enough that nothing hits the boundaries and without area walls.
static void setupWorld(MCWorld & world)
{
    world.setDimensions(-1000, 1000, -1000, 1000, -1000, 1000, 1.0f, false);
}

/*! Prepare an object for a controlled head-on collision test: known mass, no damping
 *  (so that the numbers stay exact) and a known restitution. */
static void setupBody(MCObject & object, MCWorld & world, float mass, float restitution)
{
    object.physicsComponent().setMass(mass);
    object.physicsComponent().setRestitution(restitution);
    object.physicsComponent().setLinearDamping(1.0f);
    world.addObject(object);
}

static float momentumX(MCObject & a, MCObject & b)
{
    return a.physicsComponent().mass() * a.physicsComponent().velocity().i()
      + b.physicsComponent().mass() * b.physicsComponent().velocity().i();
}

MCImpulseGeneratorTest::MCImpulseGeneratorTest()
{
}

void MCImpulseGeneratorTest::testHeadOnCollisionConservesMomentum()
{
    MCWorld world;
    setupWorld(world);

    // The taller right hand body guarantees that the corners of the left hand body end
    // up strictly inside it, so contacts are generated unambiguously.
    MCObject left(MCShapePtr(new MCRectShape(nullptr, 2, 2)), "Left");
    setupBody(left, world, 1.0f, 0.0f);

    MCObject right(MCShapePtr(new MCRectShape(nullptr, 2, 4)), "Right");
    setupBody(right, world, 3.0f, 0.0f);

    left.translate(MCVector3dF(-0.9f, 0, 0));
    right.translate(MCVector3dF(0.9f, 0, 0));

    left.physicsComponent().setVelocity(MCVector3dF(0.1f, 0, 0));
    right.physicsComponent().setVelocity(MCVector3dF(-0.1f, 0, 0));

    const float momentumBefore = momentumX(left, right);

    // The first step detects the collision and stores the impulses, the second one
    // applies them to the velocities.
    world.stepTime(1);
    world.stepTime(1);

    QVERIFY(std::fabs(momentumX(left, right) - momentumBefore) < TOLERANCE);

    // Restitution zero: the bodies end up moving together.
    QVERIFY(std::fabs(left.physicsComponent().velocity().i() - right.physicsComponent().velocity().i()) < TOLERANCE);
}

void MCImpulseGeneratorTest::testMassRatioDeterminesVelocityChange()
{
    MCWorld world;
    setupWorld(world);

    MCObject light(MCShapePtr(new MCRectShape(nullptr, 2, 2)), "Light");
    setupBody(light, world, 1.0f, 0.0f);

    MCObject heavy(MCShapePtr(new MCRectShape(nullptr, 2, 4)), "Heavy");
    setupBody(heavy, world, 9.0f, 0.0f);

    light.translate(MCVector3dF(-0.9f, 0, 0));
    heavy.translate(MCVector3dF(0.9f, 0, 0));

    light.physicsComponent().setVelocity(MCVector3dF(0.1f, 0, 0));
    heavy.physicsComponent().setVelocity(MCVector3dF(-0.1f, 0, 0));

    world.stepTime(1);
    world.stepTime(1);

    const float lightDelta = std::fabs(light.physicsComponent().velocity().i() - 0.1f);
    const float heavyDelta = std::fabs(heavy.physicsComponent().velocity().i() + 0.1f);

    // The light body must take nine times the velocity change of the nine times
    // heavier one.
    QVERIFY(heavyDelta > 0.0f);
    QVERIFY(std::fabs(lightDelta / heavyDelta - 9.0f) < 0.01f);
}

void MCImpulseGeneratorTest::testRestitutionScalesSeparationSpeed()
{
    // Fully inelastic: equal masses approaching each other both come to a stop.
    {
        MCWorld world;
        setupWorld(world);

        MCObject left(MCShapePtr(new MCRectShape(nullptr, 2, 2)), "Left");
        setupBody(left, world, 1.0f, 0.0f);

        MCObject right(MCShapePtr(new MCRectShape(nullptr, 2, 4)), "Right");
        setupBody(right, world, 1.0f, 0.0f);

        left.translate(MCVector3dF(-0.9f, 0, 0));
        right.translate(MCVector3dF(0.9f, 0, 0));

        left.physicsComponent().setVelocity(MCVector3dF(0.1f, 0, 0));
        right.physicsComponent().setVelocity(MCVector3dF(-0.1f, 0, 0));

        world.stepTime(1);
        world.stepTime(1);

        QVERIFY(std::fabs(left.physicsComponent().velocity().i()) < TOLERANCE);
        QVERIFY(std::fabs(right.physicsComponent().velocity().i()) < TOLERANCE);
    }

    // Fully elastic: equal masses swap their velocities.
    {
        MCWorld world;
        setupWorld(world);

        MCObject left(MCShapePtr(new MCRectShape(nullptr, 2, 2)), "Left");
        setupBody(left, world, 1.0f, 1.0f);

        MCObject right(MCShapePtr(new MCRectShape(nullptr, 2, 4)), "Right");
        setupBody(right, world, 1.0f, 1.0f);

        left.translate(MCVector3dF(-0.9f, 0, 0));
        right.translate(MCVector3dF(0.9f, 0, 0));

        left.physicsComponent().setVelocity(MCVector3dF(0.1f, 0, 0));
        right.physicsComponent().setVelocity(MCVector3dF(-0.1f, 0, 0));

        world.stepTime(1);
        world.stepTime(1);

        QVERIFY(std::fabs(left.physicsComponent().velocity().i() + 0.1f) < TOLERANCE);
        QVERIFY(std::fabs(right.physicsComponent().velocity().i() - 0.1f) < TOLERANCE);
    }
}

void MCImpulseGeneratorTest::testStationaryObjectDoesNotMove()
{
    MCWorld world;
    setupWorld(world);

    MCObject ball(MCShapePtr(new MCRectShape(nullptr, 2, 2)), "Ball");
    setupBody(ball, world, 1.0f, 1.0f);

    MCObject wall(MCShapePtr(new MCRectShape(nullptr, 2, 4)), "Wall");
    wall.physicsComponent().setRestitution(1.0f);
    wall.physicsComponent().setMass(1.0f, true); // Stationary
    world.addObject(wall);

    ball.translate(MCVector3dF(-0.9f, 0, 0));
    wall.translate(MCVector3dF(0.9f, 0, 0));

    ball.physicsComponent().setVelocity(MCVector3dF(0.1f, 0, 0));

    const MCVector3dF wallLocation = wall.location();

    world.stepTime(1);
    world.stepTime(1);

    // A fully elastic bounce off an immovable wall reverses the velocity..
    QVERIFY(std::fabs(ball.physicsComponent().velocity().i() + 0.1f) < TOLERANCE);

    // ..and leaves the wall exactly where it was.
    QVERIFY(std::fabs(wall.physicsComponent().velocity().lengthSquared()) < TOLERANCE);
    QVERIFY(std::fabs(wall.location().i() - wallLocation.i()) < TOLERANCE);
    QVERIFY(std::fabs(wall.location().j() - wallLocation.j()) < TOLERANCE);
}

void MCImpulseGeneratorTest::testSeparatingObjectsGetNoImpulse()
{
    MCWorld world;
    setupWorld(world);

    MCObject left(MCShapePtr(new MCRectShape(nullptr, 2, 2)), "Left");
    setupBody(left, world, 1.0f, 1.0f);

    MCObject right(MCShapePtr(new MCRectShape(nullptr, 2, 4)), "Right");
    setupBody(right, world, 1.0f, 1.0f);

    left.translate(MCVector3dF(-0.9f, 0, 0));
    right.translate(MCVector3dF(0.9f, 0, 0));

    // Overlapping, but already moving apart: the solver must not add any impulse,
    // otherwise overlapping objects would be launched away from each other.
    left.physicsComponent().setVelocity(MCVector3dF(-0.1f, 0, 0));
    right.physicsComponent().setVelocity(MCVector3dF(0.1f, 0, 0));

    world.stepTime(1);
    world.stepTime(1);

    QVERIFY(std::fabs(left.physicsComponent().velocity().i() + 0.1f) < TOLERANCE);
    QVERIFY(std::fabs(right.physicsComponent().velocity().i() - 0.1f) < TOLERANCE);
}

void MCImpulseGeneratorTest::testInterpenetrationIsResolved()
{
    MCWorld world;
    setupWorld(world);

    MCObject left(MCShapePtr(new MCRectShape(nullptr, 2, 2)), "Left");
    setupBody(left, world, 1.0f, 0.0f);
    left.physicsComponent().preventSleeping(true);

    MCObject right(MCShapePtr(new MCRectShape(nullptr, 2, 4)), "Right");
    setupBody(right, world, 1.0f, 0.0f);
    right.physicsComponent().preventSleeping(true);

    left.translate(MCVector3dF(-0.9f, 0, 0));
    right.translate(MCVector3dF(0.9f, 0, 0));

    const float distanceBefore = right.location().i() - left.location().i();
    QVERIFY(distanceBefore < 2.0f); // The bodies overlap by 0.2 units

    for (int i = 0; i < 10; i++)
    {
        world.stepTime(1);
    }

    // The position resolver must push the bodies apart even though neither of them moves.
    const float distanceAfter = right.location().i() - left.location().i();
    QVERIFY(distanceAfter > distanceBefore);
}

void MCImpulseGeneratorTest::testCircleCircleContactRefersToTheOtherObject()
{
    MCWorld world;
    setupWorld(world);

    MCObject first(MCShapePtr(new MCCircleShape(nullptr, 1.0f)), "First");
    setupBody(first, world, 1.0f, 1.0f);

    MCObject second(MCShapePtr(new MCCircleShape(nullptr, 1.0f)), "Second");
    setupBody(second, world, 1.0f, 1.0f);

    first.translate(MCVector3dF(-0.5f, 0, 0));
    second.translate(MCVector3dF(0.5f, 0, 0));

    // Run the detector directly, because MCWorld deletes the contacts as soon as it has
    // consumed them.
    MCCollisionDetector detector;
    detector.detectCollisions(world.objectGrid());

    QCOMPARE(second.contacts().size(), size_t(1));
    QVERIFY(&second.contacts().begin()->second.front()->object() == &first);

    QCOMPARE(first.contacts().size(), size_t(1));

    // testCircleAgainstCircle() initialises *both* contacts with circle1.parent(), so the
    // first object ends up with a contact against itself instead of against the second
    // one. Everything downstream (relative velocity, restitution, displacement) is then
    // computed against the object's own state and cancels out, and the first object's
    // record of which object it actually touched is lost.
    //
    // Compare with testRectAgainstRect(), which correctly passes rect2.parent() and
    // rect1.parent() to the two contacts.
    QEXPECT_FAIL("", "A circle-circle contact of the first object points back at itself", Abort);
    QVERIFY(&first.contacts().begin()->second.front()->object() == &second);
}

void MCImpulseGeneratorTest::testCircleCircleCollisionConservesMomentum()
{
    MCWorld world;
    setupWorld(world);

    MCObject first(MCShapePtr(new MCCircleShape(nullptr, 1.0f)), "First");
    setupBody(first, world, 1.0f, 0.0f);

    MCObject second(MCShapePtr(new MCCircleShape(nullptr, 1.0f)), "Second");
    setupBody(second, world, 1.0f, 0.0f);

    first.translate(MCVector3dF(-0.9f, 0, 0));
    second.translate(MCVector3dF(0.9f, 0, 0));

    first.physicsComponent().setVelocity(MCVector3dF(0.1f, 0, 0));
    second.physicsComponent().setVelocity(MCVector3dF(-0.1f, 0, 0));

    const float momentumBefore = momentumX(first, second);

    world.stepTime(1);
    world.stepTime(1);

    // Momentum survives despite the self-referring contact of the first object: the
    // solver applies the impulse to *both* bodies when it processes the second object's
    // (correctly initialised) contact, and the first object's bogus self-contact yields a
    // zero relative velocity and so contributes nothing. Position resolution cancels out
    // in the same way.
    //
    // This test pins that down so that fixing the contact identity does not accidentally
    // start applying the impulse twice.
    QVERIFY(std::fabs(momentumX(first, second) - momentumBefore) < TOLERANCE);

    // Restitution zero: the bodies end up moving together.
    QVERIFY(std::fabs(first.physicsComponent().velocity().i() - second.physicsComponent().velocity().i()) < TOLERANCE);
}

QTEST_GUILESS_MAIN(MCImpulseGeneratorTest)
