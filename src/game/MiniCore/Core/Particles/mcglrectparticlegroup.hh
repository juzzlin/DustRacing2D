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

#ifndef MCGLRECTPARTICLEGROUP_HH
#define MCGLRECTPARTICLEGROUP_HH

#include "mcparticle.hh"

class MCGLRectParticleGroupImpl;
class MCGLRectParticle;
class MCCamera;

/*! \class MCGLRectParticleGroup.
 *  \brief A virtual object that renders a group of MCGLRectParticles efficiently.
 *
 *  Just instantiate a MCGLRectParticleGroup and add it to the world.
 *  Add MCGLRectParticles to the group after creation. The group renders
 *  the particles when it gets rendered.
 *
 *  There can be multiple groups.
 */
class MCGLRectParticleGroup : public MCObject
{
public:

    //! Constructor.
    MCGLRectParticleGroup();

    //! Destructor.
    virtual ~MCGLRectParticleGroup();

    //! Adds the given particle to the group. Won't take the ownership.
    //! Does nothing if already in this group.
    void addParticle(MCGLRectParticle & particle);

    //! Clear the group.
    void clear();

    //! \reimp
    virtual void render(MCCamera * pCamera = nullptr);

    //! \reimp
    virtual void renderShadow(MCCamera * pCamera = nullptr);

private:

    DISABLE_COPY(MCGLRectParticleGroup);
    DISABLE_ASSI(MCGLRectParticleGroup);
    MCGLRectParticleGroupImpl * const m_pImpl;
};

#endif // MCGLRECTPARTICLEGROUP_HH
