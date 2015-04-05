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

#include "mcglrectparticlerenderer.hh"

#include "mccamera.hh"
#include "mcglrectparticle.hh"
#include "mcglshaderprogram.hh"
#include "mcglcolor.hh"
#include "mctrigonom.hh"
#include "mcglvertex.hh"

#include <algorithm>
#include <cassert>

namespace {
#ifdef __MC_GLES__
const int NUM_VERTICES_PER_PARTICLE = 6;
#else
const int NUM_VERTICES_PER_PARTICLE = 4;
#endif
}

MCGLRectParticleRenderer::MCGLRectParticleRenderer(int maxBatchSize)
    : MCParticleRendererBase(maxBatchSize)
    , m_vertices(new MCGLVertex[maxBatchSize * NUM_VERTICES_PER_PARTICLE])
    , m_normals(new MCGLVertex[maxBatchSize * NUM_VERTICES_PER_PARTICLE])
    , m_colors(new MCGLColor[maxBatchSize * NUM_VERTICES_PER_PARTICLE])
{
    const int NUM_VERTICES = maxBatchSize * NUM_VERTICES_PER_PARTICLE;
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

void MCGLRectParticleRenderer::setBatch(
    const MCParticleRendererBase::ParticleVector & particles_, MCCamera * camera)
{
    if (!particles_.size()) {
        return;
    }

    ParticleVector particles = std::move(particles_);
    setBatchSize(std::min(static_cast<int>(particles.size()), maxBatchSize()));
    std::sort(particles.begin(), particles.end(), [] (const MCObject * l, const MCObject * r) {
        return l->location().k() < r->location().k();
    });

    const int NUM_VERTICES = batchSize() * NUM_VERTICES_PER_PARTICLE;
    const int VERTEX_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;
    const int NORMAL_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;
    const int COLOR_DATA_SIZE  = sizeof(MCGLColor)  * NUM_VERTICES;

    // Init vertice data for a quad

    static const MCGLVertex vertices[NUM_VERTICES_PER_PARTICLE] =
    {
    #ifdef __MC_GLES__
        {-1, -1, 0},
        { 1,  1, 0},
    #endif
        {-1,  1, 0},
        {-1, -1, 0},
        { 1, -1, 0},
        { 1,  1, 0}
    };

    static const MCGLVertex normals[NUM_VERTICES_PER_PARTICLE] =
    {
    #ifdef __MC_GLES__
        { 0, 0, 1},
        { 0, 0, 1},
    #endif
        { 0, 0, 1},
        { 0, 0, 1},
        { 0, 0, 1},
        { 0, 0, 1}
    };

    assert(dynamic_cast<MCGLRectParticle *>(particles.at(0)));

    int vertexIndex = 0;
    for (int i = 0; i < batchSize(); i++)
    {
        MCGLRectParticle * particle = static_cast<MCGLRectParticle *>(particles[i]);
        MCVector3dF location(particle->location());

        MCFloat x = location.i();
        MCFloat y = location.j();
        const MCFloat z = location.k();

        if (camera)
        {
            camera->mapToCamera(x, y);
        }

        for (int j = 0; j < NUM_VERTICES_PER_PARTICLE; j++)
        {
            MCFloat vertexX = vertices[j].x() * particle->radius() * particle->scale();
            MCFloat vertexY = vertices[j].y() * particle->radius() * particle->scale();

            m_colors[vertexIndex] = particle->color();
            if (particle->animationStyle() == MCParticle::FadeOut)
            {
                m_colors[vertexIndex].setA(m_colors[vertexIndex].a() * particle->scale());
            }
            else if (particle->animationStyle() == MCParticle::FadeOutAndExpand)
            {
                m_colors[vertexIndex].setA(m_colors[vertexIndex].a() * particle->scale());
                vertexX *= particle->scale();
                vertexY *= particle->scale();
            }
            else if (particle->animationStyle() == MCParticle::Shrink)
            {
                vertexX *= particle->scale();
                vertexY *= particle->scale();
            }

            m_vertices[vertexIndex] =
                MCGLVertex(
                    x + MCTrigonom::rotatedX(vertexX, vertexY, particle->angle()),
                    y + MCTrigonom::rotatedY(vertexX, vertexY, particle->angle()),
                    z);

            m_normals[vertexIndex] = normals[j];

            vertexIndex++;
        }
    }

    initUpdateBufferData();

    const int MAX_VERTEX_DATA_SIZE = sizeof(MCGLVertex) * maxBatchSize() * NUM_VERTICES_PER_PARTICLE;
    addBufferSubData(
        MCGLShaderProgram::VAL_Vertex, VERTEX_DATA_SIZE, MAX_VERTEX_DATA_SIZE,
                reinterpret_cast<const GLfloat *>(m_vertices));

    const int MAX_NORMAL_DATA_SIZE = sizeof(MCGLVertex) * maxBatchSize() * NUM_VERTICES_PER_PARTICLE;
    addBufferSubData(
        MCGLShaderProgram::VAL_Normal, NORMAL_DATA_SIZE, MAX_NORMAL_DATA_SIZE,
                reinterpret_cast<const GLfloat *>(m_normals));

    addBufferSubData(
        MCGLShaderProgram::VAL_Color, COLOR_DATA_SIZE, reinterpret_cast<const GLfloat *>(m_colors));
}

void MCGLRectParticleRenderer::render()
{
    assert(shaderProgram());
    shaderProgram()->bind();

    if (useAlphaBlend())
    {
        glEnable(GL_BLEND);
        glBlendFunc(alphaSrc(), alphaDst());
    }

    bindMaterial();

    shaderProgram()->setTransform(0, MCVector3dF(0, 0, 0));
    shaderProgram()->setScale(1.0f, 1.0f, 1.0f);
    shaderProgram()->setColor(MCGLColor());

#ifdef __MC_GLES__
    glDrawArrays(GL_TRIANGLES, 0, batchSize() * NUM_VERTICES_PER_PARTICLE);
#else
    glDrawArrays(GL_QUADS, 0, batchSize() * NUM_VERTICES_PER_PARTICLE);
#endif
    glDisable(GL_BLEND);

    releaseVBO();
    releaseVAO();
}

MCGLRectParticleRenderer::~MCGLRectParticleRenderer()
{
    delete [] m_vertices;
    delete [] m_normals;
    delete [] m_colors;
}

