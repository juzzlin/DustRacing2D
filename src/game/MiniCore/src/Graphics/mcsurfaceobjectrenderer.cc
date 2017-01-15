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

#include <QDebug>

namespace {
const int NUM_VERTICES_PER_SURFACE = 6;
}

MCSurfaceObjectRenderer::MCSurfaceObjectRenderer(int maxBatchSize)
    : MCObjectRendererBase(maxBatchSize)
    , m_vertices(new MCGLVertex[maxBatchSize * NUM_VERTICES_PER_SURFACE])
    , m_normals(new MCGLVertex[maxBatchSize * NUM_VERTICES_PER_SURFACE])
    , m_texCoords(new MCGLTexCoord[maxBatchSize * NUM_VERTICES_PER_SURFACE])
    , m_colors(new MCGLColor[maxBatchSize * NUM_VERTICES_PER_SURFACE])
{
    const int NUM_VERTICES = maxBatchSize * NUM_VERTICES_PER_SURFACE;
    const int VERTEX_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;
    const int NORMAL_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;
    const int TEXCOORD_DATA_SIZE = sizeof(MCGLTexCoord) * NUM_VERTICES;
    const int COLOR_DATA_SIZE = sizeof(MCGLColor) * NUM_VERTICES;
    const int TOTAL_DATA_SIZE = VERTEX_DATA_SIZE + NORMAL_DATA_SIZE + TEXCOORD_DATA_SIZE + COLOR_DATA_SIZE;

    initBufferData(TOTAL_DATA_SIZE, GL_DYNAMIC_DRAW);

    addBufferSubData(
        MCGLShaderProgram::VAL_Vertex, VERTEX_DATA_SIZE, reinterpret_cast<const GLfloat *>(m_vertices));
    addBufferSubData(
        MCGLShaderProgram::VAL_Normal, NORMAL_DATA_SIZE, reinterpret_cast<const GLfloat *>(m_normals));
    addBufferSubData(
        MCGLShaderProgram::VAL_TexCoords, TEXCOORD_DATA_SIZE, reinterpret_cast<const GLfloat *>(m_texCoords));
    addBufferSubData(
        MCGLShaderProgram::VAL_Color, COLOR_DATA_SIZE, reinterpret_cast<const GLfloat *>(m_colors));

    finishBufferData();
}

