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

#include "pit.hpp"
#include "car.hpp"

#include <MCCollisionEvent>
#include <MCPhysicsComponent>
#include <MCShape>
#include <MCShapeView>
#include <MCSurface>

Pit::Pit(MCSurface & surface)
: MCObject(surface, "pit")
, m_timestamp(0)
{
    physicsComponent().setMass(1, true); // Stationary
    setIsPhysicsObject(false);
    setIsTriggerObject(true);
    shape()->view()->setHasShadow(false);
}

void Pit::collisionEvent(MCCollisionEvent & event)
{
    // Cache type id integers.
    static unsigned int carType = MCObject::typeId("car");

    if (event.collidingObject().typeId() == carType)
    {
        Car & car = static_cast<Car &>(event.collidingObject());
        if (!m_collidingCars.count(&car))
        {
            m_collidingCars[&car] = {m_timestamp, false};
        }
        else
        {
            m_collidingCars[&car].first = m_timestamp;
        }
    }
}

void Pit::onStepTime(int)
{
    for (auto && car : m_collidingCars)
    {
        if (!car.second.second && car.first->isHuman() && car.first->speedInKmh() < 25 && (car.first->hasDamage() || car.first->hasTireWear()))
        {
            emit pitStop(*car.first);

            car.second.second = true; // Mark the pit stop completed
        }
    }

    // Clear cars from the map that are not colliding anymore
    auto iter = m_collidingCars.begin();
    while (iter != m_collidingCars.end())
    {
        if (iter->second.first + 1 < m_timestamp)
        {
            iter = m_collidingCars.erase(iter);
        }
        else
        {
            iter++;
        }
    }

    m_timestamp++;
}
