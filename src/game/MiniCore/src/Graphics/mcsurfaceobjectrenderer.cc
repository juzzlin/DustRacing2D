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

#include "mcsurfaceobjectrenderer.hh"

#include "mccamera.hh"
#include "mcmathutil.hh"
#include "mcsurface.hh"
#include "mcsurfaceview.hh"
#include "mctrigonom.hh"

#include <algorithm>

MCSurfaceObjectRenderer::MCSurfaceObjectRenderer(size_t maxBatchSize)
  : MCObjectRendererBase(maxBatchSize)
  , m_vertices(maxBatchSize * m_numVerticesPerSurface)
  , m_normals(maxBatchSize * m_numVerticesPerSurface)
  , m_texCoords(maxBatchSize * m_numVerticesPerSurface)
  , m_colors(maxBatchSize * m_numVerticesPerSurface)
{
    const auto numVertices = maxBatchSize * m_numVerticesPerSurface;

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

void MCSurfaceObjectRenderer::setBatch(MCRenderLayer::ObjectBatch & batch, MCCamera * camera, bool isShadow)
{
    if (!batch.objects.size())
    {
        return;
    }

    setBatchSize(std::min(batch.objects.size(), maxBatchSize()));
    std::sort(batch.objects.begin(), batch.objects.end(), [](const MCObject * l, const MCObject * r) {
        return l->location().k() < r->location().k();
    });

    const auto numVertices = batchSize() * m_numVerticesPerSurface;
    const auto vertexDataSize = sizeof(MCGLVertex) * numVertices;
    const auto normalDataSize = sizeof(MCGLVertex) * numVertices;
    const auto texCoordDataSize = sizeof(MCGLTexCoord) * numVertices;
    const auto colorDataSize = sizeof(MCGLColor) * numVertices;

    // Take common properties from the first Object in the batch
    auto object = batch.objects.at(0);
    const auto view = dynamic_cast<MCSurfaceView *>(object->shape()->view().get());
    assert(view);

    m_surface = view->surface();

    setShaderProgram(view->shaderProgram());
    setShadowShaderProgram(view->shadowShaderProgram());

    setMaterial(m_surface->material());
    setHasShadow(view->hasShadow());

    size_t vertexIndex = 0;
    for (size_t i = 0; i < batchSize(); i++)
    {
        object = batch.objects[i];
        const auto view = static_cast<MCSurfaceView *>(object->shape()->view().get());
        const auto location(object->shape()->location());

        float x, y, z;
        if (isShadow)
        {
            x = location.i() + object->shape()->shadowOffset().i();
            y = location.j() + object->shape()->shadowOffset().j();
            z = object->shape()->shadowOffset().k();
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

        for (size_t j = 0; j < m_numVerticesPerSurface; j++)
        {
            m_colors[vertexIndex] = std::static_pointer_cast<MCGLObjectBase>(m_surface)->color(j);

            const auto vertex = m_surface->vertex(j);

            m_vertices[vertexIndex] =
              MCGLVertex(
                x + MCMathUtil::rotatedX(vertex.x(), vertex.y(), object->angle()) * view->scale().i(),
                y + MCMathUtil::rotatedY(vertex.x(), vertex.y(), object->angle()) * view->scale().j(),
                !isShadow ? z + vertex.z() : z);

            m_normals[vertexIndex] = m_surface->normal(j);

            m_texCoords[vertexIndex] = m_surface->texCoord(j);

            vertexIndex++;
        }
    }

    initUpdateBufferData();

    const auto maxVertexDataSize = sizeof(MCGLVertex) * maxBatchSize() * m_numVerticesPerSurface;
    addBufferSubData(MCGLShaderProgram::VAL_Vertex, vertexDataSize, maxVertexDataSize, reinterpret_cast<const GLfloat *>(m_vertices.data()));

    const auto maxNormalDataSize = sizeof(MCGLVertex) * maxBatchSize() * m_numVerticesPerSurface;
    addBufferSubData(MCGLShaderProgram::VAL_Normal, normalDataSize, maxNormalDataSize, reinterpret_cast<const GLfloat *>(m_normals.data()));

    const auto maxTexCoordDataSize = sizeof(MCGLTexCoord) * maxBatchSize() * m_numVerticesPerSurface;
    addBufferSubData(MCGLShaderProgram::VAL_TexCoords, texCoordDataSize, maxTexCoordDataSize, reinterpret_cast<const GLfloat *>(m_texCoords.data()));

    addBufferSubData(MCGLShaderProgram::VAL_Color, colorDataSize, reinterpret_cast<const GLfloat *>(m_colors.data()));
}

void MCSurfaceObjectRenderer::render()
{
    assert(shaderProgram());

    bind();

    material()->doAlphaBlend();

    shaderProgram()->setTransform(0, MCVector3dF(0, 0, 1));
    shaderProgram()->setScale(1.0f, 1.0f, 1.0f);
    shaderProgram()->setColor(m_surface->color());

    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(batchSize() * m_numVerticesPerSurface));

    releaseVBO();
    releaseVAO();
}

void MCSurfaceObjectRenderer::renderShadows()
{
    assert(shadowShaderProgram());

    bindShadow();

    shadowShaderProgram()->setTransform(0, MCVector3dF(0, 0, 0));
    shadowShaderProgram()->setScale(1.0f, 1.0f, 1.0f);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(batchSize() * m_numVerticesPerSurface));

    releaseVBO();
    releaseVAO();
}
