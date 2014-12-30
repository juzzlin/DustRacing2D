// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2009 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcrandom.hh"
#include "mccast.hh"

#include <cstdlib>
#include <limits>
#include <random>
#include <vector>

namespace
{
const MCUint LUT_SIZE = 0x1000;
const MCUint MOD_MASK = 0x0fff;
}

//! Implementation class for MCRandom
class MCRandomImpl
{
public:
  MCRandomImpl();
  inline MCFloat getValue() const;

private:
  void buildLUT();

  mutable MCUint m_valPtr;
  std::vector<MCFloat> m_data;
  friend class MCRandom;
};

std::unique_ptr<MCRandomImpl> const MCRandom::m_pImpl(new MCRandomImpl);

MCRandomImpl::MCRandomImpl() :
    m_valPtr(0),
    m_data(LUT_SIZE, 0)
{
    buildLUT();
}

void MCRandomImpl::buildLUT()
{
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_real_distribution<MCFloat> dist(0, 1);

    for (MCUint i = 0; i < LUT_SIZE; i++) {
        m_data[i] = dist(engine);
    }
}

MCFloat MCRandomImpl::getValue() const
{
    // Increment the value pointer and mask in
    // the valid range
    m_valPtr = (m_valPtr + 1) & MOD_MASK;

    // Return the value pointed by m_valPtr
    return MCRandomImpl::m_data[m_valPtr];
}

MCFloat MCRandom::getValue()
{
    return MCRandom::m_pImpl->getValue();
}

MCVector2d<MCFloat> MCRandom::randomVector2d()
{
    return MCVector2d<MCFloat>(getValue() - .5f, getValue() - .5f).normalized();
}

MCVector3d<MCFloat> MCRandom::randomVector3d()
{
    return MCVector3d<MCFloat>(getValue() - .5f, getValue() - .5f, getValue() - .5f).normalized();
}
