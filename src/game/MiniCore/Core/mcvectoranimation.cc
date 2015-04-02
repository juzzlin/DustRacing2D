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

#include "mcvectoranimation.hh"
#include <cassert>

MCVectorAnimation::MCVectorAnimation()
: m_vect(nullptr)
, m_steps(0)
, m_step(0)
, m_preDelay(0)
, m_postDelay(0)
{
}

void MCVectorAnimation::init(MCVector3dF & vect, const MCVector3dF & start, const MCVector3dF & end,
    MCUint steps, MCUint preDelay, MCUint postDelay)
{
    m_delta     = (end - start) / steps;
    m_vect      = &vect;
    m_steps     = steps;
    m_step      = 0;
    *m_vect     = start;
    m_preDelay  = preDelay;
    m_postDelay = postDelay;
}

bool MCVectorAnimation::update()
{
    assert(m_vect);

    if (m_preDelay)
    {
        m_preDelay--;
        return false;
    }
    else if (++m_step < m_steps)
    {
        *m_vect += m_delta;
        return false;
    }
    else if (m_postDelay)
    {
        m_postDelay--;
        return false;
    }

    return true;
}

MCVectorAnimation::~MCVectorAnimation()
{
}
