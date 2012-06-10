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

#include "mcglrectparticle.hh"
#include "../mccamera.hh"

#include <GL/gl.h>
#include <cassert>

MCRecycler<MCGLRectParticle> MCGLRectParticle::m_recycler;
GLuint MCGLRectParticle::m_listIndex  = 0;
GLuint MCGLRectParticle::m_listIndex2 = 0;

MCGLRectParticle::MCGLRectParticle()
: m_r(1.0)
, m_g(1.0)
, m_b(1.0)
, m_a(1.0)
, m_group(nullptr)
{
    // Disable shadow by default
    setHasShadow(false);
}

MCGLRectParticle::~MCGLRectParticle()
{
    if (MCGLRectParticle::m_listIndex)
    {
        glDeleteLists(MCGLRectParticle::m_listIndex, 1);
        MCGLRectParticle::m_listIndex = 0;
    }
}

void MCGLRectParticle::setColor(MCFloat r, MCFloat g, MCFloat b, MCFloat a)
{
    m_r = r;
    m_g = g;
    m_b = b;
    m_a = a;
}

void MCGLRectParticle::render(MCCamera * pCamera)
{
    // Disable texturing
    if (!m_listIndex2)
    {
        m_listIndex2 = glGenLists(1);
        assert(m_listIndex2 != 0);
        glNewList(m_listIndex2, GL_COMPILE);
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEndList();
    }
    else
    {
        glCallList(m_listIndex2);
    }

    renderInner(pCamera);

    glPopAttrib();
}

void MCGLRectParticle::renderInner(MCCamera * pCamera)
{
    glPushMatrix();

    MCFloat x = location().i();
    MCFloat y = location().j();

    if (pCamera) {
        pCamera->mapToCamera(x, y);
    }

    glTranslated(x, y, location().k());

    // Rotate
    if (angle() > 0)
    {
        glRotated(angle(), 0, 0, 1);
    }

    // Scale alpha if fading out
    MCFloat alpha = m_a;
    if (animationStyle() == FadeOut)
    {
        alpha *= scale();
    }

    // Scale radius if fading out
    MCFloat r = radius();
    if (animationStyle() == Shrink)
    {
        r *= scale();
    }

    if (r > 0)
    {
        glColor4f(m_r, m_g, m_b, alpha);
        glScaled(r, r, 1.0);

        if (!m_listIndex)
        {
            m_listIndex = glGenLists(1);
            assert(m_listIndex != 0);
            glNewList(m_listIndex, GL_COMPILE);
            glNormal3i(0, 0, 1);
            glBegin(GL_QUADS);
            glVertex2f(-1,  1);
            glVertex2f( 1,  1);
            glVertex2f( 1, -1);
            glVertex2f(-1, -1);
            glEnd();
            glEndList();
        }
        else
        {
            glCallList(m_listIndex);
        }
    }

    glPopMatrix();
}

void MCGLRectParticle::renderShadow(MCCamera *)
{
    return;
}

MCGLRectParticle & MCGLRectParticle::create()
{
    return *MCGLRectParticle::m_recycler.newObject();
}

void MCGLRectParticle::recycle()
{
    MCGLRectParticle::m_recycler.freeObject(this);
}

MCGLRectParticleGroup * MCGLRectParticle::group() const
{
    return m_group;
}

void MCGLRectParticle::setGroup(MCGLRectParticleGroup * group)
{
    // Prevents rendering by MCWorld if group is set.
    // In this case the group takes care of the rendering calls.
    setRenderable(group == nullptr);

    m_group = group;
}
