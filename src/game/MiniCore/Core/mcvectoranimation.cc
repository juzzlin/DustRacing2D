// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcvectoranimation.hh"
#include <cassert>

MCVectorAnimation::MCVectorAnimation()
  : m_pVect(nullptr)
{
}

MCVectorAnimation::~MCVectorAnimation()
{
}

void MCVectorAnimation::init(
    MCVector3dF & vect,
    const MCVector3dF & start,
    const MCVector3dF & end,
    MCUint steps)
{
    m_delta = (end - start) / steps;
    m_pVect = &vect;
    m_steps = steps;
    m_step  = 0;

    *m_pVect = start;
}

bool MCVectorAnimation::update()
{
    assert(m_pVect);
    if (++m_step < m_steps)
    {
        *m_pVect += m_delta;
        return false;
    }
    return true;
}
