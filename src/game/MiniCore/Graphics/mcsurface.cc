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
    MCGLMaterialPtr material, MCFloat width, MCFloat height,
    MCFloat z0, MCFloat z1, MCFloat z2, MCFloat z3)
{
    init(material, width, height);

    m_minZ = std::min(std::min(z0, z1), std::min(z2, z3));
    m_maxZ = std::max(std::max(z0, z1), std::max(z2, z3));

    // Init vertice data for two triangles.
    const MCGLVertex vertices[NUM_VERTICES] =
    {
        MCGLVertex(-(GLfloat)m_w2, -(GLfloat)m_h2, z0),
        MCGLVertex( (GLfloat)m_w2,  (GLfloat)m_h2, z2),
        MCGLVertex(-(GLfloat)m_w2,  (GLfloat)m_h2, z1),
        MCGLVertex(-(GLfloat)m_w2, -(GLfloat)m_h2, z0),
        MCGLVertex( (GLfloat)m_w2, -(GLfloat)m_h2, z3),
        MCGLVertex( (GLfloat)m_w2,  (GLfloat)m_h2, z2)
    };

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

    const MCGLVertex normals[NUM_VERTICES] =
    {
        {n0.i(), n0.j(), n0.k()},
        {n1.i(), n1.j(), n1.k()},
        {n2.i(), n2.j(), n2.k()},
        {n3.i(), n3.j(), n3.k()},
        {n4.i(), n4.j(), n4.k()},
        {n5.i(), n5.j(), n5.k()}
    };

    const MCGLTexCoord texCoords[NUM_VERTICES] =
    {
        {0, 0},
        {1, 1},
        {0, 1},
        {0, 0},
        {1, 0},
        {1, 1}
    };

    const GLfloat colors[COLOR_DATA_SIZE] =
    {
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1
    };

    initVBOs(vertices, normals, texCoords, colors);
}

MCSurface::MCSurface(MCGLMaterialPtr material, MCFloat width, MCFloat height, MCFloat z)
    : MCSurface(material, width, height, z, z, z, z)
{}

MCSurface::MCSurface(
    MCGLMaterialPtr material, MCFloat width, MCFloat height, const MCGLTexCoord texCoords[4])
{
    init(material, width, height);

    // Init vertice data for two triangles.
    const MCGLVertex vertices[NUM_VERTICES] =
    {
        {-(GLfloat)m_w2, -(GLfloat)m_h2, 0},
        { (GLfloat)m_w2,  (GLfloat)m_h2, 0},
        {-(GLfloat)m_w2,  (GLfloat)m_h2, 0},
        {-(GLfloat)m_w2, -(GLfloat)m_h2, 0},
        { (GLfloat)m_w2, -(GLfloat)m_h2, 0},
        { (GLfloat)m_w2,  (GLfloat)m_h2, 0}
    };

    const MCGLVertex normals[NUM_VERTICES] =
    {
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1},
        {0, 0, 1}
    };

    const MCGLTexCoord texCoordsAll[NUM_VERTICES] =
    {
        texCoords[0],
        texCoords[2],
        texCoords[1],
        texCoords[0],
        texCoords[3],
        texCoords[2]
    };

    const GLfloat colors[NUM_VERTICES * NUM_COLOR_COMPONENTS] =
    {
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1
    };

    initVBOs(vertices, normals, texCoordsAll, colors);
}

void MCSurface::init(MCGLMaterialPtr material, MCFloat width, MCFloat height)
{
    setMaterial(material);

    m_w              = width;
    m_w2             = width / 2;
    m_h              = height;
    m_h2             = height / 2;
    m_minZ           = 0;
    m_maxZ           = 0;
    m_center         = MCVector2dF(m_w2, m_h2);
    m_centerSet      = false;
    m_useAlphaBlend  = false;
    m_src            = GL_SRC_ALPHA;
    m_dst            = GL_ONE_MINUS_SRC_ALPHA;
    m_color          = MCGLColor(1.0, 1.0, 1.0, 1.0);
    m_sx             = 1.0;
    m_sy             = 1.0;
    m_sz             = 1.0;
}

void MCSurface::initVBOs(
    const MCGLVertex   * vertices,
    const MCGLVertex   * normals,
    const MCGLTexCoord * texCoords,
    const GLfloat      * colors)
{
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

void MCSurface::setCenter(MCVector2dFR center)
{
    m_centerSet = true;
    m_center    = center;
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

void MCSurface::setTexCoords(const MCGLTexCoord texCoords[4])
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

    shaderProgram()->setScale(m_sx, m_sy, m_sz);
    shaderProgram()->setColor(m_color);

    if (m_centerSet)
    {
        shaderProgram()->setTransform(angle, MCVector3dF(x + m_w2 - m_center.i(), y + m_h2 - m_center.j(), z));
    }
    else
    {
        shaderProgram()->setTransform(angle, MCVector3dF(x, y, z));
    }

    render();

    if (autoBind)
    {
        release();
    }
}

void MCSurface::renderShadow(MCCamera * camera, MCVector3dFR pos, MCFloat angle, bool autoBind)
{
    if (autoBind)
    {
        bindShadow();
    }

    MCFloat x = pos.i();
    MCFloat y = pos.j();

    if (camera)
    {
        camera->mapToCamera(x, y);
    }

    shadowShaderProgram()->setScale(m_sx, m_sy, m_sz);

    const MCFloat z = pos.k();

    if (m_centerSet)
    {
        shadowShaderProgram()->setTransform(angle,
            MCVector3dF(x + m_w2 - m_center.i(), y + m_h2 - m_center.j(), z));
    }
    else
    {
        shadowShaderProgram()->setTransform(angle, MCVector3dF(x, y, z));
    }

    render();

    if (autoBind)
    {
        releaseShadow();
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

MCVector2dF MCSurface::center() const
{
    return m_center;
}
