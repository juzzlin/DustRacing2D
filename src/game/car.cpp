// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include "car.hpp"
#include "centrifugalforcegenerator.hpp"
#include "layers.hpp"
#include "particlemanager.hpp"
#include "radius.hpp"
#include "renderer.hpp"
#include "slidefrictiongenerator.hpp"

#include <MCCollisionEvent>
#include <MCDragForceGenerator>
#include <MCFrictionGenerator>
#include <MCMathUtil>
#include <MCRectShape>
#include <MCShape>
#include <MCSurface>
#include <MCSurfaceManager>
#include <MCTrigonom>
#include <MCTypes>
#include <MCVector2d>

#include <string>

static std::string NUMBER_SURFACE_HANDLE[10] = {"1", "9", "8", "7", "6", "5", "4", "3", "2", "x"};

Car::Car(Description & desc, MCSurface & surface, MCUint index, bool isHuman)
: MCObject(&surface, "Car")
, m_desc(desc)
, m_pBrakingFriction(new MCFrictionGenerator(desc.brakingFriction, 0.0))
, m_pOnTrackFriction(new MCFrictionGenerator(desc.rollingFrictionOnTrack, desc.rotationFriction))
, m_pOffTrackFriction(new MCFrictionGenerator(desc.rollingFrictionOffTrack, desc.rotationFriction))
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
, m_number(MCSurfaceManager::instance().surface(NUMBER_SURFACE_HANDLE[index]))
, m_frontTire(MCSurfaceManager::instance().surface("frontTire"))
, m_brakeGlow(MCSurfaceManager::instance().surface("brakeGlow"))
, m_speedInKmh(0)
, m_dx(0)
, m_dy(0)
, m_currentTargetNodeIndex(-1)
, m_prevTargetNodeIndex(-1)
, m_routeProgression(0)
, m_isHuman(isHuman)
, m_smokeCounter(0)
, m_sparkleCounter(0)
, m_mudCounter(0)
{
    setProperties(desc);

    initForceGenerators(desc);

    m_number.setShaderProgram(&Renderer::instance().program("master"));
    m_brakeGlow.setShaderProgram(&Renderer::instance().program("master"));
    m_frontTire.setShaderProgram(&Renderer::instance().program("master"));
}

void Car::setProperties(Description & desc)
{
    setLayer(Layers::Cars);

    setMass(desc.mass);
    setMomentOfInertia(desc.momentOfInertia);
    setMaximumVelocity(desc.maxLinearVelocity);
    setMaximumAngularVelocity(desc.maxAngularVelocity);
    setRestitution(desc.restitution);

    setShadowOffset(MCVector2d<MCFloat>(5, -5));

    const MCFloat width  = static_cast<MCRectShape *>(shape())->width();
    const MCFloat height = static_cast<MCRectShape *>(shape())->height();
    m_length = std::max(width, height);
}

void Car::initForceGenerators(Description & desc)
{
    // Add slide friction generator
    MCWorld::instance().addForceGenerator(*m_pSlideFriction, *this, true);

    // Add braking friction generator
    MCWorld::instance().addForceGenerator(*m_pBrakingFriction, *this, true);
    m_pBrakingFriction->enable(false);

    // Add rolling friction generator (on-track)
    MCWorld::instance().addForceGenerator(*m_pOnTrackFriction, *this, true);
    m_pOnTrackFriction->enable(true);

    // Add rolling friction generator (off-track)
    MCWorld::instance().addForceGenerator(*m_pOffTrackFriction, *this, true);
    m_pOffTrackFriction->enable(false);

    MCForceGenerator * drag = new MCDragForceGenerator(desc.dragLinear, desc.dragQuadratic);
    MCWorld::instance().addForceGenerator(*drag, *this, true);
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
        velScaling = velScaling < 0.25 ? 0.25 : velScaling;

        if (!m_reverse)
        {
            if (m_braking)
            {
                addAngularImpulse(m_desc.turningImpulse / 2);
            }
            else
            {
                addAngularImpulse(m_desc.turningImpulse * velScaling);
            }
        }
        else
        {
            addAngularImpulse(-m_desc.turningImpulse * velScaling);
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
        velScaling = velScaling < 0.25 ? 0.25 : velScaling;

        if (!m_reverse)
        {
            if (m_braking)
            {
                addAngularImpulse(-m_desc.turningImpulse / 2);
            }
            else
            {
                addAngularImpulse(-m_desc.turningImpulse * velScaling);
            }
        }
        else
        {
            addAngularImpulse(m_desc.turningImpulse * velScaling);
        }
    }
}

