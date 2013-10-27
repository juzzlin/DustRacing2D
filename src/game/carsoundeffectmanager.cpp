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

static std::vector<float> gearRatios = {1.0, 0.8, 0.6, 0.5, 0.4, 0.3};

CarSoundEffectManager::CarSoundEffectManager(Car & car, QString engineSoundHandle)
    : m_car(car)
    , m_engineSoundHandle(engineSoundHandle)
    , m_gear(0)
    , m_prevSpeed(0)
{
}

void CarSoundEffectManager::startEngineSound()
{
    emit playRequested(m_engineSoundHandle, true);
    emit locationChanged(m_engineSoundHandle, m_car.location().i(), m_car.location().j());
}

void CarSoundEffectManager::stopEngineSound()
{
}

void CarSoundEffectManager::update()
{
    const int speed = m_car.speedInKmh();
    if (speed != m_prevSpeed)
    {
        const float virtualRev = speed * 50;
        const float effRev     = virtualRev * gearRatios[m_gear];
        const float pitch      = 1.0 + effRev / 5000;

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
        emit pitchChangeRequested(m_engineSoundHandle, pitch);
    }

    if ((m_car.location() - m_prevLocation).lengthFast() > 10)
    {
        emit locationChanged(m_engineSoundHandle, m_car.location().i(), m_car.location().j());
        m_prevLocation = m_car.location();
    }
}

CarSoundEffectManager::~CarSoundEffectManager()
{
}
