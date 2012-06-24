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
#include "../mcglvertex.hh"
#include "../mccamera.hh"

#include <GL/gl.h>
#include <GL/glext.h>

#include <cassert>

MCRecycler<MCGLRectParticle> MCGLRectParticle::m_recycler;

namespace
{
static const int gNumVertices = 4;
}

MCGLRectParticle::MCGLRectParticle()
: m_r(1.0)
, m_g(1.0)
, m_b(1.0)
, m_a(1.0)
, m_group(nullptr)
{
    // Disable shadow by default
    setHasShadow(false);

    // Init vertice data for a quad
    const MCGLVertex vertices[gNumVertices] =
    {
        {-1, -1, 0},
        {-1,  1, 0},
        { 1,  1, 0},
        { 1, -1, 0}
    };

    const MCGLVertex normals[gNumVertices] =
    {
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1}
    };

    const GLfloat colors[] =
    {
        m_r, m_g, m_b, m_a,
        m_r, m_g, m_b, m_a,
        m_r, m_g, m_b, m_a,
        m_r, m_g, m_b, m_a
    };

    glGenBuffers(VBOTypes, m_vbos);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOVertex]);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(MCGLVertex) * gNumVertices, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBONormal]);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(MCGLVertex) * gNumVertices, normals, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOColor]);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GLfloat) * gNumVertices * 4, colors, GL_DYNAMIC_DRAW);
}

MCGLRectParticle::~MCGLRectParticle()
{
    glDeleteBuffers(VBOTypes, m_vbos);
}

void MCGLRectParticle::setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    m_r = r;
    m_g = g;
    m_b = b;
    m_a = a;

    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOColor]);
    GLfloat * pColorData = (GLfloat *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    if (pColorData)
    {
        for (int i = 0; i < 4; i++)
        {
            const int offset = (i << 2);
            pColorData[offset + 0] = r;
            pColorData[offset + 1] = g;
            pColorData[offset + 2] = b;
            pColorData[offset + 3] = a;
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
}

void MCGLRectParticle::setAlpha(MCFloat a)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOColor]);
    GLfloat * pColorData = (GLfloat *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    if (pColorData)
    {
        for (int i = 0; i < 4; i++)
        {
            pColorData[(i << 2) + 3] = a;
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }
}

void MCGLRectParticle::render(MCCamera * pCamera)
{
    // Disable texturing
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    renderInner(pCamera);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glPopAttrib();
}

void MCGLRectParticle::renderInner(MCCamera * pCamera)
{
    glPushMatrix();

    MCFloat x = location().i();
    MCFloat y = location().j();

    if (pCamera)
    {
        pCamera->mapToCamera(x, y);
    }

    glTranslated(x, y, location().k());
    glRotated(angle(), 0, 0, 1);

    // Scale alpha if fading out
    if (animationStyle() == FadeOut)
    {
        setAlpha(m_a * scale());
    }

    // Scale radius if fading out
    MCFloat r = radius();
    if (animationStyle() == Shrink)
    {
        r *= scale();
    }

    if (r > 0)
    {
        glScaled(r, r, 1.0);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOVertex]);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBONormal]);
        glNormalPointer(GL_FLOAT, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[VBOColor]);
        glColorPointer(4, GL_FLOAT, 0, 0);
        glDrawArrays(GL_QUADS, 0, gNumVertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
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
