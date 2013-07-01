// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcglpointparticlerenderer.hh"

#include "mccamera.hh"
#include "mcglpointparticle.hh"
#include "mcglshaderprogram.hh"
#include "mcglcolor.hh"
#include "mctrigonom.hh"
#include "mcglvertex.hh"

#include <algorithm>
#include <cassert>

static const int NUM_COLOR_COMPONENTS = 4;

MCGLPointParticleRenderer::MCGLPointParticleRenderer(int maxBatchSize)
: m_batchSize(0)
, m_maxBatchSize(maxBatchSize)
, m_vertices(new MCGLVertex[maxBatchSize])
, m_normals(new MCGLVertex[maxBatchSize])
, m_colors(new MCGLColor[maxBatchSize])
, m_pointSize(8)
, m_useAlphaBlend(false)
{
    const int NUM_VERTICES     = maxBatchSize;
    const int VERTEX_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;
    const int NORMAL_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;
    const int COLOR_DATA_SIZE  = sizeof(MCGLColor)  * NUM_VERTICES;
    const int TOTAL_DATA_SIZE  = VERTEX_DATA_SIZE   + NORMAL_DATA_SIZE + COLOR_DATA_SIZE;

    createVAO();
    createVBO();

    bindVAO();
    bindVBO();

    glBufferData(GL_ARRAY_BUFFER, TOTAL_DATA_SIZE, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(MCGLShaderProgram::VAL_Vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(MCGLShaderProgram::VAL_Normal, 3, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid *>(VERTEX_DATA_SIZE));
    glVertexAttribPointer(MCGLShaderProgram::VAL_Color,  4, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid *>(VERTEX_DATA_SIZE + NORMAL_DATA_SIZE));

    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Vertex);
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Normal);
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Color);
}

void MCGLPointParticleRenderer::setPointSize(int pointSize)
{
    m_pointSize = pointSize;
}

void MCGLPointParticleRenderer::setAlphaBlend(bool useAlphaBlend, GLenum src, GLenum dst)
{
    m_useAlphaBlend = useAlphaBlend;
    m_src           = src;
    m_dst           = dst;
}

void MCGLPointParticleRenderer::setBatch(
    const MCGLPointParticleRenderer::ParticleVector & particles, MCCamera * camera)
{
    bindVAO();
    bindVBO();

    int offset = 0;

    m_batchSize = std::min(static_cast<int>(particles.size()), m_maxBatchSize);

    const int NUM_VERTICES     = m_batchSize;
    const int VERTEX_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;
    const int NORMAL_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;
    const int COLOR_DATA_SIZE  = sizeof(MCGLColor)  * NUM_VERTICES;

    for (int i = 0; i < m_batchSize; i++)
    {
        MCGLPointParticle * particle = static_cast<MCGLPointParticle *>(particles[i]);
        MCVector3dF location(particle->location());

        MCFloat x = location.i();
        MCFloat y = location.j();

        if (camera)
        {
            camera->mapToCamera(x, y);
        }

        m_vertices[i] = MCGLVertex(x, y, location.k());
        m_colors[i]   = particle->color();

        if (particle->animationStyle() == MCParticle::FadeOut)
        {
            m_colors[i].setA(m_colors[i].a() * particle->scale());
        }

        // Use normal data for sin and cos in order to rotate texture coordinates.
        // We cannot apply transformations for GL_POINT's.
        m_normals[i] = MCGLVertex(
            MCTrigonom::cos(particle->angle()),
            MCTrigonom::sin(particle->angle()));
    }

    // Vertex data
    glBufferSubData(GL_ARRAY_BUFFER, offset, VERTEX_DATA_SIZE, m_vertices);
    const int MAX_VERTEX_DATA_SIZE = sizeof(MCGLVertex) * m_maxBatchSize;
    offset += MAX_VERTEX_DATA_SIZE;

    // Normal data
    glBufferSubData(GL_ARRAY_BUFFER, offset, NORMAL_DATA_SIZE, m_normals);
    const int MAX_NORMAL_DATA_SIZE = sizeof(MCGLVertex) * m_maxBatchSize;
    offset += MAX_NORMAL_DATA_SIZE;

    // Vertex color data
    glBufferSubData(GL_ARRAY_BUFFER, offset, COLOR_DATA_SIZE, m_colors);
}

void MCGLPointParticleRenderer::render()
{
    assert(shaderProgram());
    shaderProgram()->bind();

#ifndef __MC_GLES__
    // Note: This is not needed from OpenGL 3.3 onwards
    glEnable(GL_POINT_SPRITE);
#endif

    if (m_useAlphaBlend)
    {
        glEnable(GL_BLEND);
        glBlendFunc(m_src, m_dst);
    }

    if (texture1())
    {
        bindTextures(true);
    }

    shaderProgram()->setPointSize(m_pointSize);

    glDrawArrays(GL_POINTS, 0, m_batchSize);

    if (m_useAlphaBlend)
    {
        glDisable(GL_BLEND);
    }

#ifndef __MC_GLES__
    glDisable(GL_POINT_SPRITE);
#endif

    releaseVBO();
    releaseVAO();
}

MCGLPointParticleRenderer::~MCGLPointParticleRenderer()
{
    delete [] m_vertices;
    delete [] m_normals;
    delete [] m_colors;
}

