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

#include "mcmesh.hh"

#include "mccamera.hh"
#include "mcbbox.hh"
#include "mcglshaderprogram.hh"
#include "mcglvertex.hh"
#include "mcgltexcoord.hh"
#include "mctrigonom.hh"
#include "mcassetmanager.hh"

#include <algorithm>

static const int NUM_COLOR_COMPONENTS = 4;

MCMesh::MCMesh(std::string handle, const FaceVector & faces, MCGLMaterialPtr material)
    : MCGLObjectBase(handle)
{
    setWidth(1.0f);

    setHeight(1.0f);

    init(faces);

    setMaterial(material);
}

void MCMesh::init(const FaceVector & faces)
{
    const int NUM_FACES = static_cast<int>(faces.size());

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::min();

    int vertexIndex = 0;
    for (int faceIndex = 0; faceIndex < NUM_FACES; faceIndex++)
    {
        const MCMesh::Face & face = faces.at(faceIndex);
        const int numFaceVertices = static_cast<int>(face.vertices.size());
        assert(numFaceVertices == 3); // Only triagles accepted

        for (int faceVertexIndex = 0; faceVertexIndex < numFaceVertices; faceVertexIndex++)
        {
            MCGLVertex vertex = {face.vertices.at(faceVertexIndex).x, face.vertices.at(faceVertexIndex).y, face.vertices.at(faceVertexIndex).z};

            addVertex(vertex);

            addNormal({face.vertices.at(faceVertexIndex).i, face.vertices.at(faceVertexIndex).j, face.vertices.at(faceVertexIndex).k});

            addTexCoord({face.vertices.at(faceVertexIndex).u, face.vertices.at(faceVertexIndex).v});

            if (!vertexIndex)
            {
                minX = vertex.x();
                maxX = vertex.x();
                minY = vertex.y();
                maxY = vertex.y();
            }
            else
            {
                minX = std::min(minX, vertex.x());
                maxX = std::max(maxX, vertex.x());
                minY = std::min(minY, vertex.y());
                maxY = std::max(maxY, vertex.y());
                minZ = std::min(minZ, vertex.z());
                maxZ = std::max(maxZ, vertex.z());
            }

            vertexIndex++;
        }
    }

    setWidth(maxX - minX);

    setHeight(maxY - minY);

    setMinZ(minZ);

    setMaxZ(maxZ);

    setColors(ColorVector(vertexCount(), color()));

    initVBOs();
}

void MCMesh::initVBOs()
{
    static const int VERTEX_DATA_SIZE = sizeof(MCGLVertex) * vertexCount();

    static const int NORMAL_DATA_SIZE = sizeof(MCGLVertex) * vertexCount();

    static const int TEXCOORD_DATA_SIZE = sizeof(MCGLTexCoord) * vertexCount();

    static const int COLOR_DATA_SIZE = sizeof(GLfloat) * vertexCount() * NUM_COLOR_COMPONENTS;

    static const int TOTAL_DATA_SIZE =
        VERTEX_DATA_SIZE + NORMAL_DATA_SIZE + TEXCOORD_DATA_SIZE + COLOR_DATA_SIZE;

    initBufferData(TOTAL_DATA_SIZE, GL_STATIC_DRAW);

    addBufferSubData(
        MCGLShaderProgram::VAL_Vertex, VERTEX_DATA_SIZE, verticesAsGlArray());
    addBufferSubData(
        MCGLShaderProgram::VAL_Normal, NORMAL_DATA_SIZE, normalsAsGlArray());
    addBufferSubData(
        MCGLShaderProgram::VAL_TexCoords, TEXCOORD_DATA_SIZE, texCoordsAsGlArray());
    addBufferSubData(
        MCGLShaderProgram::VAL_Color, COLOR_DATA_SIZE, colorsAsGlArray());

    finishBufferData();
}

void MCMesh::render(MCCamera * camera, MCVector3dFR pos, float angle)
{
    bind();

    float x = pos.i();
    float y = pos.j();
    float z = pos.k();

    if (camera)
    {
        camera->mapToCamera(x, y);
    }

    shaderProgram()->bind();
    shaderProgram()->setScale(scale().i(), scale().j(), scale().k());
    shaderProgram()->setColor(color());
    shaderProgram()->setTransform(angle, MCVector3dF(x, y, z));

    MCGLObjectBase::render();

    release();
}

void MCMesh::renderShadow(MCCamera * camera, MCVector3dFR pos, float angle)
{
    float x = pos.i();
    float y = pos.j();

    if (camera)
    {
        camera->mapToCamera(x, y);
    }

    bindShadow();

    shadowShaderProgram()->setScale(scale().i(), scale().j(), scale().k());
    shadowShaderProgram()->setTransform(angle, MCVector3dF(x, y, pos.k()));

    MCGLObjectBase::render();

    releaseShadow();
}
