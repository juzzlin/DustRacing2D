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
#include <MCWorld>

#include <cassert>
#include <cmath>
#include <memory>
#include <string>

using std::dynamic_pointer_cast;
using std::static_pointer_cast;

Car::Car(Description & desc, MCSurface & surface, unsigned int index, bool isHuman)
: MCObject(surface, "car")
, m_desc(desc)
, m_onTrackFriction(new MCFrictionGenerator(desc.rollingFrictionOnTrack, 0.0))
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
, m_currentTargetNodeIndex(-1)
, m_prevTargetNodeIndex(-1)
, m_routeProgression(0)
, m_position(0)
, m_isHuman(isHuman)
, m_particleEffectManager(*this)
, m_numberPos(-5, 0, 0)

// Tire positions in graphics units:
, m_leftFrontTirePos(14, 9, 0)
, m_rightFrontTirePos(14, -9, 0)
, m_leftRearTirePos(-14, 9, 0)
, m_rightRearTirePos(-14, -9, 0)

, m_leftBrakeGlowPos(-21, 8, 0)
, m_rightBrakeGlowPos(-21, -8, 0)
, m_hadHardCrash(false)
, m_gearbox(new Gearbox)
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

    const float offTrackFrictionFactor = 0.8f;
    const float frontFriction = 0.85f;
    const MCVector3dF tireZ = MCVector3dF(0, 0, 1);
    m_leftFrontTire.reset(new Tire(*this, frontFriction, frontFriction * offTrackFrictionFactor));
    addChildObject(m_leftFrontTire, m_leftFrontTirePos + tireZ, 0);

    m_rightFrontTire.reset(new Tire(*this, frontFriction, frontFriction * offTrackFrictionFactor));
    addChildObject(m_rightFrontTire, m_rightFrontTirePos + tireZ, 0);

    const float rearFriction = 0.95f;
    m_leftRearTire.reset(new Tire(*this, rearFriction, rearFriction * offTrackFrictionFactor));
    addChildObject(m_leftRearTire, m_leftRearTirePos + tireZ, 0);

    m_rightRearTire.reset(new Tire(*this, rearFriction, rearFriction * offTrackFrictionFactor));
    addChildObject(m_rightRearTire, m_rightRearTirePos + tireZ, 0);

    m_leftBrakeGlowPos += MCVector3dF(0, 0, surface.maxZ() + 1.0f);
    m_rightBrakeGlowPos += MCVector3dF(0, 0, surface.maxZ() + 1.0f);

    m_leftBrakeGlow.reset(new MCObject(m_brakeGlow, "LeftBrakeGlow"));
    m_leftBrakeGlow->setBypassCollisions(true);
    m_leftBrakeGlow->shape()->view()->setHasShadow(false);
    m_leftBrakeGlow->setIsRenderable(false);
    addChildObject(m_leftBrakeGlow, m_leftBrakeGlowPos);

    m_rightBrakeGlow.reset(new MCObject(m_brakeGlow, "RightBrakeGlow"));
    m_rightBrakeGlow->setBypassCollisions(true);
    m_rightBrakeGlow->shape()->view()->setHasShadow(false);
    m_rightBrakeGlow->setIsRenderable(false);
    addChildObject(m_rightBrakeGlow, m_rightBrakeGlowPos);
}

void Car::setProperties(Description & desc)
{
    physicsComponent().setMass(desc.mass);
	
	const float width_m = dynamic_pointer_cast<MCRectShape>(shape())->width() * MCWorld::metersPerUnit();
    const float height_m = dynamic_pointer_cast<MCRectShape>(shape())->height() * MCWorld::metersPerUnit();
	
    m_length = std::max(width_m, height_m) / MCWorld::metersPerUnit(); // note: the object's length is here in scene units. 

	
	// TODO PHYSICS: clarify center of rotation for this inertia
	// the inertia is calculated by the car's dimensions I = 1/12 * m*(width^2 + height^2), both in meters:	
    physicsComponent().setMomentOfInertia(desc.mass * 1.0f/12 * (width_m*width_m + height_m*height_m)); 
    
	physicsComponent().setRestitution(desc.restitution);
    setShadowOffset(MCVector3dF(5, -5, 1));

}

