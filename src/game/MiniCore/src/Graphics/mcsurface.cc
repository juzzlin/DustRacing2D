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

#include "mcsurface.hh"
#include "mcbbox.hh"
#include "mccamera.hh"
#include "mcglmaterial.hh"
#include "mcglshaderprogram.hh"
#include "mcgltexcoord.hh"
#include "mcglvertex.hh"
#include "mctrigonom.hh"
#include "mcvector3d.hh"

#include <algorithm>
#include <cassert>

static const int NUM_VERTICES = 6;

static const int NUM_COLOR_COMPONENTS = 4;

static const int VERTEX_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;

static const int NORMAL_DATA_SIZE = sizeof(MCGLVertex) * NUM_VERTICES;

static const int TEXCOORD_DATA_SIZE = sizeof(MCGLTexCoord) * NUM_VERTICES;

static const int COLOR_DATA_SIZE = sizeof(GLfloat) * NUM_VERTICES * NUM_COLOR_COMPONENTS;

static const int TOTAL_DATA_SIZE =
  VERTEX_DATA_SIZE + NORMAL_DATA_SIZE + TEXCOORD_DATA_SIZE + COLOR_DATA_SIZE;

MCSurface::MCSurface(
  std::string handle,
  MCGLMaterialPtr material,
  float width,
  float height,
  float z0,
  float z1,
  float z2,
  float z3)
  : MCGLObjectBase(handle)
{
    setMaterial(material);

    setWidth(width);

    setHeight(height);

    setMinZ(std::min(std::min(z0, z1), std::min(z2, z3)));

    setMaxZ(std::max(std::max(z0, z1), std::max(z2, z3)));

    // Init vertice data for two triangles.
    const float w2 = this->width() / 2;
    const float h2 = this->height() / 2;
    VertexVector vertices = {
        MCGLVertex(-(GLfloat)w2, -(GLfloat)h2, z0),
        MCGLVertex((GLfloat)w2, (GLfloat)h2, z2),
        MCGLVertex(-(GLfloat)w2, (GLfloat)h2, z1),
        MCGLVertex(-(GLfloat)w2, -(GLfloat)h2, z0),
        MCGLVertex((GLfloat)w2, -(GLfloat)h2, z3),
        MCGLVertex((GLfloat)w2, (GLfloat)h2, z2)
    };

    setVertices(vertices);

    // Calculate normals

    const MCVector3dF v0(vertices[0].x(), vertices[0].y(), vertices[0].z());
    const MCVector3dF v1(vertices[1].x(), vertices[1].y(), vertices[1].z());
    const MCVector3dF v2(vertices[2].x(), vertices[2].y(), vertices[2].z());
    const MCVector3dF v3(vertices[3].x(), vertices[3].y(), vertices[3].z());
    const MCVector3dF v4(vertices[4].x(), vertices[4].y(), vertices[4].z());
    const MCVector3dF v5(vertices[5].x(), vertices[5].y(), vertices[5].z());

    const MCVector3dF n0(((v1 - v0) % (v2 - v0)).normalized());
    const MCVector3dF n1(n0);
    const MCVector3dF n2(n0);
    const MCVector3dF n3(((v4 - v3) % (v5 - v3)).normalized());
    const MCVector3dF n4(n3);
    const MCVector3dF n5(n3);

    setNormals({ { n0.i(), n0.j(), n0.k() },
                 { n1.i(), n1.j(), n1.k() },
                 { n2.i(), n2.j(), n2.k() },
                 { n3.i(), n3.j(), n3.k() },
                 { n4.i(), n4.j(), n4.k() },
                 { n5.i(), n5.j(), n5.k() } });

    setTexCoords({ { 0, 0 },
                   { 1, 1 },
                   { 0, 1 },
                   { 0, 0 },
                   { 1, 0 },
                   { 1, 1 } });

    setColors(ColorVector(NUM_VERTICES, MCGLColor()));

    initVBOs();
}

MCSurface::MCSurface(std::string handle, MCGLMaterialPtr material, float width, float height, float z)
  : MCSurface(handle, material, width, height, z, z, z, z)
{
}

MCSurface::MCSurface(
  std::string handle, MCGLMaterialPtr material, float width, float height, const MCGLTexCoord texCoords[4])
  : MCGLObjectBase(handle)
{
    setMaterial(material);

    setWidth(width);

    setHeight(height);

    // Init vertice data for two triangles.
    const float w2 = width / 2;
    const float h2 = height / 2;
    setVertices({ { -(GLfloat)w2, -(GLfloat)h2, 0 },
                  { (GLfloat)w2, (GLfloat)h2, 0 },
                  { -(GLfloat)w2, (GLfloat)h2, 0 },
                  { -(GLfloat)w2, -(GLfloat)h2, 0 },
                  { (GLfloat)w2, -(GLfloat)h2, 0 },
                  { (GLfloat)w2, (GLfloat)h2, 0 } });

    setTexCoords({ texCoords[0],
                   texCoords[2],
                   texCoords[1],
                   texCoords[0],
                   texCoords[3],
                   texCoords[2] });

    setNormals(VertexVector(NUM_VERTICES, { 0, 0, 1 }));

    setColors(ColorVector(NUM_VERTICES, MCGLColor()));

    initVBOs();
}

void MCSurface::initVBOs()
{
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

void MCSurface::updateTexCoords(const MCGLTexCoord texCoords[4])
{
    bindVBO();

    const MCGLTexCoord texCoordsAll[NUM_VERTICES] = {
        texCoords[0],
        texCoords[2],
        texCoords[1],
        texCoords[0],
        texCoords[3],
        texCoords[2]
    };

    glBufferSubData(
      GL_ARRAY_BUFFER, VERTEX_DATA_SIZE + NORMAL_DATA_SIZE, TEXCOORD_DATA_SIZE, texCoordsAll);
}
