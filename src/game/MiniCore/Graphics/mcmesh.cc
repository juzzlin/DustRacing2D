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

MCMesh::MCMesh(const FaceVector & faces, MCGLMaterialPtr material)
: m_w(1.0)
, m_h(1.0)
, m_minZ(0)
, m_maxZ(0)
, m_color(1.0, 1.0, 1.0, 1.0)
, m_sx(1.0)
, m_sy(1.0)
, m_sz(1.0)
{
    init(faces);

    setMaterial(material);
}

void MCMesh::init(const FaceVector & faces)
{
    const int NUM_FACES = static_cast<int>(faces.size());
    m_numVertices = NUM_FACES * 3; // Only triagles accepted

    MCGLVertex   * vertices  = new MCGLVertex[m_numVertices];
    MCGLVertex   * normals   = new MCGLVertex[m_numVertices];
    MCGLTexCoord * texCoords = new MCGLTexCoord[m_numVertices];

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
            vertices[vertexIndex].setX(face.vertices.at(faceVertexIndex).x);
            vertices[vertexIndex].setY(face.vertices.at(faceVertexIndex).y);
            vertices[vertexIndex].setZ(face.vertices.at(faceVertexIndex).z);

            normals[vertexIndex].setX(face.vertices.at(faceVertexIndex).i);
            normals[vertexIndex].setY(face.vertices.at(faceVertexIndex).j);
            normals[vertexIndex].setZ(face.vertices.at(faceVertexIndex).k);

            texCoords[vertexIndex].u = face.vertices.at(faceVertexIndex).u;
            texCoords[vertexIndex].v = face.vertices.at(faceVertexIndex).v;

            if (!vertexIndex)
            {
                minX = vertices[vertexIndex].x();
                maxX = vertices[vertexIndex].x();
                minY = vertices[vertexIndex].y();
                maxY = vertices[vertexIndex].y();
            }
            else
            {
                minX = std::min(minX, vertices[vertexIndex].x());
                maxX = std::max(maxX, vertices[vertexIndex].x());
                maxZ = std::max(maxZ, vertices[vertexIndex].z());
                minY = std::min(minY, vertices[vertexIndex].y());
                maxY = std::max(maxY, vertices[vertexIndex].y());
                maxZ = std::max(maxZ, vertices[vertexIndex].z());
            }

            vertexIndex++;
        }
    }

    m_w = maxX - minX;
    m_h = maxY - minY;

    m_minZ = minZ;
    m_maxZ = maxZ;

    GLfloat * colors = new GLfloat[m_numVertices * NUM_COLOR_COMPONENTS];
    for (int colorIndex = 0; colorIndex < m_numVertices * NUM_COLOR_COMPONENTS; colorIndex++)
    {
        colors[colorIndex] = 1.0f;
    }

    initVBOs(vertices, normals, texCoords, colors);

    delete [] vertices;
    delete [] normals;
    delete [] texCoords;
    delete [] colors;
}

void MCMesh::initVBOs(
    const MCGLVertex   * vertices,
    const MCGLVertex   * normals,
    const MCGLTexCoord * texCoords,
    const GLfloat      * colors)
{
    static const int VERTEX_DATA_SIZE   = sizeof(MCGLVertex)   * m_numVertices;
    static const int NORMAL_DATA_SIZE   = sizeof(MCGLVertex)   * m_numVertices;
    static const int TEXCOORD_DATA_SIZE = sizeof(MCGLTexCoord) * m_numVertices;
    static const int COLOR_DATA_SIZE    = sizeof(GLfloat)      * m_numVertices * NUM_COLOR_COMPONENTS;
    static const int TOTAL_DATA_SIZE    =
        VERTEX_DATA_SIZE + NORMAL_DATA_SIZE + TEXCOORD_DATA_SIZE + COLOR_DATA_SIZE;

    initBufferData(TOTAL_DATA_SIZE, GL_STATIC_DRAW);

    addBufferSubData(
        MCGLShaderProgram::VAL_Vertex, VERTEX_DATA_SIZE, reinterpret_cast<const GLfloat *>(vertices));
    addBufferSubData(
        MCGLShaderProgram::VAL_Normal, NORMAL_DATA_SIZE, reinterpret_cast<const GLfloat *>(normals));
    addBufferSubData(
        MCGLShaderProgram::VAL_TexCoords, TEXCOORD_DATA_SIZE, reinterpret_cast<const GLfloat *>(texCoords));
    addBufferSubData(
        MCGLShaderProgram::VAL_Color, COLOR_DATA_SIZE, reinterpret_cast<const GLfloat *>(colors));

    finishBufferData();
}

void MCMesh::render()
{
    glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
}

void MCMesh::setColor(const MCGLColor & color)
{
    m_color = color;
}

void MCMesh::setScale(MCFloat x, MCFloat y, MCFloat z)
{
    m_sx = x;
    m_sy = y;
    m_sz = z;
}

void MCMesh::setScale(MCFloat w, MCFloat h)
{
    m_sx = w / m_w;
    m_sy = h / m_h;
}

void MCMesh::render(MCCamera * camera, MCVector3dFR pos, MCFloat angle, bool autoBind)
{
    if (autoBind)
    {
        bind();
    }

    MCFloat x = pos.i();
    MCFloat y = pos.j();
    MCFloat z = pos.k();

    if (camera)
    {
        camera->mapToCamera(x, y);
    }

    shaderProgram()->bind();
    shaderProgram()->setScale(m_sx, m_sy, m_sz);
    shaderProgram()->setColor(m_color);
    shaderProgram()->setTransform(angle, MCVector3dF(x, y, z));

    render();

    if (autoBind)
    {
        release();
    }
}

void MCMesh::renderShadow(MCCamera * camera, MCVector2dFR pos, MCFloat angle, bool autoBind)
{
    if (autoBind)
    {
        bindShadow();
    }

    MCFloat x = pos.i();
    MCFloat y = pos.j();
    MCFloat z = 0;

    if (camera)
    {
        camera->mapToCamera(x, y);
    }

    shadowShaderProgram()->bind();
    shadowShaderProgram()->setScale(m_sx, m_sy, m_sz);
    shadowShaderProgram()->setTransform(angle, MCVector3dF(x, y, z));

    render();

    if (autoBind)
    {
        releaseShadow();
    }
}

MCFloat MCMesh::width() const
{
    return m_w;
}

MCFloat MCMesh::height() const
{
    return m_h;
}

MCFloat MCMesh::minZ() const
{
    return m_minZ;
}

MCFloat MCMesh::maxZ() const
{
    return m_maxZ;
}
