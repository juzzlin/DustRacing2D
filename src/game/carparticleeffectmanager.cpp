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

#include "carparticleeffectmanager.hpp"
#include "car.hpp"

#include <MCCollisionEvent>
#include <MCPhysicsComponent>
#include <MCRandom>
#include <cmath>

namespace {
static const int SKID_MARK_DENSITY = 8;
static const int NEW_SKID_LIMIT = SKID_MARK_DENSITY * 4;
static const int ON_TRACK_ANIMATION_SPEED_MIN = 5;
} // namespace

CarParticleEffectManager::CarParticleEffectManager(Car & car)
  : m_car(car)
  , m_smokeCounter(0)
  , m_mudCounter(0)
{
}

void CarParticleEffectManager::update()
{
    doOnTrackAnimations();

    doOffTrackAnimations();

    doDamageSmoke();
}

float CarParticleEffectManager::calculateSkidAngle(float distance, double dx, double dy)
{
    return distance > NEW_SKID_LIMIT ? m_car.angle() : std::atan2(dy, dx) * 180 / 3.1415;
}

void CarParticleEffectManager::doLeftSkidMark(ParticleFactory::ParticleType type)
{
    const MCVector2dF skidLocation(m_car.leftRearTireLocation());
    const float distance = (m_prevLeftSkidMarkLocation - skidLocation).lengthFast();
    if (distance > SKID_MARK_DENSITY)
    {
        const double dx = skidLocation.i() - m_prevLeftSkidMarkLocation.i();
        const double dy = skidLocation.j() - m_prevLeftSkidMarkLocation.j();
        const int angle = static_cast<int>(calculateSkidAngle(distance, dx, dy));
        ParticleFactory::instance().doParticle(type, skidLocation, MCVector3dF(0, 0, 0), angle);
        m_prevLeftSkidMarkLocation = skidLocation;
    }
}

void CarParticleEffectManager::doRightSkidMark(ParticleFactory::ParticleType type)
{
    const MCVector2dF skidLocation(m_car.rightRearTireLocation());
    const float distance = (m_prevRightSkidMarkLocation - skidLocation).lengthFast();
    if (distance > SKID_MARK_DENSITY)
    {
        const double dx = skidLocation.i() - m_prevRightSkidMarkLocation.i();
        const double dy = skidLocation.j() - m_prevRightSkidMarkLocation.j();
        const int angle = static_cast<int>(calculateSkidAngle(distance, dx, dy));
        ParticleFactory::instance().doParticle(type, skidLocation, MCVector3dF(0, 0, 0), angle);
        m_prevRightSkidMarkLocation = skidLocation;
    }
}

void CarParticleEffectManager::doDamageSmoke()
{
    if (m_car.damageLevel() <= 0.3f && MCRandom::getValue() > m_car.damageLevel())
    {
        MCVector3dF smokeLocation = (m_car.leftFrontTireLocation() + m_car.rightFrontTireLocation()) * 0.5f;
        ParticleFactory::instance().doParticle(ParticleFactory::DamageSmoke, smokeLocation);
    }
}

void CarParticleEffectManager::doOnTrackAnimations()
{
    if ((m_car.isBraking() && m_car.speedInKmh() > ON_TRACK_ANIMATION_SPEED_MIN && m_car.speedInKmh() < 200) || m_car.isSkidding())
    {
        if (!m_car.leftSideOffTrack())
        {
            doLeftSkidMark(ParticleFactory::OnTrackSkidMark);
            ParticleFactory::instance().doParticle(ParticleFactory::SkidSmoke, m_car.leftRearTireLocation(), m_car.physicsComponent().velocity() * 0.25f);
        }

        if (!m_car.rightSideOffTrack())
        {
            doRightSkidMark(ParticleFactory::OnTrackSkidMark);
            ParticleFactory::instance().doParticle(ParticleFactory::SkidSmoke, m_car.rightRearTireLocation(), m_car.physicsComponent().velocity() * 0.25f);
        }
    }
}

void CarParticleEffectManager::doOffTrackAnimations()
{
    const bool leftSideOffTrack = m_car.leftSideOffTrack();
    const bool rightSideOffTrack = m_car.rightSideOffTrack();
    const float speed = std::abs(m_car.speedInKmh());
    const float minSpeedMud = 15;
    const float minSpeedSkidMark = 5;
    const float minSpeedSmoke = 5;

    // Left skid mark
    if (leftSideOffTrack && speed > minSpeedSkidMark)
    {
        doLeftSkidMark(ParticleFactory::OffTrackSkidMark);
    }

    // Left mud particle
    if (leftSideOffTrack && speed > minSpeedMud)
    {
        if (++m_mudCounter >= 5) // This is to prevent a continuous spray of mud particles
        {
            ParticleFactory::instance().doParticle(
              ParticleFactory::Mud, m_car.leftRearTireLocation(), m_car.physicsComponent().velocity() * 0.5f);
            m_mudCounter = 0;
        }
    }

    // Right skid mark
    if (rightSideOffTrack && speed > minSpeedSkidMark)
    {
        doRightSkidMark(ParticleFactory::OffTrackSkidMark);
    }

    // Right mud particle
    if (rightSideOffTrack && speed > minSpeedMud)
    {
        if (++m_mudCounter >= 5) // This is to prevent a continuous spray of mud particles
        {
            ParticleFactory::instance().doParticle(
              ParticleFactory::Mud, m_car.rightRearTireLocation(), m_car.physicsComponent().velocity() * 0.5f);
            m_mudCounter = 0;
        }
    }

    // Smoke
    if ((leftSideOffTrack || rightSideOffTrack) && speed > minSpeedSmoke)
    {
        if (++m_smokeCounter >= 2) // This is to prevent a continuous spray of smoke particles
        {
            MCVector3dF smokeLocation = (m_car.leftRearTireLocation() + m_car.rightRearTireLocation()) * 0.5f;
            ParticleFactory::instance().doParticle(ParticleFactory::OffTrackSmoke, smokeLocation);
        }
    }
}

void CarParticleEffectManager::collision(size_t typeId, const MCVector3dF & contactPoint)
{
    if (m_car.physicsComponent().velocity().lengthFast() > 4.0f)
    {
        // Check if the car is colliding with another car.
        const size_t particles = 10;
        if (typeId == m_car.typeId())
        {
            for (size_t i = 0; i < particles; i++)
            {
                ParticleFactory::instance().doParticle(ParticleFactory::Sparkle,
                                                       contactPoint, m_car.physicsComponent().velocity() * 0.75f);
            }
        }
        // Check if the car is colliding with hard stationary objects.
        else if (typeId == MCObject::typeId("crate") || typeId == MCObject::typeId("dustRacing2DBanner") || typeId == MCObject::typeId("grandstand") || typeId == MCObject::typeId("wall") || typeId == MCObject::typeId("wallLong") || typeId == MCObject::typeId("rock"))
        {
            for (size_t i = 0; i < particles; i++)
            {
                ParticleFactory::instance().doParticle(ParticleFactory::Sparkle,
                                                       contactPoint, m_car.physicsComponent().velocity() * 0.5f);
            }
        }
        else if (typeId == MCObject::typeId("tree"))
        {
            ParticleFactory::instance().doParticle(ParticleFactory::Leaf,
                                                   contactPoint, m_car.physicsComponent().velocity() * 0.1f);
        }
    }
}
