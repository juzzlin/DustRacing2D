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
#include "MiniCore/Core/Physics/MCFrictionGenerator"
#include "MiniCore/Core/Physics/MCDragForceGenerator"
#include "MiniCore/Core/Particles/MCGLRectParticle"
#include "MiniCore/Core/MCRandom"
#include "MiniCore/Core/MCRectShape"
#include "MiniCore/Core/MCShape"
#include "MiniCore/Core/MCSurface"
#include "MiniCore/Core/MCTextureManager"
#include "MiniCore/Core/MCTrigonom"
#include "MiniCore/Core/MCTypes"
#include "MiniCore/Core/MCVector2d"
#include "MiniCore/Core/MCMathUtil"

static const int gMaxNumParticles = 500;

Car::Car(Description desc, MCSurface & surface, MCUint index, bool isHuman)
: MCObject(&surface, "Car")
, m_desc(desc)
, m_pBrakingFriction(new MCFrictionGenerator(desc.brakingFriction, 0.0))
, m_pOnTrackFriction(new MCFrictionGenerator(desc.rollingFriction, desc.rotationFriction))
, m_pSlideFriction(new SlideFrictionGenerator(desc.slideFriction))
, m_leftSideOffTrack(false)
, m_rightSideOffTrack(false)
, m_accelerating(false)
, m_braking(false)
, m_reverse(false)
, m_turnLeft(false)
, m_turnRight(false)
, m_index(index)
, m_tireAngle(0)
, m_frontTire(MCTextureManager::instance().surface("frontTire"))
, m_brakeGlow(MCTextureManager::instance().surface("brakeGlow"))
, m_speedInKmh(0)
, m_dx(0)
, m_dy(0)
, m_currentTargetNodeIndex(-1)
, m_prevTargetNodeIndex(-1)
, m_routeProgression(0)
, m_isHuman(isHuman)
{
    setLayer(Layers::Cars);
    setMass(desc.mass);
    setMomentOfInertia(desc.momentOfInertia);
    setMaximumVelocity(desc.maxLinearVelocity);
    setMaximumAngularVelocity(desc.maxAngularVelocity);
    setShadowOffset(MCVector2d<MCFloat>(5, -5));
    setRestitution(desc.restitution);

    // Add slide friction generator
    MCWorld::instance().addForceGenerator(*m_pSlideFriction, *this, true);

    // Add braking friction generator
    MCWorld::instance().addForceGenerator(*m_pBrakingFriction, *this, true);
    m_pBrakingFriction->enable(false);

    // Add rolling friction generator
    MCWorld::instance().addForceGenerator(*m_pOnTrackFriction, *this, true);
    m_pOnTrackFriction->enable(true);

    MCForceGenerator * drag = new MCDragForceGenerator(desc.dragLinear, desc.dragQuadratic);
    MCWorld::instance().addForceGenerator(*drag, *this, true);

    const MCFloat width  = static_cast<MCRectShape *>(shape())->width();
    const MCFloat height = static_cast<MCRectShape *>(shape())->height();
    m_length = std::max(width, height);
}

void Car::addToWorld()
{
    MCObject::addToWorld();
    m_particleGroup.clear();
    m_particleGroup.addToWorld();
}

void Car::clearStatuses()
{
    m_pBrakingFriction->enable(false);
    m_pSlideFriction->enable(true);

    m_accelerating = false;
    m_braking      = false;
    m_reverse      = false;
}

MCUint Car::index() const
{
    return m_index;
}

void Car::turnLeft()
{
    if (m_tireAngle < 45) m_tireAngle++;

    m_turnLeft = true;

    if (std::abs(m_speedInKmh) > 1)
    {
        MCFloat velScaling = 1.0 - m_speedInKmh / 600.0;
        velScaling = velScaling < 0.25f ? 0.25f : velScaling;
        if (m_braking)
        {
            addAngularImpulse(m_desc.turningImpulse / 2);
        }
        else
        {
            addAngularImpulse(m_desc.turningImpulse * velScaling);
        }
    }
}

void Car::turnRight()
{
    if (m_tireAngle > -45) m_tireAngle--;

    m_turnRight = true;

    if (std::abs(m_speedInKmh) > 1)
    {
        MCFloat velScaling = 1.0 - m_speedInKmh / 600.0;
        velScaling = velScaling < 0.25f ? 0.25f : velScaling;
        if (m_braking)
        {
            addAngularImpulse(-m_desc.turningImpulse / 2);
        }
        else
        {
            addAngularImpulse(-m_desc.turningImpulse * velScaling);
        }
    }
}

