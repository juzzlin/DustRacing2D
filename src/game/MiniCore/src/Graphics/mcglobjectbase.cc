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

#include "mcglobjectbase.hh"
#include "mcglscene.hh"
#include "mclogger.hh"

#include <cassert>
#include <exception>

GLuint MCGLObjectBase::m_boundVbo = 0;

MCGLObjectBase::MCGLObjectBase()
: m_vao(0)
, m_vbo(0)
, m_program(MCGLScene::instance().defaultShaderProgram())
, m_shadowProgram(MCGLScene::instance().defaultShadowShaderProgram())
, m_bufferDataOffset(0)
, m_vertexDataSize(0)
, m_normalDataSize(0)
, m_texCoordDataSize(0)
, m_colorDataSize(0)
, m_totalDataSize(0)
, m_hasVao(false)
{
#ifdef __MC_QOPENGLFUNCTIONS__
    initializeOpenGLFunctions();
#endif
}

void MCGLObjectBase::setShaderProgram(MCGLShaderProgramPtr program)
{
    m_program = program;
}

void MCGLObjectBase::setShadowShaderProgram(MCGLShaderProgramPtr program)
{
    m_shadowProgram = program;
}

MCGLShaderProgramPtr MCGLObjectBase::shaderProgram() const
{
    return m_program;
}

MCGLShaderProgramPtr MCGLObjectBase::shadowShaderProgram() const
{
    return m_shadowProgram;
}

// Notice that on desktop OpenGL 3.0+ VAOs are mandatory.
// Without VAOs this code will run only on GLES.
void MCGLObjectBase::bindVAO()
{
#ifdef __MC_QOPENGLFUNCTIONS__
    if (m_hasVao)
    {
        m_vao.bind();
    }
    else
    {
        setAttributePointers();
    }
#else
    glBindVertexArray(m_vao);
#endif
}

void MCGLObjectBase::releaseVAO()
{
#ifdef __MC_QOPENGLFUNCTIONS__
    if (m_hasVao)
    {
        m_vao.release();
    }
#endif
}

bool MCGLObjectBase::createVAO()
{
#ifdef __MC_QOPENGLFUNCTIONS__
    m_hasVao = m_vao.create();
    return m_hasVao;
#else
    if (m_vao == 0)
    {
        glGenVertexArrays(1, &m_vao);
    }
    return m_vao;
#endif
}

void MCGLObjectBase::bindVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    MCGLObjectBase::m_boundVbo = m_vbo;
}

