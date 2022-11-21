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
#include "carparticleeffectmanager.hpp"
#include "carphysicscomponent.hpp"
#include "carsoundeffectmanager.hpp"
#include "game.hpp"
#include "gearbox.hpp"
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

#include <MCVector2d>
#include <MCWorld>

#include <cassert>
#include <cmath>
#include <memory>
#include <string>

using std::dynamic_pointer_cast;
using std::static_pointer_cast;

Car::Car(Description & desc, MCSurfacePtr surface, size_t index, bool isHuman)
  : MCObject(surface, "car")
  , m_desc(desc)
  , m_onTrackFriction(std::make_shared<MCFrictionGenerator>(desc.rollingFrictionOnTrack, 0.0))
  , m_leftSideOffTrack(false)
  , m_rightSideOffTrack(false)
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
  , m_isHuman(isHuman)
  , m_particleEffectManager(std::make_unique<CarParticleEffectManager>(*this))
  , m_numberPos(-5, 0, 0)
  , m_leftFrontTirePos(14, 9, 0)
  , m_rightFrontTirePos(14, -9, 0)
  , m_leftRearTirePos(-14, 9, 0)
  , m_rightRearTirePos(-14, -9, 0)
  , m_leftBrakeGlowPos(-21, 8, 0)
  , m_rightBrakeGlowPos(-21, -8, 0)
  , m_hadHardCrash(false)
  , m_gearbox(std::make_unique<Gearbox>())
{
    // Override the default physics component to handle damage from impulses
    setPhysicsComponent(std::make_unique<CarPhysicsComponent>(*this));

    setProperties(desc);

    initForceGenerators(desc);

    createChildObjects(surface->maxZ(), index);
}

void Car::createChildObjects(float maxZ, size_t index)
{
    // Note that the z-coordinate of the actual car body is 0. The small lift is done in the
    // surface vertex level and configured in surfaces.conf.

    const auto numberPlate = std::make_shared<MCObject>(GraphicsFactory::generateNumberSurface(index), "Number");
    addChildObject(numberPlate, m_numberPos + MCVector3dF(0, 0, maxZ + 1), 90);
    numberPlate->setBypassCollisions(true);
    numberPlate->shape()->view()->setHasShadow(false);

    createTires();

    m_leftBrakeGlowPos += MCVector3dF(0, 0, maxZ + 1.0f);
    m_rightBrakeGlowPos += MCVector3dF(0, 0, maxZ + 1.0f);

    m_leftBrakeGlow = std::make_shared<MCObject>(m_brakeGlow, "LeftBrakeGlow");
    m_leftBrakeGlow->setBypassCollisions(true);
    m_leftBrakeGlow->shape()->view()->setHasShadow(false);
    m_leftBrakeGlow->setIsRenderable(false);
    addChildObject(m_leftBrakeGlow, m_leftBrakeGlowPos);

    m_rightBrakeGlow = std::make_shared<MCObject>(m_brakeGlow, "RightBrakeGlow");
    m_rightBrakeGlow->setBypassCollisions(true);
    m_rightBrakeGlow->shape()->view()->setHasShadow(false);
    m_rightBrakeGlow->setIsRenderable(false);
    addChildObject(m_rightBrakeGlow, m_rightBrakeGlowPos);
}

void Car::createTires()
{
    const float offTrackFrictionFactor = 0.8f;
    const float frontFriction = 0.85f;
    const MCVector3dF tireZ = MCVector3dF(0, 0, 1);
    m_leftFrontTire = std::make_shared<Tire>(*this, frontFriction, frontFriction * offTrackFrictionFactor);
    addChildObject(m_leftFrontTire, m_leftFrontTirePos + tireZ, 0);

    m_rightFrontTire = std::make_shared<Tire>(*this, frontFriction, frontFriction * offTrackFrictionFactor);
    addChildObject(m_rightFrontTire, m_rightFrontTirePos + tireZ, 0);

    const float rearFriction = 0.95f;
    m_leftRearTire = std::make_shared<Tire>(*this, rearFriction, rearFriction * offTrackFrictionFactor);
    addChildObject(m_leftRearTire, m_leftRearTirePos + tireZ, 0);

    m_rightRearTire = std::make_shared<Tire>(*this, rearFriction, rearFriction * offTrackFrictionFactor);
    addChildObject(m_rightRearTire, m_rightRearTirePos + tireZ, 0);
}

