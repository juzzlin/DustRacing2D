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

#include "mcspringforcegenerator.hh"
#include "mccontact.hh"
#include "mcobject.hh"
#include "mcphysicscomponent.hh"

MCSpringForceGenerator::MCSpringForceGenerator(
    MCObject & object2, MCFloat coeff, MCFloat length, MCFloat min, MCFloat max)
: m_p2(&object2)
, m_coeff(coeff)
, m_length(length)
, m_min(min)
, m_max(max)
{}

void MCSpringForceGenerator::updateForce(MCObject & object1)
{
    // Take diff vector of the node locations
    MCVector3d<MCFloat> diff = object1.location() - m_p2->location();

    // Get length of diff and normalize
    const MCFloat length = diff.length();
    if (length > 0) {
        diff /= length;
    }

    // Add hard constraints due to too long / short spring.
    // This is handled by generating extra contacts for the node with
    // interpenetration depth scaled with respect to the masses of
    // the nodes.
    if (length > m_max)
    {
        const MCFloat m1 = object1.physicsComponent().invMass();
        const MCFloat m2 = m_p2->physicsComponent().invMass();
        MCContact & contact1 = MCContact::create();
        contact1.init(
            *m_p2, object1.location(), -diff, (length - m_max) * m2 / (m1 + m2));
        object1.addContact(contact1);

    }
    else if (length < m_min)
    {
        const MCFloat m1 = object1.physicsComponent().invMass();
        const MCFloat m2 = m_p2->physicsComponent().invMass();
        MCContact & contact1 = MCContact::create();
        contact1.init(
            *m_p2, object1.location(), diff, (m_min - length) * m2 / (m1 + m2));
        object1.addContact(contact1);
    }

    // Update force
    diff *= (m_length - length) * m_coeff;
    object1.physicsComponent().addForce(diff);
}

MCSpringForceGenerator::~MCSpringForceGenerator()
{
}
