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
#include "tire.hpp"

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
: MCObject(surface, "car")
, m_desc(desc)
, m_pBrakingFriction(new MCFrictionGenerator(desc.brakingFriction, 0.0))
, m_pOnTrackFriction(new MCFrictionGenerator(desc.rollingFrictionOnTrack, 0.0))
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
, m_frontTire(MCAssetManager::surfaceManager().surface("frontTire"))
, m_brakeGlow(MCAssetManager::surfaceManager().surface("brakeGlow"))
, m_speedInKmh(0)
, m_absSpeed(0)
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

    MCObjectPtr numberPlate(new MCObject(GraphicsFactory::generateNumberSurface(index), "Number"));
    addChildObject(numberPlate, m_desc.numberPos, 90);
    numberPlate->setRenderLayerRelative(1);
    numberPlate->setBypassCollisions(true);

    const MCFloat offTrackFrictionFactor = 0.65f;
    const MCFloat frontFriction = 0.85f;
    m_leftFrontTire.reset(new Tire(*this, frontFriction, frontFriction * offTrackFrictionFactor));
    addChildObject(m_leftFrontTire, m_desc.leftFrontTirePos, 0);
    m_leftFrontTire->setRenderLayerRelative(-1);

    m_rightFrontTire.reset(new Tire(*this, frontFriction, frontFriction * offTrackFrictionFactor));
    addChildObject(m_rightFrontTire, m_desc.rightFrontTirePos, 0);
    m_rightFrontTire->setRenderLayerRelative(-1);

    const MCFloat rearFriction = 0.95f;
    m_leftRearTire.reset(new Tire(*this, rearFriction, rearFriction * offTrackFrictionFactor));
    addChildObject(m_leftRearTire, m_desc.leftRearTirePos, 0);
    m_leftRearTire->setRenderLayerRelative(-1);

    m_rightRearTire.reset(new Tire(*this, rearFriction, rearFriction * offTrackFrictionFactor));
    addChildObject(m_rightRearTire, m_desc.rightRearTirePos, 0);
    m_rightRearTire->setRenderLayerRelative(-1);
}

void Car::setProperties(Description & desc)
{
    setRenderLayer(Layers::Objects);
    setMass(desc.mass);
    setMomentOfInertia(desc.mass * 3);
    setRestitution(desc.restitution);
    setShadowOffset(MCVector2dF(5, -5));

    const float width  = static_cast<MCRectShape *>(shape().get())->width();
    const float height = static_cast<MCRectShape *>(shape().get())->height();
    m_length = std::max(width, height);
}

void Car::initForceGenerators(Description & desc)
{
    // Add braking friction generator
    MCWorld::instance().forceRegistry().addForceGenerator(m_pBrakingFriction, *this);
    m_pBrakingFriction->enable(false);

    // Add rolling friction generator (on-track)
    MCWorld::instance().forceRegistry().addForceGenerator(m_pOnTrackFriction, *this);
    m_pOnTrackFriction->enable(true);

    MCForceGeneratorPtr drag(new MCDragForceGenerator(desc.dragLinear, desc.dragQuadratic));
    MCWorld::instance().forceRegistry().addForceGenerator(drag, *this);
}

void Car::clearStatuses()
{
    m_pBrakingFriction->enable(false);

    m_accelerating = false;
    m_braking      = false;
    m_reverse      = false;
    m_skidding     = false;
}

MCUint Car::index() const
{
    return m_index;
}

void Car::turnLeft(MCFloat control)
{
    m_tireAngle = static_cast<int>(15.0 * control);

    m_turnLeft = true;
}

void Car::turnRight(MCFloat control)
{
    m_tireAngle = static_cast<int>(-15.0 * control);

    m_turnRight = true;
}

void Car::accelerate(bool deccelerate)
{
    m_pBrakingFriction->enable(false);
    m_skidding = true;

    const float frictionLimit = mass() * m_desc.accelerationFriction * MCWorld::gravity();
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
    m_tireAngle = 0;
    m_turnLeft  = false;
    m_turnRight = false;
}

int Car::speedInKmh() const
{
    return m_speedInKmh;
}

MCFloat Car::absSpeed() const
{
    return m_absSpeed;
}

MCVector3dF Car::leftFrontTireLocation() const
{
    return m_leftFrontTire->location();
}

MCVector3dF Car::rightFrontTireLocation() const
{
    return m_rightFrontTire->location();
}

MCVector3dF Car::leftRearTireLocation() const
{
    return MCTrigonom::rotatedVector(m_desc.leftRearTirePos, angle()) + MCVector2dF(location());
}

MCVector3dF Car::rightRearTireLocation() const
{
    return MCTrigonom::rotatedVector(m_desc.rightRearTirePos, angle()) + MCVector2dF(location());
}

void Car::render(MCCamera *p)
{
    // Render body.
    MCObject::render(p);

    // Render brake light glows if braking.
    if (m_braking && m_speedInKmh > 0)
    {
        const MCVector2dF leftBrakeGlow =
            MCTrigonom::rotatedVector(m_desc.leftBrakeGlowPos, angle()) + MCVector2dF(location());
        m_brakeGlow.render(p, leftBrakeGlow, angle());

        const MCVector2dF rightBrakeGlow =
            MCTrigonom::rotatedVector(m_desc.rightBrakeGlowPos, angle()) + MCVector2dF(location());
        m_brakeGlow.render(p, rightBrakeGlow, angle());
    }
}

bool Car::update()
{
    m_particleEffectManager.update();

    if (m_soundEffectManager)
    {
        m_soundEffectManager->update();
    }

    m_leftFrontTire->rotateRelative(m_tireAngle - 2.0);
    m_rightFrontTire->rotateRelative(m_tireAngle + 2.0);

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
}

void Car::resetTireWear()
{
    m_tireWearOutCapacity = m_desc.tireWearOutCapacity;
}

float Car::tireWearLevel() const
{
    return 0.75 + (m_tireWearOutCapacity / m_desc.tireWearOutCapacity) * 0.25;
}

void Car::stepTime(MCFloat step)
{
    // Cache dx and dy.
    m_dx = MCTrigonom::cos(angle());
    m_dy = MCTrigonom::sin(angle());

    // Cache speed in km/h. Use value of 2.5 as big as the "real" value.
    m_absSpeed   = speed();
    m_speedInKmh = m_absSpeed * 3.6 * 2.5;

    if (m_isHuman)
    {
        if (m_braking || (m_accelerating && (m_turnLeft || m_turnRight)))
        {
            wearOutTires(step, 0.05f);
        }

        if (m_leftSideOffTrack)
        {
            static_cast<Tire *>(m_leftFrontTire.get())->setIsOffTrack(true);
            static_cast<Tire *>(m_leftRearTire.get())->setIsOffTrack(true);

            wearOutTires(step, 0.10f);
        }
        else
        {
            static_cast<Tire *>(m_leftFrontTire.get())->setIsOffTrack(false);
            static_cast<Tire *>(m_leftRearTire.get())->setIsOffTrack(false);
        }

        if (m_rightSideOffTrack)
        {
            static_cast<Tire *>(m_rightFrontTire.get())->setIsOffTrack(true);
            static_cast<Tire *>(m_rightRearTire.get())->setIsOffTrack(true);

            wearOutTires(step, 0.10f);
        }
        else
        {
            static_cast<Tire *>(m_rightFrontTire.get())->setIsOffTrack(false);
            static_cast<Tire *>(m_rightRearTire.get())->setIsOffTrack(false);
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
