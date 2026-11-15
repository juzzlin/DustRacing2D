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

#include "MCCollisionResponseTest.hpp"

#include "../../Core/mcobject.hh"
#include "../../Core/mcvector3d.hh"
#include "../../Core/mcworld.hh"
#include "../../Physics/mcphysicscomponent.hh"
#include "../../Physics/mcrectshape.hh"

#include <cmath>
#include <memory>

namespace {

//! Time step in milliseconds, as in the game at 60 fps.
const int STEP = 16;

const float DT = STEP / 1000.0f;

class Scene
{
public:
    Scene()
    {
        m_world.setDimensions(-1000, 1000, -1000, 1000, -1000, 1000, 1.0f, false);
    }

    //! A static wall whose top face is at y = 5.
    MCObject & addWall()
    {
        auto && wall = add(100, 10, { 0, 0 }, 0, 1, 0);
        wall.physicsComponent().setMass(1, true);
        return wall;
    }

    //! A free box with the moment of inertia of a uniform box and no damping.
    MCObject & addBox(float width, float height, MCVector2dF location, float angle, float mass, float restitution)
    {
        return add(width, height, location, angle, mass, restitution);
    }

    void step(int count = 1)
    {
        for (int i = 0; i < count; i++)
        {
            m_world.stepTime(STEP);
        }
    }

private:
    MCObject & add(float width, float height, MCVector2dF location, float angle, float mass, float restitution)
    {
        const auto object = std::make_shared<MCObject>(std::make_shared<MCRectShape>(nullptr, width, height), "TestBox");
        object->physicsComponent().setMass(mass);
        object->physicsComponent().setMomentOfInertia(mass * (width * width + height * height) / 12);
        object->physicsComponent().setRestitution(restitution);
        object->physicsComponent().setLinearDamping(1.0f);
        object->physicsComponent().setAngularDamping(1.0f);
        object->rotate(angle);
        object->translate(MCVector3dF(location));
        m_world.addObject(*object);
        m_objects.push_back(object);
        return *object;
    }

    MCWorld m_world;

    std::vector<std::shared_ptr<MCObject>> m_objects;
};

//! Linear velocity is in world units per step, angular velocity in radians per second.
float kineticEnergy(MCObject & object)
{
    const auto & physics = object.physicsComponent();
    const float angularVelocityPerStep = physics.angularVelocity() * DT;
    return 0.5f * physics.mass() * physics.velocity().lengthSquared() + 0.5f * physics.momentOfInertia() * angularVelocityPerStep * angularVelocityPerStep;
}

//! Lowest corner of a 8x4 box rotated 30 degrees counterclockwise, relative to its center.
const MCVector2dF tiltedBoxLowestCorner { -4 * std::cos(0.5236f) + 2 * std::sin(0.5236f), -4 * std::sin(0.5236f) - 2 * std::cos(0.5236f) };

} // namespace

void MCCollisionResponseTest::testFlatImpactDoesNotInduceSpin()
{
    // A box falls flat on a wall: the impulse is symmetric, so the box must not start to spin.
    Scene scene;
    scene.addWall();
    auto && box = scene.addBox(8, 4, { 0, 7.1f }, 0, 1, 0.5f);
    box.physicsComponent().setVelocity({ 0, -0.5f, 0 });

    scene.step(5);

    QEXPECT_FAIL("", "Only the deepest contact point is used", Abort);
    QVERIFY(std::fabs(box.physicsComponent().angularVelocity()) < 0.01f);
    QVERIFY(std::fabs(box.angle()) < 0.5f);
}

void MCCollisionResponseTest::testFlatImpactStopsInelasticBody()
{
    Scene scene;
    scene.addWall();
    auto && box = scene.addBox(8, 4, { 3, 7.1f }, 0, 1, 0);
    box.physicsComponent().setVelocity({ 0.2f, -0.5f, 0 });

    scene.step(5);

    // No bounce, no sinking into the wall, and no friction yet: the sideways motion stays.
    QVERIFY(std::fabs(box.physicsComponent().velocity().j()) < 0.02f);
    QVERIFY(box.location().j() > 6.5f);
    QVERIFY(std::fabs(box.physicsComponent().velocity().i() - 0.2f) < 0.02f);
}

