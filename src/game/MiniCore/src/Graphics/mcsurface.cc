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
#include "mccamera.hh"
#include "mcbbox.hh"
#include "mcglmaterial.hh"
#include "mcglshaderprogram.hh"
#include "mcglvertex.hh"
#include "mcgltexcoord.hh"
#include "mctrigonom.hh"
#include "mcvector3d.hh"

#include <algorithm>
#include <cassert>

static const int NUM_VERTICES         = 6;
static const int NUM_COLOR_COMPONENTS = 4;

static const int VERTEX_DATA_SIZE     = sizeof(MCGLVertex)   * NUM_VERTICES;
static const int NORMAL_DATA_SIZE     = sizeof(MCGLVertex)   * NUM_VERTICES;
static const int TEXCOORD_DATA_SIZE   = sizeof(MCGLTexCoord) * NUM_VERTICES;
static const int COLOR_DATA_SIZE      = sizeof(GLfloat)      * NUM_VERTICES * NUM_COLOR_COMPONENTS;
static const int TOTAL_DATA_SIZE      =
    VERTEX_DATA_SIZE + NORMAL_DATA_SIZE + TEXCOORD_DATA_SIZE + COLOR_DATA_SIZE;

MCSurface::MCSurface(
    std::string handle,
    MCGLMaterialPtr material,
    MCFloat width,
    MCFloat height,
    MCFloat z0,
    MCFloat z1,
    MCFloat z2,
    MCFloat z3)
{
    init(handle, material, width, height);

    m_minZ = std::min(std::min(z0, z1), std::min(z2, z3));
    m_maxZ = std::max(std::max(z0, z1), std::max(z2, z3));

    // Init vertice data for two triangles.
    VertexVector vertices = {
        MCGLVertex(-(GLfloat)m_w2, -(GLfloat)m_h2, z0),
        MCGLVertex( (GLfloat)m_w2,  (GLfloat)m_h2, z2),
        MCGLVertex(-(GLfloat)m_w2,  (GLfloat)m_h2, z1),
        MCGLVertex(-(GLfloat)m_w2, -(GLfloat)m_h2, z0),
        MCGLVertex( (GLfloat)m_w2, -(GLfloat)m_h2, z3),
        MCGLVertex( (GLfloat)m_w2,  (GLfloat)m_h2, z2)
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

    setNormals({
        {n0.i(), n0.j(), n0.k()},
        {n1.i(), n1.j(), n1.k()},
        {n2.i(), n2.j(), n2.k()},
        {n3.i(), n3.j(), n3.k()},
        {n4.i(), n4.j(), n4.k()},
        {n5.i(), n5.j(), n5.k()}
    });

    setTexCoords({
        {0, 0},
        {1, 1},
        {0, 1},
        {0, 0},
        {1, 0},
        {1, 1}
    });

    setColors(ColorVector(NUM_VERTICES, MCGLColor()));

    initVBOs();
}

MCSurface::MCSurface(std::string handle, MCGLMaterialPtr material, MCFloat width, MCFloat height, MCFloat z)
    : MCSurface(handle, material, width, height, z, z, z, z)
{}

MCSurface::MCSurface(
    std::string handle, MCGLMaterialPtr material, MCFloat width, MCFloat height, const MCGLTexCoord texCoords[4])
{
    init(handle, material, width, height);

    // Init vertice data for two triangles.
    setVertices({
        {-(GLfloat)m_w2, -(GLfloat)m_h2, 0},
        { (GLfloat)m_w2,  (GLfloat)m_h2, 0},
        {-(GLfloat)m_w2,  (GLfloat)m_h2, 0},
        {-(GLfloat)m_w2, -(GLfloat)m_h2, 0},
        { (GLfloat)m_w2, -(GLfloat)m_h2, 0},
        { (GLfloat)m_w2,  (GLfloat)m_h2, 0}
    });

    setTexCoords({
        texCoords[0],
        texCoords[2],
        texCoords[1],
        texCoords[0],
        texCoords[3],
        texCoords[2]
    });

    setNormals(VertexVector(NUM_VERTICES, {0, 0, 1}));

    setColors(ColorVector(NUM_VERTICES, MCGLColor()));

    initVBOs();
}

const std::string & MCSurface::handle() const
{
    return m_handle;
}

void MCSurface::init(std::string handle, MCGLMaterialPtr material, MCFloat width, MCFloat height)
{
    setMaterial(material);

    m_handle         = handle;
    m_w              = width;
    m_w2             = width / 2;
    m_h              = height;
    m_h2             = height / 2;
    m_minZ           = 0;
    m_maxZ           = 0;
    m_useAlphaBlend  = false;
    m_src            = GL_SRC_ALPHA;
    m_dst            = GL_ONE_MINUS_SRC_ALPHA;
    m_color          = MCGLColor(1.0, 1.0, 1.0, 1.0);
    m_sx             = 1.0;
    m_sy             = 1.0;
    m_sz             = 1.0;
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

void MCSurface::setAlphaBlend(bool useAlphaBlend, GLenum src, GLenum dst)
{
    m_useAlphaBlend = useAlphaBlend;
    m_src           = src;
    m_dst           = dst;
}

void MCSurface::doAlphaBlend()
{
    if (m_useAlphaBlend)
    {
        glEnable(GL_BLEND);
        glBlendFunc(m_src, m_dst);
    }
}

void MCSurface::updateTexCoords(const MCGLTexCoord texCoords[4])
{
    bindVBO();

    const MCGLTexCoord texCoordsAll[NUM_VERTICES] =
    {
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

void MCSurface::setColor(const MCGLColor & color)
{
    m_color = color;
}

void MCSurface::setScale(MCFloat x, MCFloat y, MCFloat z)
{
    m_sx = x;
    m_sy = y;
    m_sz = z;
}

void MCSurface::setSize(MCFloat w, MCFloat h)
{
    m_sx = w / m_w;
    m_sy = h / m_h;
}

void MCSurface::bind()
{
    MCGLObjectBase::bind();

    doAlphaBlend();
}

void MCSurface::bindShadow()
{
    MCGLObjectBase::bindShadow();
}

void MCSurface::release()
{
    MCGLObjectBase::release();

    if (m_useAlphaBlend)
    {
        glDisable(GL_BLEND);
    }
}

void MCSurface::releaseShadow()
{
}

void MCSurface::render()
{
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES);
}

void MCSurface::render(MCCamera * camera, MCVector3dFR pos, MCFloat angle, bool autoBind)
{
    MCFloat x = pos.i();
    MCFloat y = pos.j();
    MCFloat z = pos.k();

    if (camera)
    {
        camera->mapToCamera(x, y);
    }

    if (autoBind)
    {
        bind();

        shaderProgram()->setScale(m_sx, m_sy, m_sz);
        shaderProgram()->setColor(m_color);
        shaderProgram()->setTransform(angle, MCVector3dF(x, y, z));

        render();

        release();
    }
    else
    {
        shaderProgram()->setScale(m_sx, m_sy, m_sz);
        shaderProgram()->setColor(m_color);
        shaderProgram()->setTransform(angle, MCVector3dF(x, y, z));

        render();
    }
}

void MCSurface::renderShadow(MCCamera * camera, MCVector3dFR pos, MCFloat angle, bool autoBind)
{
    MCFloat x = pos.i();
    MCFloat y = pos.j();

    if (camera)
    {
        camera->mapToCamera(x, y);
    }

    if (autoBind)
    {
        bindShadow();

        shadowShaderProgram()->setScale(m_sx, m_sy, m_sz);
        shadowShaderProgram()->setTransform(angle, MCVector3dF(x, y, pos.k()));

        render();

        releaseShadow();
    }
    else
    {
        shadowShaderProgram()->setScale(m_sx, m_sy, m_sz);
        shadowShaderProgram()->setTransform(angle, MCVector3dF(x, y, pos.k()));

        render();
    }
}

MCFloat MCSurface::width() const
{
    return m_w;
}

MCFloat MCSurface::height() const
{
    return m_h;
}

MCFloat MCSurface::minZ() const
{
    return m_minZ;
}

MCFloat MCSurface::maxZ() const
{
    return m_maxZ;
}
