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
#include "centrifugalforcegenerator.hpp"
#include "layers.hpp"
#include "radius.hpp"
#include "slidefrictiongenerator.hpp"
#include "MiniCore/Core/MCCollisionEvent"
#include "MiniCore/Core/MCFrictionGenerator"
#include "MiniCore/Core/MCGLRectParticle"
#include "MiniCore/Core/MCRandom"
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
    const MCFloat MAX_LINEAR_VELOCITY  = 15.0f;
    const MCFloat MAX_ANGULAR_VELOCITY = 10.0f;
    const MCFloat FRICTION             = 0.5f;
    const MCFloat BRAKING_FRICTION     = 1.0f;
    const MCFloat ROLLING_FRICTION     = 0.1f;
    const MCFloat ROTATION_FRICTION    = 0.5f;
    const MCFloat OFF_TRACK_FRICTION   = 0.5f;
}

Car::Car(MCSurface & surface, MCUint index)
  : MCObject(&surface, "Car")
  , m_pBrakingFriction(new MCFrictionGenerator(BRAKING_FRICTION, 0.0f))
  , m_pOffTrackFriction(new MCFrictionGenerator(OFF_TRACK_FRICTION, 0))
  , m_offTrack(false)
  , m_accelerating(false)
  , m_braking(false)
  , m_reverse(false)
  , m_turnLeft(false)
  , m_turnRight(false)
  , m_index(index)
  , m_tireAngle(0)
  , m_frontTire(MCTextureManager::instance().surface("frontTire"))
  , m_brakeGlow(MCTextureManager::instance().surface("brakeGlow"))
  , m_power(5000.0f)
  , m_turningImpulse(.40f)
{
    setLayer(Layers::Cars);
    setMass(1000);
    setMomentOfInertia(1000 * 10);
    setMaximumVelocity(MAX_LINEAR_VELOCITY);
    setMaximumAngularVelocity(MAX_ANGULAR_VELOCITY);
    setShadowOffset(MCVector2d<MCFloat>(5, -5));
    setRestitution(0.1f);

    // Add slide friction generator
    MCWorld::instance().addForceGenerator(*new SlideFrictionGenerator(FRICTION), *this, true);

    // Add rolling friction generator
    MCWorld::instance().addForceGenerator(
        *new MCFrictionGenerator(ROLLING_FRICTION, ROTATION_FRICTION), *this, true);

    // Add braking friction generator
    MCWorld::instance().addForceGenerator(*m_pBrakingFriction, *this, true);
    m_pBrakingFriction->enable(false);

    // Add off-track friction generator
    MCWorld::instance().addForceGenerator(*m_pOffTrackFriction, *this, true);
    m_pOffTrackFriction->enable(false);

    // Add centrifugal force generator
    const MCFloat amplification = 5.0f;
    MCWorld::instance().addForceGenerator(
        *new CentrifugalForceGenerator(amplification), *this, true);

    const MCFloat width  = static_cast<MCRectShape *>(shape())->width();
    const MCFloat height = static_cast<MCRectShape *>(shape())->height();
    m_length = std::max(width, height);

    //setRenderShapeOutline(true); // FOR DEBUG
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

    m_turnLeft = true;

    if (std::fabs(speedInKmh()) > 1)
    {
        addRotationalImpulse(m_turningImpulse);
    }
}

void Car::turnRight()
{
    if (m_tireAngle > -45) m_tireAngle--;

    m_turnRight = true;

    if (std::fabs(speedInKmh()) > 1)
    {
        addRotationalImpulse(-m_turningImpulse);
    }
}

void Car::accelerate()
{
    m_pBrakingFriction->enable(false);

    const MCFloat realAngle = angle();
    MCFloat dx = MCTrigonom::cos(realAngle);
    MCFloat dy = MCTrigonom::sin(realAngle);
    MCVector2d<MCFloat> force(dx, dy);
    addForce(force * m_power);

    m_accelerating = true;
    m_braking      = false;
    m_reverse      = false;
}

void Car::brake()
{
    m_accelerating = false;

    if (speedInKmh() < 1)
    {
        m_reverse = true;
    }

    if (m_reverse)
    {
        const MCFloat realAngle = angle();
        MCFloat dx = MCTrigonom::cos(realAngle);
        MCFloat dy = MCTrigonom::sin(realAngle);
        MCVector2d<MCFloat> force(dx, dy);
        addForce(-force * m_power / 2);
    }
    else
    {
        m_braking = true;
        m_pBrakingFriction->enable(true);
    }
}

