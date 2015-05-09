// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#include "mcphysicscomponent.hh"
#include "mctrigonom.hh"

namespace {
static const MCFloat DAMPING = 0.999f;
}

MCPhysicsComponent::MCPhysicsComponent()
    : m_damping(DAMPING)
    , m_angularAcceleration(0)
    , m_angularVelocity(0)
    , m_angularImpulse(0)
    , m_torque(0)
    , m_invMass(std::numeric_limits<MCFloat>::max())
    , m_mass(0)
    , m_invMomentOfInertia(std::numeric_limits<MCFloat>::max())
    , m_momentOfInertia(0)
    , m_restitution(0.5f)
    , m_xyFriction(0.0f)
    , m_isSleeping(false)
    , m_isSleepingPrevented(false)
    , m_isStationary(false)
    , m_isIntegrating(false)
    , m_linearSleepLimit(0.01f)
    , m_angularSleepLimit(0.01f)
{
}

void MCPhysicsComponent::addImpulse(const MCVector3dF & impulse, bool)
{
    m_linearImpulse += impulse;

    toggleSleep(false);
}

void MCPhysicsComponent::addImpulse(const MCVector3dF & impulse, const MCVector3dF & pos, bool isCollision)
{
    m_linearImpulse += impulse;
    const MCFloat r = (pos - object().location()).lengthFast();
    if (r > 0) {
        addAngularImpulse((-(impulse % (pos - object().location())).k()) / r, isCollision);
    }
    toggleSleep(false);
}

void MCPhysicsComponent::addAngularImpulse(MCFloat impulse, bool)
{
    m_angularImpulse += impulse;

    toggleSleep(false);
}

void MCPhysicsComponent::setVelocity(const MCVector3dF & newVelocity)
{
    m_velocity = newVelocity;

    toggleSleep(false);
}

const MCVector3dF & MCPhysicsComponent::velocity() const
{
    return m_velocity;
}

MCFloat MCPhysicsComponent::speed() const
{
    return velocity().dot(MCVector3dF(object().direction()));
}

void MCPhysicsComponent::setAngularVelocity(MCFloat newVelocity)
{
    m_angularVelocity = newVelocity;

    toggleSleep(false);
}

MCFloat MCPhysicsComponent::angularVelocity() const
{
    return m_angularVelocity;
}

void MCPhysicsComponent::setAcceleration(const MCVector3dF & newAcceleration)
{
    m_acceleration = newAcceleration;

    toggleSleep(false);
}

const MCVector3dF & MCPhysicsComponent::acceleration() const
{
    return m_acceleration;
}

void MCPhysicsComponent::addForce(const MCVector3dF & force)
{
    m_forces += force;

    toggleSleep(false);
}

void MCPhysicsComponent::addForce(const MCVector3dF & force, const MCVector3dF & pos)
{
    addTorque(-(force % (pos - object().location())).k());
    m_forces += force;

    toggleSleep(false);
}

void MCPhysicsComponent::addTorque(MCFloat torque)
{
    m_torque += torque;

    toggleSleep(false);
}

void MCPhysicsComponent::setMass(MCFloat newMass, bool stationary)
{
    m_isStationary = stationary;

    if (!stationary)
    {
        if (newMass > 0)
        {
            m_invMass = 1.0f / newMass;
        }
        else
        {
            m_invMass = std::numeric_limits<MCFloat>::max();
        }

        m_mass = newMass;

        // This is just a default guess. The shape should set the "correct" value.
        setMomentOfInertia(newMass);
    }
    else
    {
        m_invMass  = 0;
        m_mass     = std::numeric_limits<MCFloat>::max();

        m_isSleeping = true;

        setMomentOfInertia(std::numeric_limits<MCFloat>::max());
    }
}

MCFloat MCPhysicsComponent::invMass() const
{
    return m_invMass;
}

MCFloat MCPhysicsComponent::mass() const
{
    return m_mass;
}

void MCPhysicsComponent::setMomentOfInertia(MCFloat newMomentOfInertia)
{
    if (newMomentOfInertia > 0)
    {
        m_invMomentOfInertia = 1.0f / newMomentOfInertia;
    }
    else
    {
        m_invMomentOfInertia = std::numeric_limits<MCFloat>::max();
    }

    m_momentOfInertia = newMomentOfInertia;
}

