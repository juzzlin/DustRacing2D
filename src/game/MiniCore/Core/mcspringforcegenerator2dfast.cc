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
#include "mcspringforcegenerator2dfastimpl.hh"
#include "mccontact.hh"
#include "mcobject.hh"

MCSpringForceGenerator2dFastImpl::MCSpringForceGenerator2dFastImpl(
    MCObject * p2, MCFloat coeff, MCFloat length, MCFloat min, MCFloat max)
: m_p2(p2)
, m_coeff(coeff)
, m_length(length)
, m_min(min)
, m_max(max)
{}

MCSpringForceGenerator2dFastImpl::~MCSpringForceGenerator2dFastImpl()
{}

void MCSpringForceGenerator2dFastImpl::updateForce(MCObject * p1)
{
    // Take diff vector of the node locations
    MCVector2d<MCFloat> diff(p1->location() - m_p2->location());

    // Get length of diff and normalize
    const MCFloat length = diff.lengthFast();
    if (length > 0) {
        diff /= length;
    }

    // Add hard constraints due to too long / short spring.
    // This is handled by generating extra contacts for the node with
    // interpenetration depth scaled with respect to the masses of
    // the nodes.
    if (length > m_max) {
        const MCFloat m1 = p1->invMass();
        const MCFloat m2 = m_p2->invMass();
        MCContact * contact1 = MCContact::create();
        contact1->init(m_p2, p1->location(), -diff, (length - m_max) * m2 / (m1 + m2));
        p1->addContact(contact1);

    } else if (length < m_min) {
        const MCFloat m1 = p1->invMass();
        const MCFloat m2 = m_p2->invMass();
        MCContact * contact1 = MCContact::create();
        contact1->init(m_p2, p1->location(), diff, (m_min - length) * m2 / (m1 + m2));
        p1->addContact(contact1);
    }

    // Update force
    diff *= (m_length - length) * m_coeff;
    p1->addForce(diff);
}

MCSpringForceGenerator2dFast::MCSpringForceGenerator2dFast(
    MCObject * p2, MCFloat coeff, MCFloat length, MCFloat min, MCFloat max)
: m_pImpl(new MCSpringForceGenerator2dFastImpl(
    p2, coeff, length, min, max))
{}

void MCSpringForceGenerator2dFast::updateForce(MCObject * p1)
{
    m_pImpl->updateForce(p1);
}

MCSpringForceGenerator2dFast::~MCSpringForceGenerator2dFast()
{
    delete m_pImpl;
}

