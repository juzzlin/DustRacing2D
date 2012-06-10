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

#ifndef MCGLRECTPARTICLE_HH
#define MCGLRECTPARTICLE_HH

#include "mcparticle.hh"
#include "mcglrectparticlegroup.hh"

#include <GL/gl.h>

class MCCamera;

/*! \class MCGLRectParticle
 *  \brief A particle that renders as a simple OpenGL rectangle. Final class.
 */
class MCGLRectParticle : public MCParticle
{
public:

    //! Create a new particle
    static MCGLRectParticle & create();

    //! \reimp
    void recycle();

    //! Set color
    void setColor(MCFloat r, MCFloat g, MCFloat b, MCFloat a = 1.0);

    //! \reimp
    void render(MCCamera * pCamera = nullptr);

    //! \reimp
    void renderShadow(MCCamera * pCamera = nullptr);

private:

    //! Constructor.
    MCGLRectParticle();

    //! Destructor.
    ~MCGLRectParticle();

    //! Inner calls of render.
    //! Used by MCGLRectParticleGroup.
    void renderInner(MCCamera * pCamera);

    //! Returns a pointer to the group or nullptr if not added to any group.
    //! Used by MCGLRectParticleGroup.
    MCGLRectParticleGroup * group() const;

    //! Set pointer to the current group or nullptr if not added to any group.
    //! Used by MCGLRectParticleGroup.
    void setGroup(MCGLRectParticleGroup * group);

    //! Recycler object
    static MCRecycler<MCGLRectParticle> m_recycler;
    friend class MCRecycler<MCGLRectParticle>;
    friend class MCGLRectParticleGroup;

    DISABLE_COPY(MCGLRectParticle);
    DISABLE_ASSI(MCGLRectParticle);
    MCFloat m_r, m_g, m_b, m_a;
    MCGLRectParticleGroup * m_group;
    static GLuint m_listIndex;
    static GLuint m_listIndex2;
};

#endif // MCGLRECTPARTICLE_HH
