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
#include "mcshape.hh"

static const MCFloat ROTATION_DECAY = 0.01;

MCFrictionGenerator::MCFrictionGenerator(
    MCFloat coeffLin, MCFloat coeffRot, MCFloat gravity)
: m_coeffLin(coeffLin)
, m_coeffRot(coeffRot)
, m_gravity(gravity)
, m_coeffLinTot(coeffLin * gravity)
, m_coeffRotTot(coeffRot * gravity * ROTATION_DECAY)
{}

void MCFrictionGenerator::updateForce(MCObject & object)
{
    // Simulated friction caused by linear motion.
    const MCFloat length = object.velocity().lengthFast();
    const MCVector2d<MCFloat> v(object.velocity().normalizedFast());
    if (length >= 1.0)
    {
        object.addForce(-v * m_coeffLinTot * object.mass());
    }
    else
    {
        object.addForce(-v * length * m_coeffLinTot * object.mass());
    }

    // Simulated friction caused by angular torque.
    if (object.shape())
    {
        const MCFloat a = object.angularVelocity();
        object.addAngularImpulse(-a * m_coeffRotTot);
    }
}

MCFloat MCFrictionGenerator::coeffLin() const
{
    return m_coeffLin;
}

MCFloat MCFrictionGenerator::coeffRot() const
{
    return m_coeffRot;
}

MCFloat MCFrictionGenerator::gravity() const
{
    return m_gravity;
}

MCFrictionGenerator::~MCFrictionGenerator()
{
}