void Car::accelerate()
{
    m_pBrakingFriction->enable(false);

    MCVector2d<MCFloat> force(m_dx, m_dy);

    if (m_leftSideOffTrack || m_rightSideOffTrack)
    {
        addForce(force * m_desc.power * 0.25);
    }
    else
    {
        addForce(force * m_desc.power);
    }

    m_accelerating = true;
    m_braking      = false;
    m_reverse      = false;
}

void Car::brake()
{
    m_accelerating = false;

    if (m_speedInKmh < 1)
    {
        m_reverse = true;
    }

    if (m_reverse && m_speedInKmh > -25)
    {
        MCVector2d<MCFloat> force(m_dx, m_dy);
        addForce(-force * m_desc.power);
    }
    else
    {
        m_braking = true;
        m_pBrakingFriction->enable(true);
    }
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

int Car::speedInKmh() const
{
    return m_speedInKmh;
}

MCVector3dF Car::leftFrontTireLocation() const
{
    MCVector2dF pos;
    MCTrigonom::rotatedVector(m_desc.leftFrontTirePos, pos, angle());
    return pos + MCVector2dF(location());
}

MCVector3dF Car::rightFrontTireLocation() const
{
    MCVector2dF pos;
    MCTrigonom::rotatedVector(m_desc.rightFrontTirePos, pos, angle());
    return pos + MCVector2dF(location());
}

MCVector3dF Car::leftRearTireLocation() const
{
    MCVector2dF pos;
    MCTrigonom::rotatedVector(m_desc.leftRearTirePos, pos, angle());
    return pos + MCVector2dF(location());
}

MCVector3dF Car::rightRearTireLocation() const
{
    MCVector2dF pos;
    MCTrigonom::rotatedVector(m_desc.rightRearTirePos, pos, angle());
    return pos + MCVector2dF(location());
}

void Car::render(MCCamera *p)
{
    // Render left front tire.
    const MCVector3dF leftFrontTire(leftFrontTireLocation());
    m_frontTire.render(p, leftFrontTire, m_tireAngle + angle());

    // Render right front tire.
    const MCVector3dF rightFrontTire(rightFrontTireLocation());
    m_frontTire.render(p, rightFrontTire, m_tireAngle + angle());

    // Render body.
    MCObject::render(p);

    // Render brake light glows if braking.
    if (m_braking && m_speedInKmh > 0)
    {
        MCVector2dF leftBrakeGlow;
        MCTrigonom::rotatedVector(m_desc.leftBrakeGlowPos, leftBrakeGlow, angle());
        leftBrakeGlow += MCVector2dF(location());
        m_brakeGlow.render(p, leftBrakeGlow, angle());

        MCVector2dF rightBrakeGlow;
        MCTrigonom::rotatedVector(m_desc.rightBrakeGlowPos, rightBrakeGlow, angle());
        rightBrakeGlow += MCVector2dF(location());
        m_brakeGlow.render(p, rightBrakeGlow, angle());
    }

    if (m_accelerating || m_braking)
    {
        if (m_speedInKmh < 25)
        {
            if (!m_leftSideOffTrack)
            {
                doSkidMark(leftRearTireLocation(), 0.25f, 0.25f, 0.25f, 0.25);
            }

            if (!m_rightSideOffTrack)
            {
                doSkidMark(rightRearTireLocation(), 0.25f, 0.25f, 0.25f, 0.25);
            }
        }
    }

    // Particle animations due to the car being off the track.
    if (std::abs(m_speedInKmh) > 10)
    {
        if (m_leftSideOffTrack)
        {
            doSkidMark(leftFrontTire, 0.3f, 0.2f, 0.0, 0.5);
            doSmoke(leftFrontTire, 0.75f, 0.75f, 0.75f, 0.5);

            static int counter = 0;
            if (++counter >= 5)
            {
                doMud(leftRearTireLocation(), 0.3f, 0.2f, 0.0, 0.9);
                counter = 0;
            }
        }

        if (m_rightSideOffTrack)
        {
            doSkidMark(rightFrontTire, 0.3f, 0.2f, 0.0, 0.5);
            doSmoke(rightFrontTire, 0.75f, 0.75f, 0.75f, 0.5);

            static int counter = 0;
            if (++counter >= 5)
            {
                doMud(rightRearTireLocation(), 0.3f, 0.2f, 0.0, 0.9);
                counter = 0;
            }
        }
    }
}

void Car::collisionEvent(MCCollisionEvent & event)
{
    // Spawn sparkles if colliding with another car or a wall.
    if (m_speedInKmh > 25)
    {
        if (event.collidingObject().typeID() == typeID() ||
            event.collidingObject().typeID() == MCObject::typeID("WALL"))
        {
            static int counter = 0;
            if (++counter >= 10)
            {
                doSparkle(event.contactPoint(), 1.0, 0.8f, 0.0, 0.9);
                counter = 0;
            }
        }
    }

    event.accept();
}

void Car::stepTime()
{
    // Cache dx and dy.
    m_dx = MCTrigonom::cos(angle());
    m_dy = MCTrigonom::sin(angle());

    // Cache speed in km/h. Use value of twice as big as the "real" value.
    m_speedInKmh = velocity().dot(MCVector3d<MCFloat>(m_dx, m_dy, 0)) * 3.6f * 2;
}

void Car::setLeftSideOffTrack(bool state)
{
    // Enable off-track friction if left side is off the track.
    m_leftSideOffTrack = state;
}

void Car::setRightSideOffTrack(bool state)
{
    // Enable off-track friction if right side is off the track.
    m_rightSideOffTrack = state;
}

void Car::setTurningImpulse(MCFloat impulse)
{
    m_desc.turningImpulse = impulse;
}

void Car::doSmoke(MCVector3dFR location, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const
{
    if (MCParticle::numActiveParticles() > gMaxNumParticles)
    {
        return;
    }

    MCGLRectParticle & smoke = MCGLRectParticle::create();
    smoke.init(location, 5, 180);
    smoke.setAnimationStyle(MCParticle::Shrink);
    smoke.setColor(r, g, b, a);
    smoke.setVelocity(MCRandom::randomVector2d() * 0.1);
    smoke.addToWorld();

    m_particleGroup.addParticle(smoke);
}

void Car::doSkidMark(MCVector3dFR location, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const
{
    if (MCParticle::numActiveParticles() > gMaxNumParticles)
    {
        return;
    }

    MCGLRectParticle & skidMark = MCGLRectParticle::create();
    skidMark.init(location, 3, 15 * 60);
    skidMark.setAnimationStyle(MCParticle::FadeOut);
    skidMark.setColor(r, g, b, a);
    skidMark.rotate(angle());
    skidMark.setPhysicsObject(false);
    skidMark.addToWorld();

    m_particleGroup.addParticle(skidMark);
}

void Car::doMud(MCVector3dFR location, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const
{
    if (MCParticle::numActiveParticles() > gMaxNumParticles)
    {
        return;
    }

    MCGLRectParticle & mud = MCGLRectParticle::create();
    mud.init(location, 4, 120);
    mud.setAnimationStyle(MCParticle::Shrink);
    mud.setColor(r, g, b, a);
    mud.addToWorld();
    mud.setVelocity(velocity() * 0.5f + MCVector3dF(0, 0, 2.0));
    mud.setAcceleration(MCVector3dF(0, 0, -10.0));

    m_particleGroup.addParticle(mud);
}

void Car::doSparkle(MCVector3dFR location, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const
{
    if (MCParticle::numActiveParticles() > gMaxNumParticles)
    {
        return;
    }

    MCGLRectParticle & sparkle = MCGLRectParticle::create();
    sparkle.init(location, 2, 60);
    sparkle.setAnimationStyle(MCParticle::Shrink);
    sparkle.setColor(r, g, b, a);
    sparkle.addToWorld();
    sparkle.setVelocity(velocity() * 0.5);

    m_particleGroup.addParticle(sparkle);
}

void Car::setCurrentTargetNodeIndex(int index)
{
    m_currentTargetNodeIndex = index;
}

int Car::currentTargetNodeIndex() const
{
    return m_currentTargetNodeIndex;
}

void Car::setPrevTargetNodeIndex(int index)
{
    m_prevTargetNodeIndex = index;
}

int Car::prevTargetNodeIndex() const
{
    return m_prevTargetNodeIndex;
}

void Car::setRouteProgression(int value)
{
    m_routeProgression = value;
}

int Car::routeProgression() const
{
    return m_routeProgression;
}

bool Car::isHuman() const
{
    return m_isHuman;
}
