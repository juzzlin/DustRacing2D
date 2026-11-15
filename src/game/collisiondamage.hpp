// This file is part of Dust Racing 2D.
// Copyright (C) 2026 Jussi Lind <jussi.lind@iki.fi>
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
#ifndef COLLISIONDAMAGE_HPP
#define COLLISIONDAMAGE_HPP

namespace CollisionDamage {

//! Collision impulses (velocity changes in world units per step) below this cause no damage.
//! Scrapes, resting contacts and friction against walls stay below it.
constexpr float minImpulse = 1.5f;

/*! \return the damage that a collision impulse causes to a car. Damage grows steeply
 *  with the impulse, so that hard crashes are devastating.
 *  \param impulse Length of the collision impulse. */
float damage(float impulse, bool isHuman);

/*! \return the factor of the engine power of a damaged car. Top speed scales with the cube root
 *  of the power, so a badly damaged car needs to lose most of its power to get clearly slower.
 *  \param damageLevel Remaining damage capacity from 0.0 (wrecked) to 1.0 (intact). */
float powerFactor(float damageLevel);

} // namespace CollisionDamage

#endif // COLLISIONDAMAGE_HPP
