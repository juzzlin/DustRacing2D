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

#include "mcsurfaceparticlerenderer.hh"

#include "mcmathutil.hh"
#include "mcsurfaceparticle.hh"
#include "mctrigonom.hh"

#include <algorithm>

#ifdef __MC_GLES__
const size_t MCSurfaceParticleRenderer::m_numVerticesPerParticle = 6;
#else
const size_t MCSurfaceParticleRenderer::m_numVerticesPerParticle = 4;
#endif

MCSurfaceParticleRenderer::MCSurfaceParticleRenderer(size_t maxBatchSize)
  : MCParticleRendererBase(maxBatchSize)
  , m_vertices(maxBatchSize * m_numVerticesPerParticle)
  , m_normals(maxBatchSize * m_numVerticesPerParticle)
  , m_texCoords(maxBatchSize * m_numVerticesPerParticle)
  , m_colors(maxBatchSize * m_numVerticesPerParticle)
{
    const auto numVertices = maxBatchSize * m_numVerticesPerParticle;

    const auto vertexDataSize = sizeof(MCGLVertex) * numVertices;
    const auto normalDataSize = sizeof(MCGLVertex) * numVertices;
    const auto texCoordDataSize = sizeof(MCGLTexCoord) * numVertices;
    const auto colorDataSize = sizeof(MCGLColor) * numVertices;

    const auto totalDataSize = vertexDataSize + normalDataSize + texCoordDataSize + colorDataSize;

    initBufferData(totalDataSize, GL_DYNAMIC_DRAW);

    addBufferSubData(MCGLShaderProgram::VAL_Vertex, vertexDataSize, reinterpret_cast<const GLfloat *>(m_vertices.data()));
    addBufferSubData(MCGLShaderProgram::VAL_Normal, normalDataSize, reinterpret_cast<const GLfloat *>(m_normals.data()));
    addBufferSubData(MCGLShaderProgram::VAL_TexCoords, texCoordDataSize, reinterpret_cast<const GLfloat *>(m_texCoords.data()));
    addBufferSubData(MCGLShaderProgram::VAL_Color, colorDataSize, reinterpret_cast<const GLfloat *>(m_colors.data()));

    finishBufferData();
}

