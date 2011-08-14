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

#include "mcgravitygenerator.hh"
#include "mcgravitygeneratorimpl.hh"
#include "mcobject.hh"

MCGravityGeneratorImpl::MCGravityGeneratorImpl(const MCVector3d<MCFloat> & g) :
    m_g(g)
{}

MCGravityGeneratorImpl::~MCGravityGeneratorImpl()
{}

MCGravityGenerator::MCGravityGenerator(const MCVector3d<MCFloat> & g) :
    m_pImpl(new MCGravityGeneratorImpl(g))
{}

void MCGravityGenerator::updateForce(MCObject * p)
{
  // G = m * g
  if (p->invMass() > 0.0) {
    p->addForce(m_pImpl->m_g * p->mass());
  }
}

MCGravityGenerator::~MCGravityGenerator()
{}

