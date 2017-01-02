// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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
#include "carphysicscomponent.hpp"
#include "game.hpp"
#include "graphicsfactory.hpp"
#include "layers.hpp"
#include "renderer.hpp"
#include "tire.hpp"

#include <MCAssetManager>
#include <MCCollisionEvent>
#include <MCDragForceGenerator>
#include <MCForceRegistry>
#include <MCFrictionGenerator>
#include <MCMathUtil>
#include <MCPhysicsComponent>
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
, m_onTrackFriction(new MCFrictionGenerator(desc.rollingFrictionOnTrack, 0.0))
, m_leftSideOffTrack(false)
, m_rightSideOffTrack(false)
, m_accelerating(false)
, m_braking(false)
, m_reverse(false)
, m_skidding(false)
, m_steer(Steer::Neutral)
, m_index(index)
, m_tireAngle(0)
, m_initDamageCapacity(100)
, m_damageCapacity(m_initDamageCapacity)
, m_initTireWearOutCapacity(100)
, m_tireWearOutCapacity(m_initTireWearOutCapacity)
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
, m_numberPos(-5, 0, 0)
, m_leftFrontTirePos(14, 9, 0)
, m_rightFrontTirePos(14, -9, 0)
, m_leftRearTirePos(-14, 9, 0)
, m_rightRearTirePos(-14, -9, 0)
, m_leftBrakeGlowPos(-21, 8, 0)
, m_rightBrakeGlowPos(-21, -8, 0)
, m_hadHardCrash(false)
{
    // Override the default physics component to handle damage from impulses
    setPhysicsComponent(*(new CarPhysicsComponent(*this)));

    setProperties(desc);
    initForceGenerators(desc);

    // Note that the z-coordinate of the actual car body is 0. The small lift is done in the
    // surface vertex level and configured in surfaces.conf.

    MCObjectPtr numberPlate(new MCObject(GraphicsFactory::generateNumberSurface(index), "Number"));
    addChildObject(numberPlate, m_numberPos + MCVector3dF(0, 0, surface.maxZ() + 1), 90);
    numberPlate->setBypassCollisions(true);
    numberPlate->shape()->view()->setHasShadow(false);

    const MCFloat offTrackFrictionFactor = 0.65f;
    const MCFloat frontFriction = 0.85f;
    const MCVector3dF tireZ = MCVector3dF(0, 0, 1);
    m_leftFrontTire.reset(new Tire(*this, frontFriction, frontFriction * offTrackFrictionFactor));
    addChildObject(m_leftFrontTire, m_leftFrontTirePos + tireZ, 0);

    m_rightFrontTire.reset(new Tire(*this, frontFriction, frontFriction * offTrackFrictionFactor));
    addChildObject(m_rightFrontTire, m_rightFrontTirePos + tireZ, 0);

    const MCFloat rearFriction = 0.95f;
    m_leftRearTire.reset(new Tire(*this, rearFriction, rearFriction * offTrackFrictionFactor));
    addChildObject(m_leftRearTire, m_leftRearTirePos + tireZ, 0);

    m_rightRearTire.reset(new Tire(*this, rearFriction, rearFriction * offTrackFrictionFactor));
    addChildObject(m_rightRearTire, m_rightRearTirePos + tireZ, 0);

    m_leftBrakeGlowPos += MCVector3dF(0, 0, surface.maxZ() + 1);
    m_rightBrakeGlowPos += MCVector3dF(0, 0, surface.maxZ() + 1);
}

void Car::setProperties(Description & desc)
{
    setRenderLayer(static_cast<int>(Layers::Render::Objects));
    physicsComponent().setMass(desc.mass);
    physicsComponent().setMomentOfInertia(desc.mass * 3);
    physicsComponent().setRestitution(desc.restitution);
    setShadowOffset(MCVector3dF(5, -5, 1));

    const float width  = dynamic_cast<MCRectShape *>(shape().get())->width();
    const float height = dynamic_cast<MCRectShape *>(shape().get())->height();
    m_length = std::max(width, height);
}

void Car::initForceGenerators(Description & desc)
{
    // Add rolling friction generator (on-track)
    MCWorld::instance().forceRegistry().addForceGenerator(m_onTrackFriction, *this);
    m_onTrackFriction->enable(true);

    MCForceGeneratorPtr drag(new MCDragForceGenerator(desc.dragLinear, desc.dragQuadratic));
    MCWorld::instance().forceRegistry().addForceGenerator(drag, *this);
}

void Car::clearStatuses()
{
    m_accelerating = false;
    m_braking      = false;
    m_reverse      = false;
    m_skidding     = false;
}

MCUint Car::index() const
{
    return m_index;
}

