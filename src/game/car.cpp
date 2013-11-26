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
#include "game.hpp"
#include "graphicsfactory.hpp"
#include "layers.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "slidefrictiongenerator.hpp"

#include <MCAssetManager>
#include <MCCollisionEvent>
#include <MCDragForceGenerator>
#include <MCForceRegistry>
#include <MCFrictionGenerator>
#include <MCMathUtil>
#include <MCRectShape>
#include <MCShape>
#include <MCSurface>
#include <MCTrigonom>
#include <MCTypes>
#include <MCVector2d>
#include <MCWorld>

#include <cassert>
#include <cmath>
#include <string>

Car::Car(Description & desc, MCSurface & surface, MCUint index, bool isHuman)
: MCObject(surface, "Car")
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
, m_skidding(false)
, m_turnLeft(false)
, m_turnRight(false)
, m_index(index)
, m_tireAngle(0)
, m_tireWearOutCapacity(desc.tireWearOutCapacity)
, m_number(GraphicsFactory::generateNumberSurface(index))
, m_frontTire(MCAssetManager::surfaceManager().surface("frontTire"))
, m_brakeGlow(MCAssetManager::surfaceManager().surface("brakeGlow"))
, m_speedInKmh(0)
, m_dx(0)
, m_dy(0)
, m_currentTargetNodeIndex(-1)
, m_prevTargetNodeIndex(-1)
, m_routeProgression(0)
, m_isHuman(isHuman)
, m_particleEffectManager(*this)
{
    setProperties(desc);
    initForceGenerators(desc);

    m_brakeGlow.setShaderProgram(&Renderer::instance().program("master"));
    m_frontTire.setShaderProgram(&Renderer::instance().program("master"));
    m_number.setShaderProgram(&Renderer::instance().program("master"));
}

void Car::setProperties(Description & desc)
{
    setLayer(Layers::Cars);

    setMass(desc.mass);
    setMaximumVelocity(desc.maxLinearVelocity);
    setMaximumAngularVelocity(desc.maxAngularVelocity);
    setRestitution(desc.restitution);

    setShadowOffset(MCVector2dF(5, -5));

    const float width  = static_cast<MCRectShape *>(shape().get())->width();
    const float height = static_cast<MCRectShape *>(shape().get())->height();
    m_length = std::max(width, height);
}

void Car::initForceGenerators(Description & desc)
{
    // Add slide friction generator
    MCWorld::instance().forceRegistry().addForceGenerator(m_pSlideFriction, *this);

    // Add braking friction generator
    MCWorld::instance().forceRegistry().addForceGenerator(m_pBrakingFriction, *this);
    m_pBrakingFriction->enable(false);

    // Add rolling friction generator (on-track)
    MCWorld::instance().forceRegistry().addForceGenerator(m_pOnTrackFriction, *this);
    m_pOnTrackFriction->enable(true);

    // Add rolling friction generator (off-track)
    MCWorld::instance().forceRegistry().addForceGenerator(m_pOffTrackFriction, *this);
    m_pOffTrackFriction->enable(false);

    MCForceGeneratorPtr drag(new MCDragForceGenerator(desc.dragLinear, desc.dragQuadratic));
    MCWorld::instance().forceRegistry().addForceGenerator(drag, *this);
}

void Car::clearStatuses()
{
    m_pBrakingFriction->enable(false);
    m_pSlideFriction->enable(true);

    m_accelerating = false;
    m_braking      = false;
    m_reverse      = false;
    m_skidding     = false;
}

MCUint Car::index() const
{
    return m_index;
}

void Car::turnLeft()
{
    if (m_tireAngle < 45)
    {
        m_tireAngle++;
    }

    m_turnLeft = true;

    steer();
}

void Car::turnRight()
{
    if (m_tireAngle > -45)
    {
        m_tireAngle--;
    }

    m_turnRight = true;

    steer(-1);
}

float Car::calculateSteeringCoeff() const
{
    const float absSpeed = velocity().lengthFast();
    float stabilize = 1.0 - absSpeed / 80.0;
    stabilize = stabilize < 0.25 ? 0.25 : stabilize;

    const float nonLinear = std::pow(absSpeed / 7.0, 0.5);
    const float effScaling = std::min(stabilize, nonLinear);
    return effScaling;
}

void Car::steer(int direction)
{
    if (velocity().lengthFast() > 0)
    {
        const float effScaling = calculateSteeringCoeff();

        if (!m_reverse)
        {
            if (m_braking)
            {
                addAngularImpulse(m_desc.turningImpulse * effScaling * direction * 0.5);
            }
            else
            {
                addAngularImpulse(m_desc.turningImpulse * effScaling * direction);
            }
        }
        else
        {
            addAngularImpulse(-m_desc.turningImpulse * effScaling * direction);
        }
    }
}