void Car::initForceGenerators(Description & desc)
{
    // Add rolling friction generator (on-track)
    MCWorld::instance().forceRegistry().addForceGenerator(m_onTrackFriction, *this);
    m_onTrackFriction->enable(true);

    MCForceGeneratorPtr drag(new MCDragForceGenerator(desc.dragLinear, desc.dragQuadratic));
    MCWorld::instance().forceRegistry().addForceGenerator(drag, *this);
}

unsigned int Car::index() const
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
    m_skidding = false;

    static_pointer_cast<Tire>(m_leftRearTire)->setSpinCoeff(1.0f);
    static_pointer_cast<Tire>(m_rightRearTire)->setSpinCoeff(1.0f);

    const float maxForce =
        physicsComponent().mass() * m_desc.accelerationFriction * std::fabs(MCWorld::instance().gravity().k());
    float currentForce = maxForce;
    const float velocity = physicsComponent().velocity().length(); // in m/s
    if (velocity > 0.001f)
    {
		// Our gear box translate a constant power into a force, depending on the current speed. 
        currentForce = m_desc.power / velocity; 
        if (currentForce > maxForce)
        {
            currentForce = maxForce;
            const float maxSpinVelocity = 4.5f; // FIXME: velocity is not in meters/s
            if (m_gearbox->gear() != Gearbox::Gear::Reverse && velocity > 0 && velocity < maxSpinVelocity)
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
    }

    MCVector2dF direction(m_dx, m_dy);
    if (deccelerate)
    {
        direction *= -1;
    }

    physicsComponent().addForce(direction * currentForce * damageFactor());
}

bool Car::isAccelerating() const
{
    return m_acceleratorEnabled && (m_gearbox->gear() != Gearbox::Gear::Neutral || !m_isHuman);
}

bool Car::isBraking() const
{
    return m_brakeEnabled && (m_gearbox->gear() == Gearbox::Gear::Neutral || !m_isHuman);
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
    return absSpeed() > 7.5 && s.lengthFast() > 0.25;
}

int Car::speedInKmh() const
{
    return m_speedInKmh;
}

float Car::absSpeed() const
{
    return m_absSpeed;
}

// FIXME Physics: why are front tires handled different than rear tires?
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
    m_particleEffectManager.update();

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
    m_absSpeed   = physicsComponent().speed();
    m_speedInKmh = m_absSpeed * 3.6;

	// TODO Physics 4: the tire wear should be proportinal to the product of 
	// the tire force and the velocity difference. 
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

void Car::wearOutTires(int step, float factor)
{
    if (Game::instance().difficultyProfile().hasTireWearOut())
    {
        const float wearOut = physicsComponent().velocity().lengthFast() * step * factor / 1000;
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

void Car::setBrakeEnabled(bool brakeEnabled)
{
    m_brakeEnabled = brakeEnabled;

    m_gearbox->setBrake(brakeEnabled);
}

void Car::setAcceleratorEnabled(bool acceleratorEnabled)
{
    m_acceleratorEnabled = acceleratorEnabled;

    m_gearbox->setAccelerator(acceleratorEnabled);
}

void Car::resetTireWear()
{
    m_tireWearOutCapacity = m_initTireWearOutCapacity;
}

/**
 * @brief Car::tireWearFactor
 * @return a value between 1.0 for good tires and 0.5 for worn out tires
 */
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

    if (m_gearbox->gear() == Gearbox::Gear::Forward)
    {
        accelerate();
    }
    else if (m_gearbox->gear() == Gearbox::Gear::Reverse)
    {
        accelerate(true);
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

void Car::setPosition(int value)
{
    m_position = value;
}

int Car::position() const
{
    return m_position;
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

Car::Description::Description()
{
	accelerationFriction = 0.95f;
	rollingFrictionOnTrack = 0.05f;
	rotationFriction = 1.0f;
	power = 5000.0f;
	mass = 1500.0f;
	restitution = 0.05f;
	dragLinear = 1.0f;
	dragQuadratic = 5.0f;
}
