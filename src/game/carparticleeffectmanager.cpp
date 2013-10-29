// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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
#include "particlefactory.hpp"

#include <cmath>
#include <MCCollisionEvent>

CarParticleEffectManager::CarParticleEffectManager(Car & car)
: m_car(car)
, m_sparkleCounter(0)
, m_smokeCounter(0)
, m_mudCounter(0)
{
}

void CarParticleEffectManager::update()
{
    doOnTrackAnimations();

    doOffTrackAnimations();
}

void CarParticleEffectManager::doOnTrackAnimations()
{
    if (m_car.isBraking() && m_car.speedInKmh() > 5 && m_car.speedInKmh() < 100)
    {
        const int skidMarkDensity = 16;
        const int newSkidLimit    = skidMarkDensity * 4;

        if (!m_car.leftSideOffTrack())
        {
            const MCVector2dF skidLocation(m_car.leftRearTireLocation());
            const MCFloat distance = (m_prevLeftSkidMarkLocation - skidLocation).lengthFast();
            if (distance > skidMarkDensity)
            {
                const double dx = skidLocation.i() - m_prevLeftSkidMarkLocation.i();
                const double dy = skidLocation.j() - m_prevLeftSkidMarkLocation.j();

                int angle = 0;
                if (distance > newSkidLimit)
                {
                    angle = m_car.angle();
                }
                else
                {
                    angle = static_cast<int>(std::atan2(dy, dx) * 180 / 3.1415);
                }

                ParticleFactory::instance().doParticle(
                    ParticleFactory::OnTrackSkidMark, skidLocation, MCVector3dF(0, 0, 0), angle);
                m_prevLeftSkidMarkLocation = skidLocation;
            }
        }

        if (!m_car.rightSideOffTrack())
        {
            const MCVector2dF skidLocation(m_car.rightRearTireLocation());
            const MCFloat distance = (m_prevRightSkidMarkLocation - skidLocation).lengthFast();
            if (distance > skidMarkDensity)
            {
                const double dx = skidLocation.i() - m_prevRightSkidMarkLocation.i();
                const double dy = skidLocation.j() - m_prevRightSkidMarkLocation.j();

                int angle = 0;
                if (distance > newSkidLimit)
                {
                    angle = m_car.angle();
                }
                else
                {
                    angle = static_cast<int>(std::atan2(dy, dx) * 180 / 3.1415);
                }

                ParticleFactory::instance().doParticle(
                    ParticleFactory::OnTrackSkidMark, skidLocation, MCVector3dF(0, 0, 0), angle);
                m_prevRightSkidMarkLocation = skidLocation;
            }
        }
    }
}