void MCSurfaceParticleRenderer::setBatch(MCRenderLayer::ObjectBatch & batch, MCCamera * camera, bool isShadow)
{
    if (!batch.objects.size())
    {
        return;
    }

    setBatchSize(std::min(batch.objects.size(), maxBatchSize()));
    std::sort(batch.objects.begin(), batch.objects.end(), [](auto lhs, auto rhs) {
        return lhs->location().k() < rhs->location().k();
    });

    const auto numVertices = batchSize() * m_numVerticesPerParticle;
    const auto vertexDataSize = sizeof(MCGLVertex) * numVertices;
    const auto normalDataSize = sizeof(MCGLVertex) * numVertices;
    const auto texCoordDataSize = sizeof(MCGLTexCoord) * numVertices;
    const auto colorDataSize = sizeof(MCGLColor) * numVertices;

    // Init vertice data for a quad

    static const MCGLVertex vertices[m_numVerticesPerParticle] = {
#ifdef __MC_GLES__
        { -1, -1, 0 },
        { 1, 1, 0 },
#endif
        { -1, 1, 0 },
        { -1, -1, 0 },
        { 1, -1, 0 },
        { 1, 1, 0 }
    };

    static const MCGLVertex normals[m_numVerticesPerParticle] = {
#ifdef __MC_GLES__
        { 0, 0, 1 },
        { 0, 0, 1 },
#endif
        { 0, 0, 1 },
        { 0, 0, 1 },
        { 0, 0, 1 },
        { 0, 0, 1 }
    };

    const MCGLTexCoord texCoords[m_numVerticesPerParticle] = {
#ifdef __MC_GLES__
        { 0, 0 },
        { 1, 1 },
#endif
        { 0, 1 },
        { 0, 0 },
        { 1, 0 },
        { 1, 1 }
    };

    // Take common properties from the first particle in the batch
    const auto particle = dynamic_cast<MCSurfaceParticle *>(batch.objects.at(0));
    setMaterial(particle->surface()->material());
    setHasShadow(particle->hasShadow());
    setAlphaBlend(particle->useAlphaBlend(), particle->alphaSrc(), particle->alphaDst());

    size_t vertexIndex = 0;
    for (size_t i = 0; i < batchSize(); i++)
    {
        const auto particle = static_cast<MCSurfaceParticle *>(batch.objects[i]);
        const auto location(particle->location());

        float x, y, z;

        if (isShadow)
        {
            x = location.i() + particle->shape()->shadowOffset().i();
            y = location.j() + particle->shape()->shadowOffset().j();
            z = particle->shape()->shadowOffset().k();
        }
        else
        {
            x = location.i();
            y = location.j();
            z = location.k();
        }

        if (camera)
        {
            camera->mapToCamera(x, y);
        }

        for (size_t j = 0; j < m_numVerticesPerParticle; j++)
        {
            float vertexX = vertices[j].x() * particle->radius();
            float vertexY = vertices[j].y() * particle->radius();

            m_colors[vertexIndex] = particle->color();
            if (particle->animationStyle() == MCParticle::AnimationStyle::FadeOut)
            {
                m_colors[vertexIndex].setA(m_colors[vertexIndex].a() * particle->scale());
            }
            else if (particle->animationStyle() == MCParticle::AnimationStyle::FadeOutAndExpand)
            {
                m_colors[vertexIndex].setA(m_colors[vertexIndex].a() * particle->scale());
                vertexX *= particle->scale();
                vertexY *= particle->scale();
            }
            else if (particle->animationStyle() == MCParticle::AnimationStyle::Shrink)
            {
                vertexX *= particle->scale();
                vertexY *= particle->scale();
            }

            m_vertices[vertexIndex] =
              MCGLVertex(
                x + MCMathUtil::rotatedX(vertexX, vertexY, particle->angle()),
                y + MCMathUtil::rotatedY(vertexX, vertexY, particle->angle()),
                z);

            m_normals[vertexIndex] = normals[j];

            m_texCoords[vertexIndex] = texCoords[j];

            vertexIndex++;
        }
    }

    initUpdateBufferData();

    const auto maxVertexDataSize = sizeof(MCGLVertex) * maxBatchSize() * m_numVerticesPerParticle;
    addBufferSubData(
      MCGLShaderProgram::VAL_Vertex, vertexDataSize, maxVertexDataSize,
      reinterpret_cast<const GLfloat *>(m_vertices.data()));

    const auto maxNormalDataSize = sizeof(MCGLVertex) * maxBatchSize() * m_numVerticesPerParticle;
    addBufferSubData(
      MCGLShaderProgram::VAL_Normal, normalDataSize, maxNormalDataSize,
      reinterpret_cast<const GLfloat *>(m_normals.data()));

    const auto maxTexCoordDataSize = sizeof(MCGLTexCoord) * maxBatchSize() * m_numVerticesPerParticle;
    addBufferSubData(
      MCGLShaderProgram::VAL_TexCoords, texCoordDataSize, maxTexCoordDataSize,
      reinterpret_cast<const GLfloat *>(m_texCoords.data()));

    addBufferSubData(
      MCGLShaderProgram::VAL_Color, colorDataSize, reinterpret_cast<const GLfloat *>(m_colors.data()));
}

void MCSurfaceParticleRenderer::render()
{
    assert(shaderProgram());

    bind();

    shaderProgram()->setTransform(0, MCVector3dF(0, 0, 1));
    shaderProgram()->setScale(1.0f, 1.0f, 1.0f);
    shaderProgram()->setColor(MCGLColor(1.0f, 1.0f, 1.0f, 1.0f));

#ifdef __MC_GLES__
    const auto mode = GL_TRIANGLES;
#else
    const auto mode = GL_QUADS;
#endif
    glDrawArrays(mode, 0, static_cast<int>(batchSize() * m_numVerticesPerParticle));
    glDisable(GL_BLEND);

    releaseVBO();
    releaseVAO();
}

void MCSurfaceParticleRenderer::renderShadows()
{
    assert(shadowShaderProgram());

    bindShadow();

    shadowShaderProgram()->setTransform(0, MCVector3dF(0, 0, 0));
    shadowShaderProgram()->setScale(1.0f, 1.0f, 1.0f);

#ifdef __MC_GLES__
    const auto mode = GL_TRIANGLES;
#else
    const auto mode = GL_QUADS;
#endif
    glDrawArrays(mode, 0, static_cast<int>(batchSize() * m_numVerticesPerParticle));

    releaseVBO();
    releaseVAO();
}
