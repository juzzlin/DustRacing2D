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
#include "collisiondamage.hpp"
#include "difficultyprofile.hpp"
#include "game.hpp"

#include <MCMathUtil>

#include <cmath>

namespace {

//! \return the side of the car that a collision impulse hit.
Car::DamageSide hitSide(const Car & car, const MCVector3dF & impulse)
{
    // The impulse pushes the car away from the side that was hit. The car points to +x in its own frame.
    const auto local = MCMathUtil::rotatedVector(MCVector2dF { impulse }, -car.angle());
    if (std::fabs(local.i()) > std::fabs(local.j()))
    {
        return local.i() < 0 ? Car::DamageSide::Front : Car::DamageSide::Rear;
    }

    return local.j() < 0 ? Car::DamageSide::Left : Car::DamageSide::Right;
}

} // namespace

CarPhysicsComponent::CarPhysicsComponent(Car & car)
  : m_car(car)
{
}

void CarPhysicsComponent::addImpulse(const MCVector3dF & impulse, bool isCollision)
{
    MCPhysicsComponent::addImpulse(impulse, isCollision);

    if (Game::instance().difficultyProfile().hasBodyDamage() && isCollision && !m_car.isDestroyed())
    {
        if (const float damage = CollisionDamage::damage(impulse.length(), m_car.isHuman()); damage > 0)
        {
            m_car.addDamage(damage, hitSide(m_car, impulse));
        }
    }
}
