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
#include "radius.hpp"
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
    const MCFloat SLIDE_FRICTION     = 0.5f;
    const MCFloat ROLLING_FRICTION   = 0.1f;
    const MCFloat ROTATION_FRICTION  = 0.1f;
}

Car::Car(MCSurface & surface, MCUint index)
  : MCObject(&surface, "Car")
  , m_pBrakingFriction(new MCFrictionGenerator(BRAKE_FRICTION, 0.0f))
  , m_accelerating(false)
  , m_braking(false)
  , m_turnLeft(false)
  , m_turnRight(false)
  , m_index(index)
  , m_tireAngle(0)
  , m_frontTire(MCTextureManager::instance().surface("frontTire"))
  , m_brakeGlow(MCTextureManager::instance().surface("brakeGlow"))
  , m_power(3000.0f)
  , m_turningMoment(1e5)
{
    setLayer(Layers::Cars);
    setMass(1000);
    setMaximumVelocity(MAX_VELOCITY);
    setMaximumAngularVelocity(10);
    setShadowOffset(MCVector2d<MCFloat>(5, -5));
    setRestitution(0.1f);

    // Add slide friction generator
    MCWorld::instance().addForceGenerator(
        *new SlideFrictionGenerator(SLIDE_FRICTION), *this, true);

    // Add rolling friction generator
    MCWorld::instance().addForceGenerator(
        *new MCFrictionGenerator(
            ROLLING_FRICTION, ROTATION_FRICTION), *this, true);

    // Add braking friction generator
    MCWorld::instance().addForceGenerator(
        *m_pBrakingFriction, *this, true);
    m_pBrakingFriction->enable(false);

    const MCFloat width  = static_cast<MCRectShape *>(shape())->width();
    const MCFloat height = static_cast<MCRectShape *>(shape())->height();
    m_length = std::max(width, height);

    //setRenderShapeOutline(true);
}

void Car::setPower(MCFloat power)
{
    m_power = power;
}

void Car::setTurningMoment(MCFloat value)
{
    m_turningMoment = value;
}

MCFloat Car::turningMoment() const
{
    return m_turningMoment;
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

    m_turnLeft = true;

    if (speedInKmh() > 5)
    {
        addMoment(m_turningMoment);
    }
}

void Car::turnRight()
{
    if (m_tireAngle > -45) m_tireAngle--;

    m_turnRight = true;

    if (speedInKmh() > 5)
    {
        addMoment(-m_turningMoment);
    }
}

void Car::accelerate()
{
    m_pBrakingFriction->enable(false);

    const MCFloat realAngle = angle();/* + m_effectiveTireAngle*/;
    MCFloat dx = MCTrigonom::cos(realAngle);
    MCFloat dy = MCTrigonom::sin(realAngle);
    MCVector2d<MCFloat> force(dx, dy);
    addForce(force * m_power);

    m_accelerating = true;
    m_braking      = false;
}

void Car::brake()
{
    m_pBrakingFriction->enable(true);

    m_accelerating = false;
    m_braking      = true;
}

void Car::noAction()
{
    m_pBrakingFriction->enable(false);

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

    m_turnLeft = false;
    m_turnRight = false;
}

MCFloat Car::speedInKmh() const
{
    const MCFloat i = MCTrigonom::cos(angle());
    const MCFloat j = MCTrigonom::sin(angle());

    return velocity().dot(MCVector3d<MCFloat>(i, j, 0)) * 200 / 10;
}

void Car::render(MCCamera *p)
{
    // Render left front tire
    MCVector2dF leftTire;
    MCTrigonom::rotated(MCVector2dF(20, 13), leftTire, angle());
    leftTire += MCVector2dF(location());
    m_frontTire.render(p, leftTire, m_tireAngle + angle());

    // Render right front tire
    MCVector2dF rightTire;
    MCTrigonom::rotated(MCVector2dF(20, -13), rightTire, angle());
    rightTire += MCVector2dF(location());
    m_frontTire.render(p, rightTire, m_tireAngle + angle());

    // Render body
    MCObject::render(p);

    // Render brake lights
    if (m_braking)
    {
        MCVector2dF leftBrakeGlow;
        MCTrigonom::rotated(MCVector2dF(-36, 12), leftBrakeGlow, angle());
        leftBrakeGlow += MCVector2dF(location());
        m_brakeGlow.render(p, leftBrakeGlow, angle());

        MCVector2dF rightBrakeGlow;
        MCTrigonom::rotated(MCVector2dF(-36, -12), rightBrakeGlow, angle());
        rightBrakeGlow += MCVector2dF(location());
        m_brakeGlow.render(p, rightBrakeGlow, angle());
    }
}

Car::~Car()
{
}