void Car::accelerate()
{
    m_pBrakingFriction->enable(false);

    MCVector2d<MCFloat> force(m_dx, m_dy);
    addForce(force * m_desc.power);

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

MCVector3dF Car::numberLocation() const
{
    MCVector2dF pos;
    MCTrigonom::rotatedVector(m_desc.numberPos, pos, angle());
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
                ParticleManager::instance().doSkidMark(
                    leftRearTireLocation(), 0.25, 0.25, 0.25, 0.25);
            }

            if (!m_rightSideOffTrack)
            {
                ParticleManager::instance().doSkidMark(
                    rightRearTireLocation(), 0.25, 0.25, 0.25, 0.25);
            }
        }
    }

    // Particle animations due to the car being off the track.
    if (std::abs(m_speedInKmh) > 10)
    {
        bool smoke = false;
        if (m_leftSideOffTrack)
        {
            ParticleManager::instance().doSkidMark(leftFrontTire, 0.3, 0.2, 0.0, 0.5);
            smoke = true;

            if (++m_mudCounter >= 5)
            {
                ParticleManager::instance().doMud(
                    leftRearTireLocation(), velocity() * 0.5, 0.3, 0.2, 0.0, 0.9);
                m_mudCounter = 0;
            }
        }

        if (m_rightSideOffTrack)
        {
            ParticleManager::instance().doSkidMark(rightFrontTire, 0.3, 0.2, 0.0, 0.5);
            smoke = true;

            if (++m_mudCounter >= 5)
            {
                ParticleManager::instance().doMud(
                    rightRearTireLocation(), velocity() * 0.5, 0.3, 0.2, 0.0, 0.9);
                m_mudCounter = 0;
            }
        }

        if (smoke)
        {
            if (++m_smokeCounter >= 2)
            {
                MCVector3dF smokeLocation = (leftRearTireLocation() + rightRearTireLocation()) * 0.5;
                ParticleManager::instance().doSmoke(smokeLocation, 0.75, 0.75, 0.75, 0.5);
            }
        }
    }

    m_number.render(p, numberLocation(), angle() + 90);
}

void Car::collisionEvent(MCCollisionEvent & event)
{
    static MCUint dustRacing2DBanner = MCObject::typeID("dustRacing2DBanner");
    static MCUint grandstand         = MCObject::typeID("grandstand");
    static MCUint wall               = MCObject::typeID("wall");
    static MCUint rock               = MCObject::typeID("rock");

    // Spawn sparkles if colliding with another car or a wall.
    if (m_speedInKmh > 25)
    {
        if (event.collidingObject().typeID() == typeID())
        {
            if (++m_sparkleCounter >= 10)
            {
                ParticleManager::instance().doSparkle(
                    event.contactPoint(), velocity() * 0.5, 1.0, 0.8, 0.0, 0.9);
                ParticleManager::instance().doSmoke(event.contactPoint(), 0.75, 0.75, 0.75, 0.5);
                m_sparkleCounter = 0;
            }
        }
        else if (
            event.collidingObject().typeID() == dustRacing2DBanner ||
            event.collidingObject().typeID() == grandstand ||
            event.collidingObject().typeID() == wall ||
            event.collidingObject().typeID() == rock)
        {
            ParticleManager::instance().doSparkle(
                event.contactPoint(), velocity() * 0.5, 1.0, 0.8, 0.0, 0.9);
            ParticleManager::instance().doSmoke(event.contactPoint(), 0.75, 0.75, 0.75, 0.5);
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
    m_speedInKmh = velocity().dot(MCVector3d<MCFloat>(m_dx, m_dy, 0)) * 3.6 * 2;

    if (m_leftSideOffTrack || m_rightSideOffTrack)
    {
        m_pOffTrackFriction->enable(true);
        m_pOnTrackFriction->enable(false);
    }
    else
    {
        m_pOffTrackFriction->enable(false);
        m_pOnTrackFriction->enable(true);
    }
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
