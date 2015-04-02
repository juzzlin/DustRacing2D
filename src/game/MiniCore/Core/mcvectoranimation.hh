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

#ifndef MCVECTORANIMATION_HH
#define MCVECTORANIMATION_HH

#include "mctypes.hh"
#include "mcmacros.hh"
#include "mcvector3d.hh"

class MCVectorAnimationImpl;

/*!
 *  \class MCVectorAnimation
 *
 *  MCVectorAnimation implements a simple class to animate
 *  an MCVector3d.
 *
 */
class MCVectorAnimation
{
public:

    //! Constructor.
    MCVectorAnimation();

    //! Destructor.
    virtual ~MCVectorAnimation();

    //! Initialize the animation.
    void init(
        MCVector3dF & vect, MCVector3dFR start, MCVector3dFR & end, MCUint steps,
        MCUint preDelay = 0, MCUint postDelay = 0);

    //! Update the animation. Returns true when done.
    bool update();

private:

    DISABLE_COPY(MCVectorAnimation);
    DISABLE_ASSI(MCVectorAnimation);

    MCVector3dF * m_vect;

    MCVector3dF m_start;

    MCVector3dF m_end;

    MCVector3dF m_delta;

    MCUint m_steps;

    MCUint m_step;

    MCUint m_preDelay;

    MCUint m_postDelay;
};

#endif // MCVECTORANIMATION_HH

