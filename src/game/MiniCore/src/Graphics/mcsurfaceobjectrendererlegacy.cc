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

#include <QDebug>

namespace {
#ifdef __MC_GLES__
const int NUM_VERTICES_PER_SURFACE = 6;
#else
const int NUM_VERTICES_PER_SURFACE = 4;
#endif
}

MCSurfaceObjectRendererLegacy::MCSurfaceObjectRendererLegacy(int maxBatchSize)
    : MCObjectRendererBase(maxBatchSize)
    , m_vertices(new MCGLVertex[maxBatchSize * NUM_VERTICES_PER_SURFACE])
    , m_normals(new MCGLVertex[maxBatchSize * NUM_VERTICES_PER_SURFACE])
    , m_texCoords(new MCGLTexCoord[maxBatchSize * NUM_VERTICES_PER_SURFACE])
    , m_colors(new MCGLColor[maxBatchSize * NUM_VERTICES_PER_SURFACE])
{
}

void MCSurfaceObjectRendererLegacy::setBatch(MCObjectRendererBase::ObjectVector & objects, MCCamera * camera, bool isShadow)
{
    if (!objects.size()) {
        return;
    }

    setBatchSize(std::min(static_cast<int>(objects.size()), maxBatchSize()));
    std::sort(objects.begin(), objects.end(), [] (const MCObject * l, const MCObject * r) {
        return l->location().k() < r->location().k();
    });

    // Init vertice data for a quad

    static const MCGLVertex vertices[NUM_VERTICES_PER_SURFACE] =
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

    static const MCGLVertex normals[NUM_VERTICES_PER_SURFACE] =
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

    const MCGLTexCoord texCoords[NUM_VERTICES_PER_SURFACE] =
    {
    #ifdef __MC_GLES__
        {0, 0},
        {1, 1},
    #endif
        {0, 1},
        {0, 0},
        {1, 0},
        {1, 1}
    };

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

        const MCGLColor & color = object->shape()->view()->color();

        for (int j = 0; j < NUM_VERTICES_PER_SURFACE; j++)
        {
            MCFloat vertexX = vertices[j].x() * surface->width() / 2;
            MCFloat vertexY = vertices[j].y() * surface->height() / 2;

            m_colors[vertexIndex] = color;

            m_vertices[vertexIndex] =
                MCGLVertex(
                     x + MCMathUtil::rotatedX(vertexX, vertexY, object->angle()),
                     y + MCMathUtil::rotatedY(vertexX, vertexY, object->angle()),
                     z);

            m_normals[vertexIndex] = normals[j];

            m_texCoords[vertexIndex] = texCoords[j];

            vertexIndex++;
        }
    }
}

void MCSurfaceObjectRendererLegacy::setAttributePointers()
{
    glVertexAttribPointer(MCGLShaderProgram::VAL_Vertex, 3, GL_FLOAT, GL_FALSE,
        sizeof(MCGLVertex), reinterpret_cast<GLvoid *>(m_vertices));
    glVertexAttribPointer(MCGLShaderProgram::VAL_Normal, 3, GL_FLOAT, GL_FALSE,
        sizeof(MCGLVertex), reinterpret_cast<GLvoid *>(m_normals));
    glVertexAttribPointer(MCGLShaderProgram::VAL_TexCoords, 2, GL_FLOAT, GL_FALSE,
        sizeof(MCGLTexCoord), reinterpret_cast<GLvoid *>(m_texCoords));
    glVertexAttribPointer(MCGLShaderProgram::VAL_Color, 4, GL_FLOAT, GL_FALSE,
        sizeof(MCGLColor), reinterpret_cast<GLvoid *>(m_colors));
}

void MCSurfaceObjectRendererLegacy::render()
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

    // Be sure active VBO is disabled because we are using client-side arrays here for dynamic data
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    enableAttributePointers();
    setAttributePointers();

#ifdef __MC_GLES__
    glDrawArrays(GL_TRIANGLES, 0, batchSize() * NUM_VERTICES_PER_SURFACE);
#else
    glDrawArrays(GL_QUADS, 0, batchSize() * NUM_VERTICES_PER_SURFACE);
#endif
    glDisable(GL_BLEND);
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

#ifdef __MC_GLES__
    glDrawArrays(GL_TRIANGLES, 0, batchSize() * NUM_VERTICES_PER_SURFACE);
#else
    glDrawArrays(GL_QUADS, 0, batchSize() * NUM_VERTICES_PER_SURFACE);
#endif
}

MCSurfaceObjectRendererLegacy::~MCSurfaceObjectRendererLegacy()
{
    delete [] m_vertices;
    delete [] m_normals;
    delete [] m_texCoords;
    delete [] m_colors;
}

