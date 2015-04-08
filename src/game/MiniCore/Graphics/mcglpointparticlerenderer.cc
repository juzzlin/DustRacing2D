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

MCGLPointParticleRenderer::MCGLPointParticleRenderer(int maxBatchSize)
    : MCParticleRendererBase(maxBatchSize)
    , m_vertices(new MCGLVertex[maxBatchSize])
    , m_normals(new MCGLVertex[maxBatchSize])
    , m_colors(new MCGLColor[maxBatchSize])
    , m_pointSize(8)
{
    const int NUM_VERTICES     = maxBatchSize;
    const int VERTEX_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;
    const int NORMAL_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;
    const int COLOR_DATA_SIZE  = sizeof(MCGLColor)  * NUM_VERTICES;
    const int TOTAL_DATA_SIZE  = VERTEX_DATA_SIZE   + NORMAL_DATA_SIZE + COLOR_DATA_SIZE;

    initBufferData(TOTAL_DATA_SIZE, GL_DYNAMIC_DRAW);

    addBufferSubData(
        MCGLShaderProgram::VAL_Vertex, VERTEX_DATA_SIZE, reinterpret_cast<const GLfloat *>(m_vertices));
    addBufferSubData(
        MCGLShaderProgram::VAL_Normal, NORMAL_DATA_SIZE, reinterpret_cast<const GLfloat *>(m_normals));
    addBufferSubData(
        MCGLShaderProgram::VAL_Color, COLOR_DATA_SIZE, reinterpret_cast<const GLfloat *>(m_colors));

    finishBufferData();
}

void MCGLPointParticleRenderer::setPointSize(int pointSize)
{
    m_pointSize = pointSize;
}

void MCGLPointParticleRenderer::setBatch(MCParticleRendererBase::ParticleVector & particles, MCCamera * camera)
{
    if (!particles.size()) {
        return;
    }

    setBatchSize(std::min(static_cast<int>(particles.size()), maxBatchSize()));
    std::sort(particles.begin(), particles.end(), [] (const MCObject * l, const MCObject * r) {
        return l->location().k() < r->location().k();
    });

    const int NUM_VERTICES     = batchSize();
    const int VERTEX_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;
    const int NORMAL_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;
    const int COLOR_DATA_SIZE  = sizeof(MCGLColor)  * NUM_VERTICES;

    assert(dynamic_cast<MCGLPointParticle *>(particles.at(0)));

    for (int i = 0; i < batchSize(); i++)
    {
        MCGLPointParticle * particle = static_cast<MCGLPointParticle *>(particles[i]);
        MCVector3dF location(particle->location());

        MCFloat x = location.i();
        MCFloat y = location.j();

        if (camera)
        {
            camera->mapToCamera(x, y);
        }

        m_vertices[i] = MCGLVertex(x, y, 1);
        m_colors[i]   = particle->color();

        if (particle->animationStyle() == MCParticle::FadeOut)
        {
            m_colors[i].setA(m_colors[i].a() * particle->scale());
        }
        else if (particle->animationStyle() == MCParticle::FadeOutAndExpand)
        {
            m_colors[i].setA(m_colors[i].a() * particle->scale());
            m_vertices[i].setZ(2.0 - 2.0 * particle->scale());
        }

        // Use normal data for sin and cos in order to rotate texture coordinates.
        // We cannot apply transformations to GL_POINT's.
        m_normals[i] = MCGLVertex(
            MCTrigonom::cos(particle->angle()),
            MCTrigonom::sin(particle->angle()));
    }

    initUpdateBufferData();

    const int MAX_VERTEX_DATA_SIZE = sizeof(MCGLVertex) * maxBatchSize();
    addBufferSubData(
        MCGLShaderProgram::VAL_Vertex, VERTEX_DATA_SIZE, MAX_VERTEX_DATA_SIZE,
                reinterpret_cast<const GLfloat *>(m_vertices));

    const int MAX_NORMAL_DATA_SIZE = sizeof(MCGLVertex) * maxBatchSize();
    addBufferSubData(
        MCGLShaderProgram::VAL_Normal, NORMAL_DATA_SIZE, MAX_NORMAL_DATA_SIZE,
                reinterpret_cast<const GLfloat *>(m_normals));

    addBufferSubData(
        MCGLShaderProgram::VAL_Color, COLOR_DATA_SIZE, reinterpret_cast<const GLfloat *>(m_colors));
}

void MCGLPointParticleRenderer::render()
{
    assert(shaderProgram());
    shaderProgram()->bind();

#ifndef __MC_GLES__
    // Note: This is not needed from OpenGL 3.3 onwards
    glEnable(GL_POINT_SPRITE);
#endif

    if (useAlphaBlend())
    {
        glEnable(GL_BLEND);
        glBlendFunc(alphaSrc(), alphaDst());
    }

    bindMaterial();

    shaderProgram()->setPointSize(m_pointSize);

    glDrawArrays(GL_POINTS, 0, batchSize());

    if (useAlphaBlend())
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

