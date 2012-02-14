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
#include "MiniCore/Core/MCRectShape"
#include "MiniCore/Core/MCShape"
#include "MiniCore/Core/MCSurface"
#include "MiniCore/Core/MCTextureManager"
#include "MiniCore/Core/MCTrigonom"
#include "MiniCore/Core/MCTypes"
#include "MiniCore/Core/MCVector2d"
#include "MiniCore/Core/MCMathUtil"

namespace
{
    const MCFloat MAX_VELOCITY       = 10.0f;
    const MCFloat BRAKE_FRICTION     = 0.5f;
    const MCFloat SLIDE_FRICTION     = 0.25f;
    const MCFloat ROLLING_FRICTION   = 0.1f;
    const MCFloat ROTATION_FRICTION  = 1.0f;
}

Car::Car(MCSurface & surface, MCUint index)
  : MCObject(&surface, "Car")
  , m_pDeccelerationFriction(new MCFrictionGenerator(BRAKE_FRICTION, 0.0f))
  , m_frictionGeneratorAdded(false)
  , m_accelerating(false)
  , m_braking(false)
  , m_index(index)
  , m_tireAngle(0)
  , m_effectiveTireAngle(0)
  , m_frontTire(MCTextureManager::instance().surface("frontTire"))
  , m_brakeGlow(MCTextureManager::instance().surface("brakeGlow"))
  , m_power(3000.0f)
{
    setLayer(Layers::Cars);
    setMass(1000);
    setMaximumVelocity(MAX_VELOCITY);
    setShadowOffset(MCVector2d<MCFloat>(5, -5));
    setRestitution(0.1f);

//    MCWorld::instance().addForceGenerator(
//        *new SlideFrictionGenerator(SLIDE_FRICTION), *this, true);

    MCWorld::instance().addForceGenerator(
        *new MCFrictionGenerator(
            ROLLING_FRICTION, ROTATION_FRICTION), *this, true);

    //setRenderShapeOutline(true);
}

void Car::setPower(MCFloat power)
{
    m_power = power;
}

void Car::clearStatuses()
{
    m_braking      = false;
    m_accelerating = false;
}

MCUint Car::index() const
{
    return m_index;
}

void Car::turnLeft()
{
    if (m_tireAngle < 45) m_tireAngle++;

    m_effectiveTireAngle = 45;
}

void Car::turnRight()
{
    if (m_tireAngle > -45) m_tireAngle--;

    m_effectiveTireAngle = -45;
}

void Car::accelerate()
{
    const MCFloat realAngle = angle() + m_effectiveTireAngle;
    MCFloat dx = MCTrigonom::cos(realAngle);
    MCFloat dy = MCTrigonom::sin(realAngle);
    MCVector2d<MCFloat> force(dx, dy);
    addForce(force * m_power);

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
    m_braking      = true;
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
    m_braking      = false;
}

void Car::noSteering()
{
    if (m_tireAngle < 0)
    {
        m_tireAngle++;
    }
    else if (m_tireAngle > 0)
    {
        m_tireAngle--;
    }

    m_effectiveTireAngle = 0;
}

MCUint Car::speedInKmh() const
{
    const MCFloat i = MCTrigonom::cos(angle());
    const MCFloat j = MCTrigonom::sin(angle());

    return velocity().dot(MCVector3d<MCFloat>(i, j, 0)) * 200 / 10;
}

void Car::render(MCCamera *p)
{
    // Render left front tire
    MCVector2dF leftTire;
    MCTrigonom::rotated(MCVector2dF(25, 16), leftTire, angle());
    leftTire += MCVector2dF(location());
    m_frontTire.render(p, leftTire, m_tireAngle + angle());

    // Render right front tire
    MCVector2dF rightTire;
    MCTrigonom::rotated(MCVector2dF(25, -16), rightTire, angle());
    rightTire += MCVector2dF(location());
    m_frontTire.render(p, rightTire, m_tireAngle + angle());

    // Render body
    MCObject::render(p);

    // Render brake lights
    if (m_braking)
    {
        MCVector2dF leftBrakeGlow;
        MCTrigonom::rotated(MCVector2dF(-45, 15), leftBrakeGlow, angle());
        leftBrakeGlow += MCVector2dF(location());
        m_brakeGlow.render(p, leftBrakeGlow, angle());

        MCVector2dF rightBrakeGlow;
        MCTrigonom::rotated(MCVector2dF(-45, -15), rightBrakeGlow, angle());
        rightBrakeGlow += MCVector2dF(location());
        m_brakeGlow.render(p, rightBrakeGlow, angle());
    }
}

void Car::beforeIntegration()
{
    const MCFloat width = static_cast<MCRectShape *>(shape())->width();
    const MCVector2dF front(width / 2, 0);
    MCTrigonom::rotated(front, m_front, angle());
    m_front += MCVector2dF(location());

    const MCVector2dF back(-width / 2, 0);
    MCTrigonom::rotated(back, m_back, angle());
    m_back = MCVector2dF(location());
}

void Car::afterIntegration()
{
    const MCFloat width = static_cast<MCRectShape *>(shape())->width();
    const MCVector2dF front(width / 2, 0);
    MCTrigonom::rotated(front, m_front2, angle());
    m_front2 += MCVector2dF(location());

    MCVector2dF bodyDirection(m_front2 - m_back);
    MCFloat newBodyAngle = MCTrigonom::radToDeg(std::atan2(
        bodyDirection.j(), bodyDirection.i()));

    rotate(newBodyAngle);

    const MCFloat speed = velocity().length();
    const MCFloat coeff = 0.99f;
    const MCFloat realAngle = angle() + 2 * m_tireAngle;
    const MCFloat newI = MCTrigonom::cos(realAngle);
    const MCFloat newJ = MCTrigonom::sin(realAngle);
    setVelocity(MCVector2d<MCFloat>(newI, newJ) * speed * (1.0f - coeff) +
        MCVector2d<MCFloat>(velocity()) * coeff);
}

Car::~Car()
{
}
