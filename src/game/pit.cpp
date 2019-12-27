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
#include <MCSeparationEvent>
#include <MCShape>
#include <MCShapeView>
#include <MCSurface>

Pit::Pit(MCSurfacePtr surface)
  : MCObject(surface, "pit")
{
    physicsComponent().setMass(1, true); // Stationary
    setIsPhysicsObject(false);
    setIsTriggerObject(true);
    shape()->view()->setHasShadow(false);
}

void Pit::reset()
{
    m_possiblyPittingCars.clear();
}

void Pit::collisionEvent(MCCollisionEvent & event)
{
    // Cache type id integers.
    static const auto carType = MCObject::typeId("car");
    if (event.collidingObject().typeId() == carType)
    {
        Car & car = static_cast<Car &>(event.collidingObject());
        if (car.isHuman())
        {
            m_possiblyPittingCars.insert(&car);
        }
    }
}

void Pit::separationEvent(MCSeparationEvent & event)
{
    // Cache type id integers.
    static const auto carType = MCObject::typeId("car");
    if (event.separatedObject().typeId() == carType)
    {
        Car & car = static_cast<Car &>(event.separatedObject());
        if (car.isHuman())
        {
            m_possiblyPittingCars.erase(&car);
        }
    }
}

void Pit::onStepTime(int)
{
    auto iter = m_possiblyPittingCars.begin();
    while (iter != m_possiblyPittingCars.end())
    {
        if ((*iter)->speedInKmh() < 25)
        {
            emit pitStop(**iter);

            iter = m_possiblyPittingCars.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}
