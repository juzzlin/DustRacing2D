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
#include "car.hpp"

#include <MCAssetManager>
#include <MCMathUtil>
#include <MCPhysicsComponent>
#include <MCSurface>
#include <MCTrigonom>

Tire::Tire(Car & car, MCFloat friction, MCFloat offTrackFriction)
    : MCObject(MCAssetManager::surfaceManager().surface("frontTire"), "Tire")
    , m_isOffTrack(false)
    , m_friction(friction)
    , m_offTrackFriction(offTrackFriction)
    , m_car(car)
{
    setBypassCollisions(true);
}

void Tire::setIsOffTrack(bool flag)
{
    m_isOffTrack = flag;
}

void Tire::onStepTime(int)
{
    if (physicsComponent().velocity().lengthFast() > 0)
    {
        const MCFloat tireNormalAngle = angle() + 90;
        const MCVector2dF tire(
            MCTrigonom::cos(tireNormalAngle), MCTrigonom::sin(tireNormalAngle));
        MCVector2dF v = physicsComponent().velocity();
        v.clampFast(0.999f); // Clamp instead of normalizing to avoid artifacts on small values
        MCVector2dF impulse =
            MCVector2dF::projection(v, tire) *
                (m_isOffTrack ? m_offTrackFriction : m_friction) *
                    -MCWorld::instance().gravity().k() * parent().physicsComponent().mass();
        impulse.clampFast(parent().physicsComponent().mass() * 7.0f * m_car.tireWearFactor());
        parent().physicsComponent().addForce(-impulse, location());

        if (m_car.isBraking())
        {
            MCVector2dF impulse =
                v * 0.5f * (m_isOffTrack ? m_offTrackFriction : m_friction) *
                    -MCWorld::instance().gravity().k() * parent().physicsComponent().mass() * m_car.tireWearFactor();
            parent().physicsComponent().addForce(-impulse, location());
        }
    }
}