void Car::setProperties(Description & desc)
{
    physicsComponent().setMass(desc.mass);
    physicsComponent().setMomentOfInertia(desc.mass * 2.5f);
    physicsComponent().setRestitution(desc.restitution);
    setShadowOffset(MCVector3dF(5, -5, 1));

    const float width = dynamic_pointer_cast<MCRectShape>(shape())->width();
    const float height = dynamic_pointer_cast<MCRectShape>(shape())->height();
    m_length = std::max(width, height);
}

void Car::initForceGenerators(Description & desc)
{
    // Add rolling friction generator (on-track)
    MCWorld::instance().forceRegistry().addForceGenerator(m_onTrackFriction, *this);
    m_onTrackFriction->enable(true);

    MCWorld::instance().forceRegistry().addForceGenerator(std::make_shared<MCDragForceGenerator>(desc.dragLinear, desc.dragQuadratic), *this);
}

size_t Car::index() const
{
    return m_index;
}

void Car::steer(Steer direction, float control)
{
    if (direction == Steer::Neutral)
    {
        m_tireAngle = 0;
    }
    else
    {
        if (const float maxAngle = 15.0f * (direction == Steer::Left ? 1 : -1); !m_isHuman)
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
    const float maxForce =
      physicsComponent().mass() * m_desc.accelerationFriction * std::fabs(MCWorld::instance().gravity().k());
    float currentForce = maxForce;

    if (const float velocity = physicsComponent().velocity().length(); velocity > 0.001f)
    {
        currentForce = m_desc.power / velocity;
        if (currentForce > maxForce)
        {
            currentForce = maxForce;
        }
    }

    MCVector2dF direction(m_dx, m_dy);
    if (deccelerate)
    {
        if (std::abs(speedInKmh()) < 25)
        {
            direction *= -1;
        }
        else
        {
            direction *= 0;
        }
    }

    physicsComponent().addForce(direction * currentForce * damageFactor());
}

void Car::doTireSpinEffect()
{
    static_pointer_cast<Tire>(m_leftRearTire)->setSpinCoeff(1.0f);
    static_pointer_cast<Tire>(m_rightRearTire)->setSpinCoeff(1.0f);

    m_skidding = false;

    const float minSpinVelocity = 0.1f;
    const float maxSpinVelocity = 4.5f;
    const float velocity = physicsComponent().velocity().length();
    if (m_acceleratorEnabled && m_gearbox->gear() == Gearbox::Gear::Forward && velocity > minSpinVelocity && velocity < maxSpinVelocity)
    {
        if (isHuman()) // Don't enable tire spin for AI yet
        {
            const float spinCoeff = 0.025f + 0.975f * std::pow(velocity / maxSpinVelocity, 2.0f);
            static_pointer_cast<Tire>(m_leftRearTire)->setSpinCoeff(spinCoeff);
            static_pointer_cast<Tire>(m_rightRearTire)->setSpinCoeff(spinCoeff);
        }

        m_skidding = true;
    }
}

bool Car::isAccelerating() const
{
    return m_acceleratorEnabled && (m_gearbox->gear() != Gearbox::Gear::Neutral || !m_isHuman);
}

bool Car::isBraking() const
{
    // This is not entirely correct, but should work in most scenarios
    return m_brakeEnabled && (m_gearbox->gear() != Gearbox::Gear::Reverse || !m_isHuman);
}

bool Car::isSkidding() const
{
    return m_skidding;
}

bool Car::isSliding()
{
    const float bodyNormalAngle = angle() + 90;
    const MCVector2dF n(
      MCTrigonom::cos(bodyNormalAngle), MCTrigonom::sin(bodyNormalAngle));
    const MCVector2dF & v = physicsComponent().velocity().normalized();
    const MCVector2dF s = MCVector2dF::projection(v, n);
    return absSpeed() > 7.5f && s.lengthFast() > 0.25f;
}

int Car::speedInKmh() const
{
    return m_speedInKmh;
}

float Car::absSpeed() const
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
    return MCMathUtil::rotatedVector(m_leftRearTirePos, angle()) + MCVector2dF(location());
}

