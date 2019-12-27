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

#include "mcsurfaceobjectrendererlegacy.hh"

#include "mccamera.hh"
#include "mcmathutil.hh"
#include "mcsurface.hh"
#include "mcsurfaceview.hh"
#include "mctrigonom.hh"

#include <algorithm>

MCSurfaceObjectRendererLegacy::MCSurfaceObjectRendererLegacy(size_t maxBatchSize)
  : MCObjectRendererBase(maxBatchSize)
  , m_vertices(maxBatchSize * m_numVerticesPerSurface)
  , m_normals(maxBatchSize * m_numVerticesPerSurface)
  , m_texCoords(maxBatchSize * m_numVerticesPerSurface)
  , m_colors(maxBatchSize * m_numVerticesPerSurface)
{
}

void MCSurfaceObjectRendererLegacy::setBatch(MCRenderLayer::ObjectBatch & batch, MCCamera * camera, bool isShadow)
{
    if (!batch.objects.size())
    {
        return;
    }

    setBatchSize(std::min(batch.objects.size(), maxBatchSize()));
    std::sort(batch.objects.begin(), batch.objects.end(), [](auto lhs, auto rhs) {
        return lhs->location().k() < rhs->location().k();
    });

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

            auto vertex = m_surface->vertex(j);

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
}

void MCSurfaceObjectRendererLegacy::setAttributePointers()
{
    glVertexAttribPointer(MCGLShaderProgram::VAL_Vertex, 3, GL_FLOAT, GL_FALSE,
                          sizeof(MCGLVertex), reinterpret_cast<GLvoid *>(m_vertices.data()));
    glVertexAttribPointer(MCGLShaderProgram::VAL_Normal, 3, GL_FLOAT, GL_FALSE,
                          sizeof(MCGLVertex), reinterpret_cast<GLvoid *>(m_normals.data()));
    glVertexAttribPointer(MCGLShaderProgram::VAL_TexCoords, 2, GL_FLOAT, GL_FALSE,
                          sizeof(MCGLTexCoord), reinterpret_cast<GLvoid *>(m_texCoords.data()));
    glVertexAttribPointer(MCGLShaderProgram::VAL_Color, 4, GL_FLOAT, GL_FALSE,
                          sizeof(MCGLColor), reinterpret_cast<GLvoid *>(m_colors.data()));
}

void MCSurfaceObjectRendererLegacy::render()
{
    assert(shaderProgram());

    shaderProgram()->bind();
    shaderProgram()->bindMaterial(material());

    shaderProgram()->setTransform(0, MCVector3dF(0, 0, 1));
    shaderProgram()->setScale(1.0f, 1.0f, 1.0f);
    shaderProgram()->setColor(m_surface->color());

    // Be sure active VBO is disabled because we are using client-side arrays here for dynamic data
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    enableAttributePointers();
    setAttributePointers();

    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(batchSize() * m_numVerticesPerSurface));
}

void MCSurfaceObjectRendererLegacy::renderShadows()
{
    assert(shadowShaderProgram());

    shadowShaderProgram()->bind();
    shadowShaderProgram()->bindMaterial(material());

    shadowShaderProgram()->setTransform(0, MCVector3dF(0, 0, 0));
    shadowShaderProgram()->setScale(1.0f, 1.0f, 1.0f);

    // Be sure active VBO is disabled because we are using client-side arrays here for dynamic data
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    enableAttributePointers();
    setAttributePointers();

    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(batchSize() * m_numVerticesPerSurface));
}
