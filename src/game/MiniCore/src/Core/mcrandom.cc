// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

namespace {
const unsigned int LUT_SIZE = 1024;
const unsigned int MOD_MASK = 0x3ff;
} // namespace

//! Implementation class for MCRandom
class MCRandomImpl
{
public:
    MCRandomImpl();
    inline float getValue();

private:
    void buildLUT();

    mutable unsigned int m_valPtr;
    std::vector<float> m_data;
    int m_seed;
    bool m_isBuilt;
    friend class MCRandom;
};

std::unique_ptr<MCRandomImpl> const MCRandom::m_impl(new MCRandomImpl);

MCRandomImpl::MCRandomImpl()
  : m_valPtr(0)
  , m_data(LUT_SIZE, 0)
  , m_seed(0)
  , m_isBuilt(false)
{
}

void MCRandomImpl::buildLUT()
{
    std::mt19937 engine(m_seed);
    std::uniform_real_distribution<float> dist(0, 1);

    for (unsigned int i = 0; i < LUT_SIZE; i++)
    {
        m_data[i] = dist(engine);
    }

    m_isBuilt = true;
}

float MCRandomImpl::getValue()
{
    if (!MCRandomImpl::m_isBuilt)
    {
        MCRandomImpl::buildLUT();
    }

    return MCRandomImpl::m_data[++m_valPtr & MOD_MASK];
}

float MCRandom::getValue()
{
    return MCRandom::m_impl->getValue();
}

void MCRandom::setSeed(int seed)
{
    MCRandom::m_impl->m_seed = seed;
}

MCVector2dF MCRandom::randomVector2d()
{
    return MCVector2dF(getValue() - .5f, getValue() - .5f).normalized();
}

MCVector3dF MCRandom::randomVector3d()
{
    return MCVector3dF(getValue() - .5f, getValue() - .5f, getValue() - .5f).normalized();
}

MCVector3dF MCRandom::randomVector3dPositiveZ()
{
    return MCVector3dF(getValue() - .5f, getValue() - .5f, std::fabs(getValue() - .5f)).normalized();
}
