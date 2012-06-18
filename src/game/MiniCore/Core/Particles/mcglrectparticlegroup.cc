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

#include "mcglrectparticle.hh"
#include "mcglrectparticlegroup.hh"
#include "../mccamera.hh"
#include "../mcshape.hh"
#include "../mcshapeview.hh"

#include <GL/gl.h>

MCGLRectParticleGroup::MCGLRectParticleGroup()
: MCObject("MCGLRECTPARTICLEGROUP")
{
    setHasShadow(false);
    setRenderable(false);
    setPhysicsObject(false);
    setIsParticleGroup(true);
}

void MCGLRectParticleGroup::render(MCCamera * pCamera)
{
    // Disable texturing
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int i2 = m_particles.size();
    for (unsigned int i = 0; i < i2; i++)
    {
        // Render the particle if it belongs to this group.
        MCGLRectParticle * const p = m_particles[i];
        if (p->group() == this)
        {
            // This is normally checked in MCWorld::render().
            if (!pCamera || pCamera->isVisible(p->bbox()))
            {
                p->renderInner(pCamera);
            }
        }
        else
        {
            // Didn't belong to this group, remove from the vector.
            m_particles[i] = m_particles.back();
            m_particles.pop_back();
            i2--;
        }
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glPopAttrib();
}

void MCGLRectParticleGroup::renderShadow(MCCamera *)
{
    return;
}

void MCGLRectParticleGroup::addParticle(MCGLRectParticle & particle)
{
    if (particle.group() != this)
    {
        particle.setGroup(this);
        m_particles.push_back(&particle);
    }
}

void MCGLRectParticleGroup::clear()
{
    for (MCGLRectParticle * p : m_particles)
    {
        if (p->group() == this)
        {
            p->setGroup(nullptr);
        }
    }

    m_particles.clear();
}

MCGLRectParticleGroup::~MCGLRectParticleGroup()
{
    clear();
}