void CarParticleEffectManager::doOffTrackAnimations()
{
    if (std::abs(m_car.speedInKmh()) > 10)
    {
        const int skidMarkDensity = 16;
        const int newSkidLimit    = skidMarkDensity * 4;
        bool smoke = false;
        if (m_car.leftSideOffTrack())
        {
            const MCVector2dF skidLocation(m_car.leftRearTireLocation());
            const MCFloat distance = (m_prevLeftSkidMarkLocation - skidLocation).lengthFast();
            if (distance > skidMarkDensity)
            {
                const double dx = skidLocation.i() - m_prevLeftSkidMarkLocation.i();
                const double dy = skidLocation.j() - m_prevLeftSkidMarkLocation.j();

                int angle = 0;
                if (distance > newSkidLimit)
                {
                    angle = m_car.angle();
                }
                else
                {
                    angle = static_cast<int>(std::atan2(dy, dx) * 180 / 3.1415);
                }

                ParticleFactory::instance().doParticle(
                    ParticleFactory::OffTrackSkidMark, skidLocation, MCVector3dF(0, 0, 0), angle);
                m_prevLeftSkidMarkLocation = skidLocation;
            }

            smoke = true;

            if (++m_mudCounter >= 5)
            {
                ParticleFactory::instance().doParticle(
                    ParticleFactory::Mud, m_car.leftRearTireLocation(), m_car.velocity() * 0.5);
                m_mudCounter = 0;
            }
        }

        if (m_car.rightSideOffTrack())
        {
            const MCVector2dF skidLocation(m_car.rightRearTireLocation());
            const MCFloat distance = (m_prevRightSkidMarkLocation - skidLocation).lengthFast();
            if (distance > skidMarkDensity)
            {
                const double dx = skidLocation.i() - m_prevRightSkidMarkLocation.i();
                const double dy = skidLocation.j() - m_prevRightSkidMarkLocation.j();

                int angle = 0;
                if (distance > newSkidLimit)
                {
                    angle = m_car.angle();
                }
                else
                {
                    angle = static_cast<int>(std::atan2(dy, dx) * 180 / 3.1415);
                }

                ParticleFactory::instance().doParticle(
                    ParticleFactory::OffTrackSkidMark, skidLocation, MCVector3dF(0, 0, 0), angle);
                m_prevRightSkidMarkLocation = skidLocation;
            }

            smoke = true;

            if (++m_mudCounter >= 5)
            {
                ParticleFactory::instance().doParticle(
                    ParticleFactory::Mud, m_car.rightRearTireLocation(), m_car.velocity() * 0.5);
                m_mudCounter = 0;
            }
        }

        if (smoke)
        {
            if (++m_smokeCounter >= 2)
            {
                MCVector3dF smokeLocation = (m_car.leftRearTireLocation() + m_car.rightRearTireLocation()) * 0.5;
                ParticleFactory::instance().doParticle(ParticleFactory::OffTrackSmoke, smokeLocation);
            }
        }
    }
}

void CarParticleEffectManager::collision(const MCCollisionEvent & event)
{
    // Cache type id integers.
    static MCUint crate              = MCObject::typeID("crate");
    static MCUint dustRacing2DBanner = MCObject::typeID("dustRacing2DBanner");
    static MCUint grandstand         = MCObject::typeID("grandstand");
    static MCUint wall               = MCObject::typeID("wall");
    static MCUint wallLong           = MCObject::typeID("wallLong");
    static MCUint rock               = MCObject::typeID("rock");
    static MCUint tree               = MCObject::typeID("tree");
    static MCUint plant              = MCObject::typeID("plant");

    if (m_car.speedInKmh() > 25)
    {
        // Check if the car is colliding with another car.
        if (event.collidingObject().typeID() == m_car.typeID())
        {
            if (++m_sparkleCounter >= 10)
            {
                ParticleFactory::instance().doParticle(ParticleFactory::Sparkle,
                    event.contactPoint(), m_car.velocity() * 0.75);
                ParticleFactory::instance().doParticle(ParticleFactory::Smoke,
                    event.contactPoint(), m_car.velocity() * 0.5);
                m_sparkleCounter = 0;
            }
        }
        // Check if the car is colliding with hard stationary objects.
        else if (
            event.collidingObject().typeID() == crate ||
            event.collidingObject().typeID() == dustRacing2DBanner ||
            event.collidingObject().typeID() == grandstand ||
            event.collidingObject().typeID() == wall ||
            event.collidingObject().typeID() == wallLong ||
            event.collidingObject().typeID() == rock)
        {
            ParticleFactory::instance().doParticle(ParticleFactory::Sparkle,
                event.contactPoint(), m_car.velocity() * 0.5);
            ParticleFactory::instance().doParticle(ParticleFactory::Smoke,
                event.contactPoint(), m_car.velocity() * 0.1);
        }
        // Check if the car is colliding with trees or plants.
        else if (
            event.collidingObject().typeID() == tree ||
            event.collidingObject().typeID() == plant)
        {
            ParticleFactory::instance().doParticle(ParticleFactory::Leaf,
                event.contactPoint(), m_car.velocity() * 0.1);
        }
    }
}
