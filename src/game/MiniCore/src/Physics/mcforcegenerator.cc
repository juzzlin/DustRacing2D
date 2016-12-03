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

#include "mcforcegenerator.hh"

class MCForceGeneratorImpl
{
    MCForceGeneratorImpl();

    bool enabled;
    friend class MCForceGenerator;
};

MCForceGeneratorImpl::MCForceGeneratorImpl()
  : enabled(true)
{}

MCForceGenerator::MCForceGenerator()
  : m_pImpl(new MCForceGeneratorImpl)
{}

void MCForceGenerator::enable(bool status)
{
    m_pImpl->enabled = status;
}

bool MCForceGenerator::enabled() const
{
    return m_pImpl->enabled;
}

MCForceGenerator::~MCForceGenerator()
{
    delete m_pImpl;
}

