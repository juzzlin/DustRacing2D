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

#include "mcassetmanager.hh"
#include "mcbbox.hh"
#include "mccamera.hh"
#include "mcglshaderprogram.hh"
#include "mcgltexcoord.hh"
#include "mcglvertex.hh"
#include "mctrigonom.hh"

#include <algorithm>

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
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::min();

    size_t vertexIndex = 0;
    for (auto && face : faces)
    {
        if (face.vertices.size() != 3)
        {
            throw std::runtime_error("Only triangular faces supported!");
        }

        for (size_t faceVertexIndex = 0; faceVertexIndex < face.vertices.size(); faceVertexIndex++)
        {
            MCGLVertex vertex = { face.vertices.at(faceVertexIndex).x, face.vertices.at(faceVertexIndex).y, face.vertices.at(faceVertexIndex).z };

            addVertex(vertex);

            addNormal({ face.vertices.at(faceVertexIndex).i, face.vertices.at(faceVertexIndex).j, face.vertices.at(faceVertexIndex).k });

            addTexCoord({ face.vertices.at(faceVertexIndex).u, face.vertices.at(faceVertexIndex).v });

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
    static const auto vertexDataSize = sizeof(MCGLVertex) * vertexCount();

    static const auto normalDataSize = sizeof(MCGLVertex) * vertexCount();

    static const auto texCoordDataSize = sizeof(MCGLTexCoord) * vertexCount();

    static const auto numColorComponents = 4;

    static const auto colorDataSize = sizeof(GLfloat) * vertexCount() * numColorComponents;

    static const auto totalDataSize = vertexDataSize + normalDataSize + texCoordDataSize + colorDataSize;

    initBufferData(totalDataSize, GL_STATIC_DRAW);

    addBufferSubData(
      MCGLShaderProgram::VAL_Vertex, vertexDataSize, verticesAsGlArray());
    addBufferSubData(
      MCGLShaderProgram::VAL_Normal, normalDataSize, normalsAsGlArray());
    addBufferSubData(
      MCGLShaderProgram::VAL_TexCoords, texCoordDataSize, texCoordsAsGlArray());
    addBufferSubData(
      MCGLShaderProgram::VAL_Color, colorDataSize, colorsAsGlArray());

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
