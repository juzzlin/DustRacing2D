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
        const MCVector2dF tireAxisUnityVector(
            MCTrigonom::cos(tireNormalAngle), MCTrigonom::sin(tireNormalAngle));
        MCVector2dF tireVelocityMaxUnityVector = physicsComponent().velocity();
		tireVelocityMaxUnityVector.normalize();
		
		// FIXME Physics: what we calculate here is only a maximum force, the tire can handle, 
		// but it isn't a force in first place...
		// There should be a condition, if the tire slipps along the tire-axis. If so, we 
		// must apply this normalForceVector, but if not, we only have to apply the needed centripedal force
		// needed for the current steering radius. 
        MCVector2dF normalForceVector =
            MCVector2dF::projection(tireVelocityMaxUnityVector, tireAxisUnityVector) *
                (m_isOffTrack ? m_offTrackFriction : m_friction) * /* m_spinCoeff * */ // TODO what is this spinCoeff for?
                    -MCWorld::instance().gravity().k() * parent().physicsComponent().mass()*0.25; // NOTE scale this to 25% load on one wheel
		
        MCVector3dF physicalLocation = location(); // location() of a tire is in scene units, relative to a global origin. 
		physicalLocation -= parent().location();
		physicalLocation *= MCWorld::metersPerUnit();
		physicalLocation += parent().location() * MCWorld::metersPerUnit();
		
        parent().physicsComponent().addForce( -normalForceVector, physicalLocation );  

        if (m_car.isBraking())
        {
			// TODO 3: improvement: for braking the front wheels create more force than the rear ones. 
            MCVector2dF brakingForceVector =
                tireVelocityMaxUnityVector * (m_isOffTrack ? m_offTrackFriction : m_friction) *
                    -MCWorld::instance().gravity().k() * parent().physicsComponent().mass() * m_car.tireWearFactor()*0.25; // NOTE scale this to 25% load on one wheel
            parent().physicsComponent().addForce( -brakingForceVector, physicalLocation ); 
        }
    }
}
