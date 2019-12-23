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

#include "carsoundeffectmanager.hpp"
#include "car.hpp"

#include <MCCollisionEvent>
#include <MCMathUtil>
#include <MCPhysicsComponent>
#include <MCTrigonom>

static std::vector<float> gearRatios = { 1.0f, 0.8f, 0.6f, 0.5f, 0.4f, 0.3f };

CarSoundEffectManager::CarSoundEffectManager(
  Car & car, const CarSoundEffectManager::MultiSoundHandles & handles)
  : m_car(car)
  , m_gear(0)
  , m_prevSpeed(0)
  , m_handles(handles)
  , m_skidPlaying(false)
{
    m_hitTimer.setSingleShot(true);
    m_hitTimer.setInterval(500);

    m_skidTimer.setSingleShot(true);
    m_skidTimer.setInterval(100);
}

void CarSoundEffectManager::startEngineSound()
{
    emit playRequested(m_handles.engineSoundHandle, true);
    emit locationChanged(m_handles.engineSoundHandle, m_car.location().i(), m_car.location().j());
}

void CarSoundEffectManager::stopEngineSound()
{
    emit stopRequested(m_handles.engineSoundHandle);
}

void CarSoundEffectManager::update()
{
    processEngineSound();
    processSkidSound();
}

void CarSoundEffectManager::processEngineSound()
{
    const int speed = static_cast<int>(m_car.absSpeed() * 10.0);
    if (speed != m_prevSpeed)
    {
        const float virtualRev = speed * 50;
        const float effRev = virtualRev * gearRatios[m_gear];
        const float pitch = 1.0 + effRev / 5000;

        if (effRev > 3000)
        {
            if (m_gear < static_cast<int>(gearRatios.size() - 1))
            {
                m_gear++;
            }
        }
        else if (effRev < 1000)
        {
            if (m_gear)
            {
                m_gear--;
            }
        }

        m_prevSpeed = speed;
        emit pitchChanged(m_handles.engineSoundHandle, pitch);
    }

    emit locationChanged(m_handles.engineSoundHandle, m_car.location().i(), m_car.location().j());
    m_prevLocation = m_car.location();
}

void CarSoundEffectManager::processSkidSound()
{
    if (m_car.isSliding())
    {
        if (!m_skidTimer.isActive())
        {
            emit locationChanged(m_handles.skidSoundHandle, m_car.location().i(), m_car.location().j());
            emit playRequested(m_handles.skidSoundHandle, false);
            m_skidPlaying = true;
            m_skidTimer.start();
        }
    }
    else if (m_skidPlaying)
    {
        emit stopRequested(m_handles.skidSoundHandle);
        m_skidPlaying = false;
    }
}

void CarSoundEffectManager::collision(MCObject & collidingObject)
{
    const MCVector3dF speedDiff(collidingObject.physicsComponent().velocity() - m_car.physicsComponent().velocity());
    if (!m_hitTimer.isActive() && speedDiff.lengthFast() > 4.0)
    {
        if (collidingObject.typeId() == m_car.typeId() || collidingObject.typeId() == MCObject::typeId("grandstand") || collidingObject.typeId() == MCObject::typeId("tree") || collidingObject.typeId() == MCObject::typeId("rock"))
        {
            emit locationChanged(m_handles.hitSoundHandle, m_car.location().i(), m_car.location().j());
            emit playRequested(m_handles.hitSoundHandle, false);
            m_hitTimer.start();
        }
        else if (
          collidingObject.typeId() == MCObject::typeId("wall") || collidingObject.typeId() == MCObject::typeId("bridgeRail") || collidingObject.typeId() == MCObject::typeId("wallLong"))
        {
            emit locationChanged("carHit2", m_car.location().i(), m_car.location().j());
            emit playRequested("carHit2", false);
            m_hitTimer.start();
        }
        else if (
          collidingObject.typeId() == MCObject::typeId("dustRacing2DBanner") || collidingObject.typeId() == MCObject::typeId("brake") || collidingObject.typeId() == MCObject::typeId("crate") || collidingObject.typeId() == MCObject::typeId("left") || collidingObject.typeId() == MCObject::typeId("plant") || collidingObject.typeId() == MCObject::typeId("right") || collidingObject.typeId() == MCObject::typeId("tire"))
        {
            emit locationChanged("carHit3", m_car.location().i(), m_car.location().j());
            emit playRequested("carHit3", false);
            m_hitTimer.start();
        }
    }
}

CarSoundEffectManager::~CarSoundEffectManager()
{
}
