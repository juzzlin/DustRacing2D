// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#include "car.h"
#include "layers.h"
#include "MiniCore/Core/MCFrictionGenerator"
#include "MiniCore/Core/MCSurface"
#include "MiniCore/Core/MCTrigonom"
#include "MiniCore/Core/MCTypes"
#include "MiniCore/Core/MCVector2d"
#include "MiniCore/Core/MCMathUtil"

Car::Car(MCSurface * pSurface)
: MCObject(pSurface, "Car")
, m_deccelerationFriction(new MCFrictionGenerator(1.0f))
, m_frictionGeneratorAdded(false)
{
    setLayer(Layers::Cars);
    setMass(1000);
    setMaximumVelocity(1000);
    setShadowOffset(MCVector2d<MCFloat>(5, -5));
}

void Car::turnLeft()
{
    rotate(angle() + 1);
}

void Car::turnRight()
{
    rotate(360 + static_cast<int>(angle()) - 1);
}

void Car::accelerate()
{
    MCFloat dx = MCTrigonom::cos(angle());
    MCFloat dy = MCTrigonom::sin(angle());
    MCVector2d<MCFloat> force(dx, dy);
    addForce(force * 2000.0f);

    // Remove friction generator if it was added
    if (m_frictionGeneratorAdded)
    {
        MCWorld::instance()->removeForceGenerator(
            m_deccelerationFriction, this);
        m_frictionGeneratorAdded = false;
    }
}

void Car::brake()
{
    // Simulate braking by adding a friction
    // generator.
    MCWorld::instance()->addForceGenerator(
        m_deccelerationFriction, this, false);
    m_frictionGeneratorAdded = true;
}

Car::~Car()
{
    delete m_deccelerationFriction;
}