void Car::accelerate(bool deccelerate)
{
    m_pBrakingFriction->enable(false);
    m_skidding = true;

    const float gravity = 9.81;
    const float frictionLimit = mass() * m_desc.accelerationFriction * gravity;
    float effForce = frictionLimit;
    if (!velocity().isZero())
    {
        const float powerLimit = m_desc.power / velocity().lengthFast();
        if (powerLimit < frictionLimit)
        {
            effForce   = powerLimit;
            m_skidding = false;
        }
    }

    const MCVector2dF direction(m_dx, m_dy);

    if (deccelerate)
    {
        addForce(-direction * effForce);
    }
    else
    {
        addForce(direction * effForce);
        m_accelerating = true;
        m_reverse = false;
    }

    m_braking = false;
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
        accelerate(true);
    }
    else
    {
        m_braking = true;
        m_pBrakingFriction->enable(true);
    }
}

bool Car::isBraking() const
{
    return m_braking;
}

bool Car::isSkidding() const
{
    return m_skidding;
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
    m_frontTire.render(p, leftFrontTireLocation(), m_tireAngle + angle());

    // Render right front tire.
    m_frontTire.render(p, rightFrontTireLocation(), m_tireAngle + angle());

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

    m_number.render(p, numberLocation(), angle() + 90);
}

bool Car::update()
{
    m_particleEffectManager.update();

    if (m_soundEffectManager)
    {
        m_soundEffectManager->update();
    }

    return true;
}

void Car::reset()
{
}

void Car::collisionEvent(MCCollisionEvent & event)
{
    m_particleEffectManager.collision(event);
    m_soundEffectManager->collision(event);
    event.accept();
}

void Car::wearOutTires(MCFloat step, MCFloat factor)
{
    if (m_tireWearOutCapacity > 0)
    {
        m_tireWearOutCapacity -= velocity().lengthFast() * step * factor;
    }
    else
    {
        m_tireWearOutCapacity = 0;
    }

    static_cast<SlideFrictionGenerator *>(
        m_pSlideFriction.get())->setTireWearOutFactor(tireWearLevel());
}

void Car::resetTireWear()
{
    m_tireWearOutCapacity = m_desc.tireWearOutCapacity;

    static_cast<SlideFrictionGenerator *>(
        m_pSlideFriction.get())->setTireWearOutFactor(tireWearLevel());
}

float Car::tireWearLevel() const
{
    return m_tireWearOutCapacity / m_desc.tireWearOutCapacity;
}

void Car::stepTime(MCFloat step)
{
    // Cache dx and dy.
    m_dx = MCTrigonom::cos(angle());
    m_dy = MCTrigonom::sin(angle());

    // Cache speed in km/h. Use value of 2.5 as big as the "real" value.
    m_speedInKmh = speed() * 3.6 * 2.5;

    if (m_isHuman)
    {
        if (m_braking || (m_accelerating && (m_turnLeft || m_turnRight)))
        {
            wearOutTires(step, 0.05);
        }

        if (m_leftSideOffTrack || m_rightSideOffTrack)
        {
            m_pOffTrackFriction->enable(true);
            m_pOnTrackFriction->enable(false);

            wearOutTires(step, 0.75);
        }
        else
        {
            m_pOffTrackFriction->enable(false);
            m_pOnTrackFriction->enable(true);
        }
    }
}

void Car::setLeftSideOffTrack(bool state)
{
    // Enable off-track friction if left side is off the track.
    m_leftSideOffTrack = state;
}

bool Car::leftSideOffTrack() const
{
    return m_leftSideOffTrack;
}

void Car::setRightSideOffTrack(bool state)
{
    // Enable off-track friction if right side is off the track.
    m_rightSideOffTrack = state;
}

bool Car::rightSideOffTrack() const
{
    return m_rightSideOffTrack;
}

bool Car::isOffTrack() const
{
    return leftSideOffTrack() || rightSideOffTrack();
}

void Car::setTurningImpulse(float impulse)
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

void Car::setSoundEffectManager(CarSoundEffectManagerPtr soundEffectManager)
{
    m_soundEffectManager = soundEffectManager;
}

CarSoundEffectManagerPtr Car::soundEffectManager() const
{
    return m_soundEffectManager;
}

Car::~Car()
{
    MCWorld::instance().forceRegistry().removeForceGenerators(*this);
}