void MCGLObjectBase::releaseVBO()
{
    MCGLObjectBase::m_boundVbo = 0;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MCGLObjectBase::createVBO()
{
    if (m_vbo == 0)
    {
        glGenBuffers(1, &m_vbo);
    }
}

void MCGLObjectBase::bind()
{
    if (shaderProgram())
    {
        shaderProgram()->bind();

        if (m_material)
        {
            shaderProgram()->bindMaterial(m_material);
        }

        bindVBO();
        bindVAO();
    }
    else
    {
        // Save the user from debugging as to why nothing is being drawn.
        throw std::runtime_error("Trying to bind MCGLObject but shader program for it not set!");
    }
}

void MCGLObjectBase::bindShadow()
{
    if (shadowShaderProgram())
    {
        shadowShaderProgram()->bind();

        if (m_material)
        {
            shadowShaderProgram()->bindMaterial(m_material);
        }

        bindVBO();
        bindVAO();
    }
    else
    {
        // Save the user from debugging as to why nothing is being drawn.
        throw std::runtime_error("Trying to render shadow for surface, but shader program for it not set!");
    }
}

void MCGLObjectBase::release()
{
    releaseVAO();
}

void MCGLObjectBase::releaseShadow()
{
    releaseVAO();
}

void MCGLObjectBase::setMaterial(MCGLMaterialPtr material)
{
    m_material = material;
}

MCGLMaterialPtr MCGLObjectBase::material() const
{
    return m_material;
}

void MCGLObjectBase::initBufferData(int totalDataSize, GLuint drawType)
{
    m_totalDataSize = totalDataSize;

    createVAO();
    createVBO();

    bindVAO();
    bindVBO();

    glBufferData(GL_ARRAY_BUFFER, m_totalDataSize, nullptr, drawType);

    m_bufferDataOffset = 0;
}

void MCGLObjectBase::addVertex(const MCGLVertex & vertex)
{
    m_vertices.push_back(vertex);
}

void MCGLObjectBase::setVertices(const VertexVector & vertices)
{
    m_vertices = vertices;
}

const MCGLVertex & MCGLObjectBase::vertex(int index) const
{
    return m_vertices.at(index);
}

const GLfloat * MCGLObjectBase::verticesAsGlArray() const
{
    return reinterpret_cast<const GLfloat *>(&m_vertices[0]);
}

int MCGLObjectBase::vertexCount() const
{
    return m_vertices.size();
}

void MCGLObjectBase::addNormal(const MCGLVertex & normal)
{
    m_normals.push_back(normal);
}

void MCGLObjectBase::setNormals(const VertexVector & normals)
{
    m_normals = normals;
}

const MCGLVertex & MCGLObjectBase::normal(int index) const
{
    return m_normals.at(index);
}

const GLfloat * MCGLObjectBase::normalsAsGlArray() const
{
    return reinterpret_cast<const GLfloat *>(&m_normals[0]);
}

void MCGLObjectBase::addColor(const MCGLColor & color)
{
    m_colors.push_back(color);
}

void MCGLObjectBase::setColors(const ColorVector & colors)
{
    m_colors = colors;
}

const MCGLColor & MCGLObjectBase::color(int index) const
{
    return m_colors.at(index);
}

const GLfloat * MCGLObjectBase::colorsAsGlArray() const
{
    return reinterpret_cast<const GLfloat *>(&m_colors[0]);
}

void MCGLObjectBase::addTexCoord(const MCGLTexCoord & texCoord)
{
    m_texCoords.push_back(texCoord);
}

void MCGLObjectBase::setTexCoords(const TexCoordVector & texCoords)
{
    m_texCoords = texCoords;
}

const MCGLTexCoord & MCGLObjectBase::texCoord(int index) const
{
    return m_texCoords.at(index);
}

const GLfloat * MCGLObjectBase::texCoordsAsGlArray() const
{
    return reinterpret_cast<const GLfloat *>(&m_texCoords[0]);
}

void MCGLObjectBase::initUpdateBufferData()
{
    bindVAO();
    bindVBO();

    glBufferData(GL_ARRAY_BUFFER, m_totalDataSize, nullptr, GL_DYNAMIC_DRAW);

    m_bufferDataOffset = 0;
}

void MCGLObjectBase::addBufferSubData(
    MCGLShaderProgram::VertexAttribLocations dataType, int dataSize, const GLfloat * data)
{
    addBufferSubData(dataType, dataSize, dataSize, data);
}

void MCGLObjectBase::addBufferSubData(
    MCGLShaderProgram::VertexAttribLocations dataType, int dataSize, int offsetJump, const GLfloat * data)
{
    assert(dataSize <= offsetJump);

    glBufferSubData(GL_ARRAY_BUFFER, m_bufferDataOffset, dataSize, data);

    m_bufferDataOffset += offsetJump;

    switch (dataType) {
    case MCGLShaderProgram::VAL_Vertex:
        m_vertexDataSize = dataSize;
        break;
    case MCGLShaderProgram::VAL_Normal:
        m_normalDataSize = dataSize;
        break;
    case MCGLShaderProgram::VAL_TexCoords:
        m_texCoordDataSize = dataSize;
        break;
    case MCGLShaderProgram::VAL_Color:
        m_colorDataSize = dataSize;
        break;
    default:
        assert(false);
    }
}

void MCGLObjectBase::enableAttributePointers()
{
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Vertex);
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Normal);
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_TexCoords);
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Color);
}

void MCGLObjectBase::setAttributePointers()
{
    enableAttributePointers();

    glVertexAttribPointer(MCGLShaderProgram::VAL_Vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glVertexAttribPointer(MCGLShaderProgram::VAL_Normal, 3, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid *>(m_vertexDataSize));

    glVertexAttribPointer(MCGLShaderProgram::VAL_TexCoords, 2, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid *>(m_vertexDataSize + m_normalDataSize));

    glVertexAttribPointer(MCGLShaderProgram::VAL_Color, 4, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid *>(m_vertexDataSize + m_normalDataSize + m_texCoordDataSize));
}

void MCGLObjectBase::disableAttributePointers()
{
    glDisableVertexAttribArray(MCGLShaderProgram::VAL_Vertex);
    glDisableVertexAttribArray(MCGLShaderProgram::VAL_Normal);
    glDisableVertexAttribArray(MCGLShaderProgram::VAL_TexCoords);
    glDisableVertexAttribArray(MCGLShaderProgram::VAL_Color);
}

void MCGLObjectBase::finishBufferData()
{
    setAttributePointers();

    releaseVBO();
    releaseVAO();
}

int MCGLObjectBase::totalDataSize() const
{
    return m_totalDataSize;
}

MCGLObjectBase::~MCGLObjectBase()
{
    if (m_vbo != 0)
    {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
#ifndef __MC_QOPENGLFUNCTIONS__
    if (m_vao != 0)
    {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
#endif
}
