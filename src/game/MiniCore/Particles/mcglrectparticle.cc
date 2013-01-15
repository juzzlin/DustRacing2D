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
#include "mcglshaderprogram.hh"
#include "mcglvertex.hh"
#include "mccamera.hh"

#include <MCGLEW>

#include <cassert>

static const int NUM_VERTICES         = 6;
static const int NUM_COLOR_COMPONENTS = 4;
static const int VERTEX_DATA_SIZE     = sizeof(MCGLVertex) * NUM_VERTICES;
static const int COLOR_DATA_SIZE      = sizeof(GLfloat)    * NUM_VERTICES * NUM_COLOR_COMPONENTS;
static const int TOTAL_DATA_SIZE      = VERTEX_DATA_SIZE + COLOR_DATA_SIZE;

MCGLRectParticle::MCGLRectParticle(const std::string & typeID)
: MCParticle(typeID)
, m_r(1.0)
, m_g(1.0)
, m_b(1.0)
, m_a(1.0)
, m_program(nullptr)
{
    // Init vertice data for a quad
    const MCGLVertex vertices[NUM_VERTICES] =
    {
        {-1, -1, 0},
        {-1,  1, 0},
        { 1,  1, 0},
        {-1, -1, 0},
        { 1,  1, 0},
        { 1, -1, 0}
    };

    const GLfloat colors[NUM_VERTICES * NUM_COLOR_COMPONENTS] =
    {
        m_r, m_g, m_b, m_a,
        m_r, m_g, m_b, m_a,
        m_r, m_g, m_b, m_a,
        m_r, m_g, m_b, m_a,
        m_r, m_g, m_b, m_a,
        m_r, m_g, m_b, m_a
    };

    int offset = 0;

    glGenVertexArrays(1, &m_vba);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vba);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
        TOTAL_DATA_SIZE, nullptr, GL_STATIC_DRAW);

    // Vertex data
    glBufferSubData(GL_ARRAY_BUFFER, offset, VERTEX_DATA_SIZE, vertices);
    offset += VERTEX_DATA_SIZE;

    // Vertex color data
    glBufferSubData(GL_ARRAY_BUFFER, offset, COLOR_DATA_SIZE, colors);

    glVertexAttribPointer(MCGLShaderProgram::VAL_Vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(MCGLShaderProgram::VAL_Color,  4, GL_FLOAT, GL_FALSE, 0,
        (GLvoid *)(VERTEX_DATA_SIZE));

    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Vertex);
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Color);
}

MCGLRectParticle::~MCGLRectParticle()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vba);
}

void MCGLRectParticle::setShaderProgram(MCGLShaderProgram * program)
{
    m_program = program;
}

void MCGLRectParticle::setColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    m_r = r;
    m_g = g;
    m_b = b;
    m_a = a;
}

void MCGLRectParticle::beginBatch()
{
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_program->bind();
    glBindVertexArray(m_vba);
}

void MCGLRectParticle::endBatch()
{
    glBindVertexArray(0);
    glDisable(GL_BLEND);
}

void MCGLRectParticle::render(MCCamera * pCamera)
{
    // Scale radius if fading out
    MCFloat r = radius();
    if (animationStyle() == Shrink)
    {
        r *= scale();
    }

    if (r > 0 && m_program)
    {
        MCFloat x = location().i();
        MCFloat y = location().j();

        if (pCamera)
        {
            pCamera->mapToCamera(x, y);
        }

        // Scale alpha if fading out.
        GLfloat alpha = m_a;
        if (animationStyle() == FadeOut)
        {
            alpha *= scale();
        }

        m_program->translate(MCVector3dF(x, y, location().k()));
        m_program->rotate(angle());
        m_program->setColor(m_r, m_g, m_b, alpha);
        m_program->setScale(r, r, 1.0);

        glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);
    }
}

void MCGLRectParticle::renderShadow(MCCamera *)
{
    return;
}
