// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#include "car.hpp"
#include "layers.hpp"
#include "slidefrictiongenerator.hpp"
#include "MiniCore/Core/MCCollisionEvent"
#include "MiniCore/Core/MCFrictionGenerator"
#include "MiniCore/Core/MCShape"
#include "MiniCore/Core/MCSurface"
#include "MiniCore/Core/MCTrigonom"
#include "MiniCore/Core/MCTypes"
#include "MiniCore/Core/MCVector2d"
#include "MiniCore/Core/MCMathUtil"

namespace
{
    const MCFloat MAX_VELOCITY      = 6.0f;
    const MCFloat BRAKE_FRICTION    = 0.25f;
    const MCFloat SLIDE_FRICTION    = 0.15f;
    const MCFloat ROLLING_FRICTION  = 0.01f;
    const MCFloat ROTATION_FRICTION = 0.9f;
}

Car::Car(MCSurface & surface, MCUint index)
  : MCObject(&surface, "Car")
  , m_pDeccelerationFriction(new MCFrictionGenerator(BRAKE_FRICTION, 0.0f))
  , m_frictionGeneratorAdded(false)
  , m_accelerating(false)
  , m_index(index)
{
    setLayer(Layers::Cars);
    setMass(1000);
    setMaximumVelocity(MAX_VELOCITY);
    setShadowOffset(MCVector2d<MCFloat>(5, -5));

    MCWorld::instance().addForceGenerator(
        *new SlideFrictionGenerator(SLIDE_FRICTION), *this, true);

    MCWorld::instance().addForceGenerator(
        *new MCFrictionGenerator(
            ROLLING_FRICTION, ROTATION_FRICTION), *this, true);

    //setRenderShapeOutline(true);
}

MCUint Car::index() const
{
    return m_index;
}

void Car::turnLeft()
{
    const MCUint newAngle = angle() + 1;
    oversteer(newAngle);
    rotate(newAngle);
}

void Car::turnRight()
{
    const MCUint newAngle = 360 + static_cast<int>(angle()) - 1;
    oversteer(newAngle);
    rotate(newAngle);
}

void Car::oversteer(MCUint newBodyAngle)
{
    const MCFloat speed = velocity().length();
    const MCFloat coeff = 0.99f;
    const MCFloat newI = MCTrigonom::cos(newBodyAngle);
    const MCFloat newJ = MCTrigonom::sin(newBodyAngle);
    setVelocity(MCVector2d<MCFloat>(newI, newJ) * speed * (1.0f - coeff) +
        MCVector2d<MCFloat>(velocity()) * coeff);
}

void Car::accelerate()
{
    MCFloat dx = MCTrigonom::cos(angle());
    MCFloat dy = MCTrigonom::sin(angle());
    MCVector2d<MCFloat> force(dx, dy);
    addForce(force * 2000.0f);

    // Remove friction generator if it was added
    if (m_frictionGeneratorAdded)
    {
        MCWorld::instance().removeForceGenerator(
           *m_pDeccelerationFriction, *this);
        m_frictionGeneratorAdded = false;
    }

    m_accelerating = true;
}

void Car::brake()
{
    // Simulate braking by adding a friction
    // generator.
    MCWorld::instance().addForceGenerator(
        *m_pDeccelerationFriction, *this, true);
    m_frictionGeneratorAdded = true;

    m_accelerating = false;
}

void Car::noAction()
{
    // Remove friction generator if it was added
    if (m_frictionGeneratorAdded)
    {
        MCWorld::instance().removeForceGenerator(
            *m_pDeccelerationFriction, *this);
        m_frictionGeneratorAdded = false;
    }

    m_accelerating = false;
}

MCUint Car::speedInKmh() const
{
    const MCFloat i = MCTrigonom::cos(angle());
    const MCFloat j = MCTrigonom::sin(angle());

    return velocity().dot(MCVector3d<MCFloat>(i, j, 0)) * 200 / 10;
}

Car::~Car()
{
}
