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

class MCVectorAnimationImpl
{
    MCVectorAnimationImpl();
    void init(
        MCVector3dF & vect, const MCVector3dF & start, const MCVector3dF & end,
        MCUint steps);
    bool update();

    MCVector3dF * pVect;
    MCVector3dF   delta;
    MCUint        steps;
    MCUint        step;

    friend class MCVectorAnimation;
};

MCVectorAnimationImpl::MCVectorAnimationImpl()
: pVect(nullptr)
{
}

void MCVectorAnimationImpl::init(
    MCVector3dF       & vect,
    const MCVector3dF & start,
    const MCVector3dF & end,
    MCUint steps_)
{
    delta  = (end - start) / steps_;
    pVect  = &vect;
    steps  = steps_;
    step   = 0;
    *pVect = start;
}

bool MCVectorAnimationImpl::update()
{
    assert(pVect);
    if (++step < steps)
    {
        *pVect += delta;
        return false;
    }
    return true;
}

MCVectorAnimation::MCVectorAnimation()
: m_pImpl(new MCVectorAnimationImpl)
{
}

MCVectorAnimation::~MCVectorAnimation()
{
    delete m_pImpl;
}

void MCVectorAnimation::init(
    MCVector3dF & vect,
    const MCVector3dF & start,
    const MCVector3dF & end,
    MCUint steps)
{
    m_pImpl->init(vect, start, end, steps);
}

bool MCVectorAnimation::update()
{
    return m_pImpl->update();
}
