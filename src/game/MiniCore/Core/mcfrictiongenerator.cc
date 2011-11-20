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
#include "mcfrictiongeneratorimpl.hh"
#include "mcobject.hh"

namespace
{
    const MCFloat FRICTION_SPEED_TH = 0.001f;
}

MCFrictionGeneratorImpl::MCFrictionGeneratorImpl(
    MCFloat coeff, MCFloat acceleration)
: m_coeff(coeff * acceleration)
{}

MCFrictionGeneratorImpl::~MCFrictionGeneratorImpl()
{}

MCFrictionGenerator::MCFrictionGenerator(
    MCFloat coeff, MCFloat acceleration)
: m_pImpl(new MCFrictionGeneratorImpl(coeff, acceleration))
{}

void MCFrictionGenerator::updateForce(MCObject * p)
{
    MCVector2d<MCFloat> v(p->velocity());
    MCFloat l = v.lengthFast();
    if (l > FRICTION_SPEED_TH) {
        p->addForce((-v / l) * m_pImpl->m_coeff * p->mass());
    }
}

MCFrictionGenerator::~MCFrictionGenerator()
{
    delete m_pImpl;
}

