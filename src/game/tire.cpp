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
#include <MCWorld>

Tire::Tire(Car & car, float friction, float offTrackFriction)
    : MCObject(MCAssetManager::surfaceManager().surface("frontTire"), "Tire")
    , m_isOffTrack(false)
    , m_friction(friction)
    , m_offTrackFriction(offTrackFriction)
    , m_spinCoeff(1.0f)
    , m_car(car)
{
    setBypassCollisions(true);
}

void Tire::setIsOffTrack(bool flag)
{
    m_isOffTrack = flag;
}

void Tire::setSpinCoeff(float spinCoeff)
{
    m_spinCoeff = spinCoeff;
}

void Tire::onStepTime(int)
{
    if (physicsComponent().velocity().lengthFast() > 0)
    {
        const float tireNormalAngle = angle() + 90;
        const MCVector2dF tireAxisVector(
            MCTrigonom::cos(tireNormalAngle), MCTrigonom::sin(tireNormalAngle));
        MCVector2dF tireVelocityMaxUnityVector = physicsComponent().velocity();
		tireVelocityMaxUnityVector.normalize();
		
		// FIXME Physics: what we calculate here is only a maximum force, the tire can handle, 
		// but it isn't a force in first place...
        MCVector2dF normalForceVector =
            MCVector2dF::projection(tireVelocityMaxUnityVector, tireAxisVector) *
                (m_isOffTrack ? m_offTrackFriction : m_friction) * /* m_spinCoeff * */ // TODO reenable spinCoeff
                    -MCWorld::instance().gravity().k() * parent().physicsComponent().mass(); // TODO scale this to 25% load on one wheel
		
        //normalForceVector.clampFast(parent().physicsComponent().mass() * 7.0f * m_car.tireWearFactor()); // FIXME: what's this code for?
        parent().physicsComponent().addForce( -normalForceVector, location() );  // TODO: transform to physics unit

        if (m_car.isBraking())
        {
 
			// TODO 3: improvement: for braking the front wheels create more force than the rear ones. 
            MCVector2dF brakingForceVector =
                tireVelocityMaxUnityVector * (m_isOffTrack ? m_offTrackFriction : m_friction) *
                    -MCWorld::instance().gravity().k() * parent().physicsComponent().mass() * m_car.tireWearFactor()*0.25; // TODO scale this to 25% load on one wheel
            parent().physicsComponent().addForce( -brakingForceVector, location() ); 
        }
    }
}
