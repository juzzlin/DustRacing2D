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
#include "collisiondamage.hpp"

#include <algorithm>
#include <cmath>

namespace CollisionDamage {

float damage(float impulse, bool isHuman)
{
    if (impulse <= minImpulse)
    {
        return 0;
    }

    const float scale = isHuman ? 0.5f : 0.25f;
    return scale * std::pow(impulse - minImpulse, 1.6f);
}

float powerFactor(float damageLevel)
{
    // Light damage doesn't matter, but below that the power drops smoothly towards the minimum
    const float minFactor = 0.08f;
    const float fullPowerLevel = 0.8f;
    const float t = std::clamp(damageLevel / fullPowerLevel, 0.0f, 1.0f);
    return minFactor + (1.0f - minFactor) * t * t * (3.0f - 2.0f * t);
}

} // namespace CollisionDamage
