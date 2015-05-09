// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#include "mcfrictiongenerator.hh"
#include "mcmathutil.hh"
#include "mcobject.hh"
#include "mcphysicscomponent.hh"
#include "mcshape.hh"

#include <cmath>

static const MCFloat ROTATION_DECAY = 0.01f;

MCFrictionGenerator::MCFrictionGenerator(MCFloat coeffLin, MCFloat coeffRot)
    : m_coeffLinTot(std::fabs(coeffLin * MCWorld::instance().gravity().k()))
    , m_coeffRotTot(std::fabs(coeffRot * MCWorld::instance().gravity().k() * ROTATION_DECAY))
{}

void MCFrictionGenerator::updateForce(MCObject & object)
{
    // Simulated friction caused by linear motion.
    MCPhysicsComponent & physicsComponent = object.physicsComponent();
    const MCFloat length = physicsComponent.velocity().lengthFast();
    const MCVector2d<MCFloat> v(physicsComponent.velocity().normalizedFast());
    if (length >= 1.0)
    {
        physicsComponent.addForce(-v * m_coeffLinTot * physicsComponent.mass());
    }
    else
    {
        physicsComponent.addForce(-v * length * m_coeffLinTot * physicsComponent.mass());
    }

    // Simulated friction caused by angular torque.
    if (object.shape())
    {
        const MCFloat a = physicsComponent.angularVelocity();
        physicsComponent.addAngularImpulse(-a * m_coeffRotTot);
    }
}

MCFrictionGenerator::~MCFrictionGenerator()
{
}