MCFloat MCPhysicsComponent::momentOfInertia() const
{
    return m_momentOfInertia;
}

MCFloat MCPhysicsComponent::invMomentOfInertia() const
{
    return m_invMomentOfInertia;
}

void MCPhysicsComponent::setRestitution(MCFloat newRestitution)
{
    newRestitution = newRestitution < 0.0f ? 0.0f : newRestitution;
    newRestitution = newRestitution > 1.0f ? 1.0f : newRestitution;
    m_restitution = newRestitution;
}

MCFloat MCPhysicsComponent::restitution() const
{
    return m_restitution;
}

void MCPhysicsComponent::setXYFriction(MCFloat friction)
{
    m_xyFriction = friction;
}

MCFloat MCPhysicsComponent::xyFriction() const
{
    return m_xyFriction;
}

bool MCPhysicsComponent::isSleeping() const
{
    return m_isSleeping;
}

bool MCPhysicsComponent::isIntegrating() const
{
    return m_isIntegrating;
}

void MCPhysicsComponent::resetZ()
{
    m_velocity.setK(0);
    m_forces.setK(0);
}

void MCPhysicsComponent::setSleepLimits(MCFloat linearSleepLimit, MCFloat angularSleepLimit)
{
    m_linearSleepLimit  = linearSleepLimit;
    m_angularSleepLimit = angularSleepLimit;
}

void MCPhysicsComponent::toggleSleep(bool state)
{
    if (state && m_isSleepingPrevented)
    {
        return;
    }

    m_isSleeping = state;

    // Optimization: dynamically remove from the integration vector
    if (!object().isParticle())
    {
        if (state)
        {
            MCWorld::instance().removeObjectFromIntegration(object());
        }
        else
        {
            MCWorld::instance().restoreObjectToIntegration(object());
        }
    }
}

void MCPhysicsComponent::preventSleeping(bool flag)
{
    m_isSleepingPrevented = flag;
}

bool MCPhysicsComponent::isStationary() const
{
    return m_isStationary;
}

void MCPhysicsComponent::integrate(MCFloat step)
{
    // Integrate, if the object is not sleeping and it doesn't
    // have a parent object.
    if (!object().physicsComponent().isSleeping() && (&object().parent() == &object()))
    {
        m_isIntegrating = true;

        integrateLinear(step);
        integrateAngular(step);
        object().checkBoundaries();

        if (m_velocity.lengthFast() < m_linearSleepLimit &&
            m_angularVelocity       < m_angularSleepLimit)
        {
            toggleSleep(true);
            reset();
        }

        m_forces.setZero();
        m_linearImpulse.setZero();
        m_angularImpulse = 0.0f;

        object().translate(object().location() + m_velocity);

        m_isIntegrating = false;
    }
}

void MCPhysicsComponent::integrateLinear(MCFloat step)
{
    MCVector3dF totAcceleration(m_acceleration);
    totAcceleration += m_forces * m_invMass;
    m_velocity      += totAcceleration * step + m_linearImpulse;
    m_velocity      *= m_damping;
}

void MCPhysicsComponent::integrateAngular(MCFloat step)
{
    if (object().shape() && m_momentOfInertia > 0.0f)
    {
        MCFloat totAngularAcceleration(m_angularAcceleration);
        totAngularAcceleration += m_torque * m_invMomentOfInertia;
        m_angularVelocity      += totAngularAcceleration * step + m_angularImpulse;
        m_angularVelocity      *= m_damping;

        const MCFloat newAngle = object().angle() + MCTrigonom::radToDeg(m_angularVelocity * step);
        object().rotate(newAngle, false);
    }

    m_torque = 0.0f;
}

void MCPhysicsComponent::stepTime(MCFloat step)
{
    integrate(step);
}

void MCPhysicsComponent::reset()
{
    // Reset linear motion
    m_forces.setZero();
    m_velocity.setZero();
    m_acceleration.setZero();
    m_linearImpulse.setZero();

    // Reset angular motion
    m_torque              = 0.0f;
    m_angularAcceleration = 0.0f;
    m_angularVelocity     = 0.0f;
    m_angularImpulse      = 0.0f;

    for (auto child: object().children())
    {
        child->physicsComponent().reset();
    }
}

MCPhysicsComponent::~MCPhysicsComponent()
{
}