void Car::noAction()
{
    m_pBrakingFriction->enable(false);

    m_accelerating = false;
    m_braking      = false;
    m_reverse      = false;
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

    return velocity().dot(MCVector3d<MCFloat>(i, j, 0)) * 120 / 10;
}

void Car::render(MCCamera *p)
{
    static const MCVector2dF leftFrontTirePos(20, 13);
    static const MCVector2dF rightFrontTirePos(20, -13);

    // Render left front tire
    MCVector2dF leftTire;
    MCTrigonom::rotated(leftFrontTirePos, leftTire, angle());
    leftTire += MCVector2dF(location());
    m_frontTire.render(p, leftTire, m_tireAngle + angle());

    // Render right front tire
    MCVector2dF rightTire;
    MCTrigonom::rotated(rightFrontTirePos, rightTire, angle());
    rightTire += MCVector2dF(location());
    m_frontTire.render(p, rightTire, m_tireAngle + angle());

    // Render body
    MCObject::render(p);

    // Render brake lights
    if (m_braking)
    {
        static const MCVector2dF leftBrakeGlowPos(-36, 12);
        static const MCVector2dF rightBrakeGlowPos(-36, -12);

        MCVector2dF leftBrakeGlow;
        MCTrigonom::rotated(leftBrakeGlowPos, leftBrakeGlow, angle());
        leftBrakeGlow += MCVector2dF(location());
        m_brakeGlow.render(p, leftBrakeGlow, angle());

        MCVector2dF rightBrakeGlow;
        MCTrigonom::rotated(rightBrakeGlowPos, rightBrakeGlow, angle());
        rightBrakeGlow += MCVector2dF(location());
        m_brakeGlow.render(p, rightBrakeGlow, angle());
    }

    if (m_offTrack && speedInKmh() > 10)
    {
        MCGLRectParticle & grass1 = MCGLRectParticle::create();
        grass1.init(leftTire, 5, 180);
        grass1.setAnimationStyle(MCParticle::Shrink);
        grass1.setColor(0.75f, 0.75f, 0.75f, 0.5f);
        grass1.addToWorld();
        grass1.setVelocity(MCRandom::randomVector2d() * 0.1f);

        MCGLRectParticle & grass2 = MCGLRectParticle::create();
        grass2.init(rightTire, 5, 180);
        grass2.setAnimationStyle(MCParticle::Shrink);
        grass2.setColor(0.75f, 0.75f, 0.75f, 0.5f);
        grass2.addToWorld();
        grass2.setVelocity(MCRandom::randomVector2d() * 0.1f);

        MCGLRectParticle & grass3 = MCGLRectParticle::create();
        grass3.init(leftTire, 4, 360);
        grass3.setAnimationStyle(MCParticle::FadeOut);
        grass3.setColor(0.3f, 0.2f, 0.0f, 0.5f);
        grass3.addToWorld();

        MCGLRectParticle & grass4 = MCGLRectParticle::create();
        grass4.init(rightTire, 4, 360);
        grass4.setAnimationStyle(MCParticle::FadeOut);
        grass4.setColor(0.3f, 0.2f, 0.0f, 0.5f);
        grass4.addToWorld();
    }
}

void Car::collisionEvent(MCCollisionEvent & event)
{
    if (event.collidingObject().typeID() == typeID())
    {
        if (rand() % 10 == 0)
        {
            MCGLRectParticle & sparkle = MCGLRectParticle::create();
            sparkle.init(event.contactPoint(), 2, 60);
            sparkle.setAnimationStyle(MCParticle::Shrink);
            sparkle.setColor(1.0f, 0.8f, 0.0f, 0.9f);
            sparkle.addToWorld();
            sparkle.setVelocity(velocity() * 0.5f);
        }
    }

    event.accept();
}

void Car::setOffTrack(bool state)
{
    m_pOffTrackFriction->enable(state);
    m_offTrack = state;
}

void Car::setTurningImpulse(MCFloat impulse)
{
    m_turningImpulse = impulse;
}

Car::~Car()
{
}