void MCSurfaceObjectRenderer::setBatch(MCObjectRendererBase::ObjectVector & objects, MCCamera * camera, bool isShadow)
{
    if (!objects.size()) {
        return;
    }

    setBatchSize(std::min(static_cast<int>(objects.size()), maxBatchSize()));
    std::sort(objects.begin(), objects.end(), [] (const MCObject * l, const MCObject * r) {
        return l->location().k() < r->location().k();
    });

    const int NUM_VERTICES = batchSize() * NUM_VERTICES_PER_SURFACE;
    const int VERTEX_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;
    const int NORMAL_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;
    const int TEXCOORD_DATA_SIZE = sizeof(MCGLTexCoord) * NUM_VERTICES;
    const int COLOR_DATA_SIZE  = sizeof(MCGLColor) * NUM_VERTICES;

    // Take common properties from the first Object in the batch
    MCObject * object = objects.at(0);
    MCSurfaceView * view = dynamic_cast<MCSurfaceView *>(object->shape()->view().get());
    assert(view);

    MCSurface * surface = view->surface();

    setShaderProgram(surface->shaderProgram());
    setShadowShaderProgram(surface->shadowShaderProgram());

    setMaterial(surface->material());
    setHasShadow(view->hasShadow());

    int vertexIndex = 0;
    for (int i = 0; i < batchSize(); i++)
    {
        object = objects[i];
        MCVector3dF location(object->shape()->location());

        MCFloat x, y, z;
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

        if (!isShadow)
        {
            for (int j = 0; j < NUM_VERTICES_PER_SURFACE; j++)
            {
                m_colors[vertexIndex] = surface->color(j);

                auto vertex = surface->vertex(j);
                m_vertices[vertexIndex] =
                        MCGLVertex(
                            x + MCMathUtil::rotatedX(vertex.x(), vertex.y(), object->angle()),
                            y + MCMathUtil::rotatedY(vertex.x(), vertex.y(), object->angle()),
                            z + vertex.z());

                m_normals[vertexIndex] = surface->normal(j);

                m_texCoords[vertexIndex] = surface->texCoord(j);

                vertexIndex++;
            }
        }
        else
        {
            for (int j = 0; j < NUM_VERTICES_PER_SURFACE; j++)
            {
                m_colors[vertexIndex] = surface->color(j);

                auto vertex = surface->vertex(j);
                m_vertices[vertexIndex] =
                        MCGLVertex(
                            x + MCMathUtil::rotatedX(vertex.x(), vertex.y(), object->angle()),
                            y + MCMathUtil::rotatedY(vertex.x(), vertex.y(), object->angle()),
                            z);

                m_normals[vertexIndex] = surface->normal(j);

                m_texCoords[vertexIndex] = surface->texCoord(j);

                vertexIndex++;
            }
        }
    }

    initUpdateBufferData();

    const int MAX_VERTEX_DATA_SIZE = sizeof(MCGLVertex) * maxBatchSize() * NUM_VERTICES_PER_SURFACE;
    addBufferSubData(
        MCGLShaderProgram::VAL_Vertex, VERTEX_DATA_SIZE, MAX_VERTEX_DATA_SIZE,
        reinterpret_cast<const GLfloat *>(m_vertices));

    const int MAX_NORMAL_DATA_SIZE = sizeof(MCGLVertex) * maxBatchSize() * NUM_VERTICES_PER_SURFACE;
    addBufferSubData(
        MCGLShaderProgram::VAL_Normal, NORMAL_DATA_SIZE, MAX_NORMAL_DATA_SIZE,
        reinterpret_cast<const GLfloat *>(m_normals));

    const int MAX_TEXCOORD_DATA_SIZE = sizeof(MCGLTexCoord) * maxBatchSize() * NUM_VERTICES_PER_SURFACE;
    addBufferSubData(
        MCGLShaderProgram::VAL_TexCoords, TEXCOORD_DATA_SIZE, MAX_TEXCOORD_DATA_SIZE,
        reinterpret_cast<const GLfloat *>(m_texCoords));

    addBufferSubData(
        MCGLShaderProgram::VAL_Color, COLOR_DATA_SIZE, reinterpret_cast<const GLfloat *>(m_colors));
}

void MCSurfaceObjectRenderer::render()
{
    assert(shaderProgram());

    if (useAlphaBlend())
    {
        glEnable(GL_BLEND);
        glBlendFunc(alphaSrc(), alphaDst());
    }

    shaderProgram()->bind();
    shaderProgram()->bindMaterial(material());

    shaderProgram()->setTransform(0, MCVector3dF(0, 0, 1));
    shaderProgram()->setScale(1.0f, 1.0f, 1.0f);
    shaderProgram()->setColor(MCGLColor(1.0f, 1.0f, 1.0f, 1.0f));

    glDrawArrays(GL_TRIANGLES, 0, batchSize() * NUM_VERTICES_PER_SURFACE);
    glDisable(GL_BLEND);

    releaseVBO();
    releaseVAO();
}

void MCSurfaceObjectRenderer::renderShadows()
{
    assert(shadowShaderProgram());

    shadowShaderProgram()->bind();
    shadowShaderProgram()->bindMaterial(material());

    shadowShaderProgram()->setTransform(0, MCVector3dF(0, 0, 0));
    shadowShaderProgram()->setScale(1.0f, 1.0f, 1.0f);

    glDrawArrays(GL_TRIANGLES, 0, batchSize() * NUM_VERTICES_PER_SURFACE);

    releaseVBO();
    releaseVAO();
}

MCSurfaceObjectRenderer::~MCSurfaceObjectRenderer()
{
    delete [] m_vertices;
    delete [] m_normals;
    delete [] m_texCoords;
    delete [] m_colors;
}