void Car::steer(Steer direction, MCFloat control)
{
    if (direction == Steer::Neutral)
    {
        m_tireAngle = 0;
    }
    else
    {
        const float maxAngle = 15.0f * (direction == Steer::Left ? 1 : -1);

        if (!m_isHuman)
        {
            m_tireAngle = static_cast<int>(maxAngle * control);
        }
        else
        {
            m_tireAngle = m_tireAngle + (maxAngle - m_tireAngle) * 0.15f;
        }
    }

    m_steer = direction;
}

void Car::accelerate(bool deccelerate)
{
    m_skidding = true;

    const float frictionLimit =
        physicsComponent().mass() * m_desc.accelerationFriction * std::fabs(MCWorld::instance().gravity().k()) * damageFactor();
    float effForce = frictionLimit;
    if (!physicsComponent().velocity().isZero())
    {
        const float powerLimit = m_desc.power / physicsComponent().velocity().lengthFast();
        if (powerLimit < frictionLimit)
        {
            effForce   = powerLimit;
            m_skidding = false;
        }
    }

    MCVector2dF direction(m_dx, m_dy);
    if (deccelerate)
    {
        direction *= -1;
    }
    else
    {
        m_accelerating = true;
        m_reverse = false;
    }

    physicsComponent().addForce(direction * effForce);

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
    return MCTrigonom::rotatedVector(m_leftRearTirePos, angle()) + MCVector2dF(location());
}

MCVector3dF Car::rightRearTireLocation() const
{
    return MCTrigonom::rotatedVector(m_rightRearTirePos, angle()) + MCVector2dF(location());
}

void Car::render(MCCamera *p)
{
    // Render body.
    MCObject::render(p);

    // Render brake light glows if braking.
    if (m_braking && m_speedInKmh > 0)
    {
        const MCVector2dF leftBrakeGlow =
            MCTrigonom::rotatedVector(m_leftBrakeGlowPos, angle()) + MCVector2dF(location());
        m_brakeGlow.render(p, leftBrakeGlow, angle());

        const MCVector2dF rightBrakeGlow =
            MCTrigonom::rotatedVector(m_rightBrakeGlowPos, angle()) + MCVector2dF(location());
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

    const float offset = 5.0f;
    m_leftFrontTire->rotateRelative(m_tireAngle - offset);
    m_rightFrontTire->rotateRelative(m_tireAngle + offset);

    return true;
}

void Car::reset()
{
}

void Car::collisionEvent(MCCollisionEvent & event)
{
    if (!event.collidingObject().isTriggerObject())
    {
        m_particleEffectManager.collision(event);
        m_soundEffectManager->collision(event);
    }

    event.accept();
}

void Car::addDamage(float damage)
{
    if (m_damageCapacity > damage)
    {
        m_damageCapacity -= damage;
    }
    else
    {
        m_damageCapacity = 0;
    }

    const float hardCrashDamageLimit = 3.5f;
    if (damage >= hardCrashDamageLimit)
    {
        m_hadHardCrash = true;
    }
}

void Car::wearOutTires(MCFloat step, MCFloat factor)
{
    if (Game::instance().difficultyProfile().hasTireWearOut())
    {
        const MCFloat wearOut = physicsComponent().velocity().lengthFast() * step * factor;
        if (m_tireWearOutCapacity >= wearOut)
        {
            m_tireWearOutCapacity -= wearOut;
        }
        else
        {
            m_tireWearOutCapacity = 0;
        }
    }
}

void Car::resetTireWear()
{
    m_tireWearOutCapacity = m_initTireWearOutCapacity;
}

float Car::tireWearFactor() const
{
    return 0.5f + m_tireWearOutCapacity * 0.5f / m_initTireWearOutCapacity;
}

float Car::tireWearLevel() const
{
    return m_tireWearOutCapacity / m_initTireWearOutCapacity;
}

void Car::resetDamage()
{
    m_damageCapacity = m_initDamageCapacity;
}

float Car::damageFactor() const
{
    return 0.7f + (m_damageCapacity / m_initDamageCapacity) * 0.3f;
}

float Car::damageLevel() const
{
    return m_damageCapacity / m_initDamageCapacity;
}

bool Car::hadHardCrash()
{
    if (m_hadHardCrash)
    {
        m_hadHardCrash = false;
        return true;
    }
    return false;
}

void Car::onStepTime(MCFloat step)
{
    // Cache dx and dy.
    m_dx = MCTrigonom::cos(angle());
    m_dy = MCTrigonom::sin(angle());

    // Cache speed in km/h. Use value of 2.5 as big as the "real" value.
    m_absSpeed   = physicsComponent().speed();
    m_speedInKmh = m_absSpeed * 3.6 * 2.5;

    if (m_isHuman)
    {
        if (m_braking || (m_accelerating && m_steer != Steer::Neutral))
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

void Car::setNextTargetNodeIndex(int index)
{
    m_nextTargetNodeIndex = index;
}

int Car::nextTargetNodeIndex() const
{
    return m_nextTargetNodeIndex;
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
