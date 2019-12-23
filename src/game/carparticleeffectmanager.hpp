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

#ifndef CARPARTICLEEFFECTMANAGER_HPP
#define CARPARTICLEEFFECTMANAGER_HPP

#include <MCVector2d>

#include "particlefactory.hpp"

class Car;
class MCCollisionEvent;

//! Manages particle effects caused by a car.
class CarParticleEffectManager
{
public:
    //! Constructor.
    CarParticleEffectManager(Car & car);

    void update();

    void collision(size_t typeId, const MCVector3dF & contactPoint);

private:
    void doDamageSmoke();

    void doOnTrackAnimations();

    void doOffTrackAnimations();

    void doLeftSkidMark(ParticleFactory::ParticleType type);

    void doRightSkidMark(ParticleFactory::ParticleType type);

    float calculateSkidAngle(float distance, double dx, double dy);

    Car & m_car;

    int m_smokeCounter;

    int m_mudCounter;

    MCVector2dF m_prevLeftSkidMarkLocation;

    MCVector2dF m_prevRightSkidMarkLocation;
};

#endif // CARPARTICLEEFFECTMANAGER_HPP
