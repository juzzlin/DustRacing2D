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

#include "mcspringforcegenerator2dfast.hh"
#include "mcobject.hh"
#include "mcphysicscomponent.hh"

MCSpringForceGenerator2dFast::MCSpringForceGenerator2dFast(
    MCObject & object2, MCFloat coeff, MCFloat length, MCFloat min, MCFloat max)
: m_p2(&object2)
, m_coeff(coeff)
, m_length(length)
, m_min(min)
, m_max(max)
{}

void MCSpringForceGenerator2dFast::updateForce(MCObject & object1)
{
    object1.physicsComponent().preventSleeping(true);

    // Take diff vector of the node locations
    MCVector2dF diff(object1.location() - m_p2->location());

    // Get length of diff and normalize
    const MCFloat length = diff.lengthFast();
    if (length > 0)
    {
        diff /= length;
    }

    // Update force
    diff *= (m_length - length) * m_coeff;
    object1.physicsComponent().addForce(diff);

    if (m_p2->physicsComponent().isSleeping())
    {
        m_p2->physicsComponent().toggleSleep(false);
    }
}

MCSpringForceGenerator2dFast::~MCSpringForceGenerator2dFast()
{
}
