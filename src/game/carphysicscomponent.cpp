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

#include "carphysicscomponent.hpp"

#include "car.hpp"
#include "difficultyprofile.hpp"
#include "game.hpp"

#include <cmath>

CarPhysicsComponent::CarPhysicsComponent(Car & car)
  : m_car(car)
{
}

void CarPhysicsComponent::addImpulse(const MCVector3dF & impulse, bool isCollision)
{
    MCPhysicsComponent::addImpulse(impulse, isCollision);

    if (Game::instance().difficultyProfile().hasBodyDamage() && isCollision && !m_car.isDestroyed())
    {
        // Light scrapes cause hardly any damage, but hard crashes are devastating.
        const float damage = (m_car.isHuman() ? 0.5f : 0.25f) * std::pow(impulse.lengthFast(), 1.6f);
        m_car.addDamage(damage);
    }
}