MCVector3dF Car::rightRearTireLocation() const
{
    return MCMathUtil::rotatedVector(m_rightRearTirePos, angle()) + MCVector2dF(location());
}

void Car::updateAnimations()
{
    for (auto && collision : m_collisionEffectStack)
    {
        m_particleEffectManager->collision(collision.first->typeId(), collision.second);
        m_soundEffectManager->collision(*collision.first);
    }
    m_collisionEffectStack.clear();

    m_particleEffectManager->update();

    if (m_soundEffectManager)
    {
        m_soundEffectManager->update();
    }

    const float offset = 5.0f;
    m_leftFrontTire->rotateRelative(m_tireAngle - offset);
    m_rightFrontTire->rotateRelative(m_tireAngle + offset);

    const bool brakingGlowVisible = isBraking() && speedInKmh() > 0;
    m_leftBrakeGlow->setIsRenderable(brakingGlowVisible);
    m_rightBrakeGlow->setIsRenderable(brakingGlowVisible);
}

void Car::updateTireWear(int step)
{
    // Cache dx and dy.
    m_dx = MCTrigonom::cos(angle());
    m_dy = MCTrigonom::sin(angle());

    // Cache speed in km/h.
    m_absSpeed = physicsComponent().speed();
    m_speedInKmh = static_cast<int>(m_absSpeed * 3.6f * 2.75f);

    if (m_isHuman)
    {
        if (isBraking() || (isAccelerating() && m_steer != Steer::Neutral))
        {
            const float brakingTireWearFactor = 0.05f;
            wearOutTires(step, brakingTireWearFactor);
        }

        const float offTrackTireWearFactor = 0.10f;

        if (m_leftSideOffTrack)
        {
            static_pointer_cast<Tire>(m_leftFrontTire)->setIsOffTrack(true);
            static_pointer_cast<Tire>(m_leftRearTire)->setIsOffTrack(true);

            wearOutTires(step, offTrackTireWearFactor);
        }
        else
        {
            static_pointer_cast<Tire>(m_leftFrontTire)->setIsOffTrack(false);
            static_pointer_cast<Tire>(m_leftRearTire)->setIsOffTrack(false);
        }

        if (m_rightSideOffTrack)
        {
            static_pointer_cast<Tire>(m_rightFrontTire)->setIsOffTrack(true);
            static_pointer_cast<Tire>(m_rightRearTire)->setIsOffTrack(true);

            wearOutTires(step, offTrackTireWearFactor);
        }
        else
        {
            static_pointer_cast<Tire>(m_rightFrontTire)->setIsOffTrack(false);
            static_pointer_cast<Tire>(m_rightRearTire)->setIsOffTrack(false);
        }
    }
}

void Car::collisionEvent(MCCollisionEvent & event)
{
    if (!event.collidingObject().isTriggerObject())
    {
        m_collisionEffectStack.push_back({ &event.collidingObject(), event.contactPoint() });
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

    if (const float hardCrashDamageLimit = 3.5f; damage >= hardCrashDamageLimit)
    {
        m_hadHardCrash = true;
    }
}

void Car::wearOutTires(int step, float factor)
{
    if (Game::instance().difficultyProfile().hasTireWearOut())
    {
        if (const float wearOut = physicsComponent().velocity().lengthFast() * step * factor / 1000; m_tireWearOutCapacity >= wearOut)
        {
            m_tireWearOutCapacity -= wearOut;
        }
        else
        {
            m_tireWearOutCapacity = 0;
        }
    }
}

void Car::setBrakeEnabled(bool brakeEnabled)
{
    m_brakeEnabled = brakeEnabled;

    m_gearbox->setBrakeEnabled(brakeEnabled);
}

void Car::setAcceleratorEnabled(bool acceleratorEnabled)
{
    m_acceleratorEnabled = acceleratorEnabled;

    m_gearbox->setAcceleratorEnabled(acceleratorEnabled);
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

void Car::onStepTime(int step)
{
    updateAnimations();

    updateTireWear(step);

    m_gearbox->update(speedInKmh());

    if (m_gearbox->gear() == Gearbox::Gear::Forward && m_acceleratorEnabled)
    {
        accelerate();
    }
    else if (m_gearbox->gear() == Gearbox::Gear::Reverse && m_brakeEnabled)
    {
        accelerate(true);
    }

    doTireSpinEffect();
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