void MCCollisionResponseTest::testOffCenterImpactSpinsTheBodyTheRightWay()
{
    // A tilted box hits the wall with its lowest corner, which is left of its center.
    // The wall pushes that corner up, so the box starts to rotate clockwise.
    Scene scene;
    scene.addWall();
    auto && box = scene.addBox(8, 4, { 0, 5 - tiltedBoxLowestCorner.j() + 0.05f }, 30, 1, 0);
    box.physicsComponent().setVelocity({ 0, -0.5f, 0 });

    scene.step(3);

    QVERIFY(box.physicsComponent().angularVelocity() < 0);
}

void MCCollisionResponseTest::testOffCenterImpactLeavesSomeLinearMotion()
{
    // When the wall stops the corner, part of the impulse goes to the rotation,
    // so the center of the box doesn't come to a full stop.
    Scene scene;
    scene.addWall();
    auto && box = scene.addBox(8, 4, { 0, 5 - tiltedBoxLowestCorner.j() + 0.05f }, 30, 1, 0);
    box.physicsComponent().setVelocity({ 0, -0.5f, 0 });

    scene.step(3);

    const float vy = box.physicsComponent().velocity().j();
    QEXPECT_FAIL("", "Impulses ignore the rotational inertia", Abort);
    QVERIFY(vy < -0.05f);
    QVERIFY(vy > -0.5f);
}

void MCCollisionResponseTest::testElasticOffCenterImpactConservesEnergy()
{
    Scene scene;
    scene.addWall();
    auto && box = scene.addBox(8, 4, { 0, 5 - tiltedBoxLowestCorner.j() + 0.05f }, 30, 1, 1);
    box.physicsComponent().setVelocity({ 0, -0.5f, 0 });
    const float energyBefore = kineticEnergy(box);

    scene.step(3);

    const float energyAfter = kineticEnergy(box);
    QVERIFY(energyAfter < energyBefore * 1.02f);
    QVERIFY(energyAfter > energyBefore * 0.9f);
}

void MCCollisionResponseTest::testObliqueCollisionConservesMomentum()
{
    // Two free boxes hit each other off center and at an angle
    Scene scene;
    auto && a = scene.addBox(8, 4, { 0, 0 }, 0, 1, 0.5f);
    auto && b = scene.addBox(8, 4, { 7.9f, 2.5f }, 20, 2, 0.5f);
    a.physicsComponent().setVelocity({ 0.3f, 0.1f, 0 });
    b.physicsComponent().setVelocity({ -0.2f, 0, 0 });

    const auto momentum = [&]() {
        return a.physicsComponent().velocity() * a.physicsComponent().mass() + b.physicsComponent().velocity() * b.physicsComponent().mass();
    };
    const auto before { momentum() };

    scene.step(3);

    const auto after { momentum() };
    QVERIFY(std::fabs(after.i() - before.i()) < 0.001f);
    QVERIFY(std::fabs(after.j() - before.j()) < 0.001f);
    // They really did collide
    QVERIFY(std::fabs(a.physicsComponent().velocity().i() - 0.3f) > 0.01f);
}

void MCCollisionResponseTest::testBoxPushedIntoWallRestsStill()
{
    // Like a car accelerating against a wall: the box must settle without jitter
    // and without sinking into the wall.
    Scene scene;
    scene.addWall();
    auto && box = scene.addBox(8, 4, { 0, 7.0f }, 0, 1, 0.3f);

    float minY = 1000;
    float maxY = -1000;
    for (int i = 0; i < 200; i++)
    {
        box.physicsComponent().addForce({ 0, -10, 0 });
        scene.step();
        if (i >= 100)
        {
            minY = std::min(minY, box.location().j());
            maxY = std::max(maxY, box.location().j());
        }
    }

    QEXPECT_FAIL("", "Positions are corrected without slop", Abort);
    QVERIFY(maxY - minY < 0.05f);
    QVERIFY(minY > 6.5f);
    QVERIFY(std::fabs(box.angle()) < 0.5f);
}

QTEST_GUILESS_MAIN(MCCollisionResponseTest)
