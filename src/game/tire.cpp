// This file is part of Dust Racing 2D.
// Copyright (C) 2014 Jussi Lind <jussi.lind@iki.fi>
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

#include "tire.hpp"

#include <MCAssetManager>
#include <MCMathUtil>
#include <MCSurface>
#include <MCTrigonom>

Tire::Tire(MCFloat friction, MCFloat offTrackFriction)
    : MCObject(MCAssetManager::surfaceManager().surface("frontTire"), "Tire")
    , m_isOffTrack(false)
    , m_friction(friction)
    , m_offTrackFriction(offTrackFriction)
{
    setBypassCollisions(true);
}

void Tire::setIsOffTrack(bool flag)
{
    m_isOffTrack = flag;
}

void Tire::stepTime(MCFloat step)
{
    const MCFloat tireNormalAngle = angle() + 90;
    const MCVector2d<MCFloat> tire(
        MCTrigonom::cos(tireNormalAngle), MCTrigonom::sin(tireNormalAngle));

    if (velocity().lengthFast() > 0.1)
    {
        const MCVector2d<MCFloat> & v = velocity().normalizedFast();
        const MCFloat magicConstant = 40.0;
        MCVector2d<MCFloat> impulse =
            MCMathUtil::projection(v, tire) * (m_isOffTrack ? m_offTrackFriction : m_friction) * magicConstant * step;
        impulse.clampFast(0.22);
        parent().addImpulse(-impulse, location());
    }
}
