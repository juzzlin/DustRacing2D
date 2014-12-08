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
#include "mcexception.hh"

#include <cassert>

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
, m_hasVao(true)
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
    // Intentionally do nothing
}

void MCGLObjectBase::createVAO()
{
#ifdef __MC_QOPENGLFUNCTIONS__
    m_hasVao = m_vao.create();
#else
    if (m_vao == 0)
    {
        glGenVertexArrays(1, &m_vao);
    }
#endif
}

void MCGLObjectBase::bindVBO()
{
    if (MCGLObjectBase::m_boundVbo != m_vbo)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        MCGLObjectBase::m_boundVbo = m_vbo;
    }
}

void MCGLObjectBase::releaseVBO()
{
    MCGLObjectBase::m_boundVbo = 0;
}

void MCGLObjectBase::createVBO()
{
    if (m_vbo == 0)
    {
        glGenBuffers(1, &m_vbo);
    }
}

void MCGLObjectBase::bindMaterial(bool bindForShadow)
{
    if (m_material)
    {
        MCGLShaderProgramPtr program = bindForShadow ? m_shadowProgram : m_program;
        assert(program.get());

        program->bindMaterial(m_material);
    }
}

void MCGLObjectBase::bind()
{
    if (shaderProgram())
    {
        shaderProgram()->bind();

        bindVAO();
        bindMaterial();
    }
    else
    {
        // Save the user from debugging as to why nothing is being drawn.
        throw MCException("Trying to bind MCGLObject but shader program for it not set!");
    }
}

void MCGLObjectBase::bindShadow()
{
    if (shadowShaderProgram())
    {
        shadowShaderProgram()->bind();

        bindVAO();
        bindMaterial(true);
    }
    else
    {
        // Save the user from debugging as to why nothing is being drawn.
        throw MCException("Trying to render shadow for surface, but shader program for it not set!");
    }
}

void MCGLObjectBase::release()
{
}

void MCGLObjectBase::releaseShadow()
{
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
    createVAO();
    createVBO();

    bindVAO();
    bindVBO();

    glBufferData(GL_ARRAY_BUFFER, totalDataSize, nullptr, drawType);

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

void MCGLObjectBase::setAttributePointers()
{
    glVertexAttribPointer(MCGLShaderProgram::VAL_Vertex, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Vertex);

    glVertexAttribPointer(MCGLShaderProgram::VAL_Normal, 3, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid *>(m_vertexDataSize));
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Normal);

    glVertexAttribPointer(MCGLShaderProgram::VAL_TexCoords, 2, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid *>(m_vertexDataSize + m_normalDataSize));
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_TexCoords);

    glVertexAttribPointer(MCGLShaderProgram::VAL_Color, 4, GL_FLOAT, GL_FALSE, 0,
        reinterpret_cast<GLvoid *>(m_vertexDataSize + m_normalDataSize + m_texCoordDataSize));
    glEnableVertexAttribArray(MCGLShaderProgram::VAL_Color);
}

void MCGLObjectBase::finishBufferData()
{
    setAttributePointers();

    releaseVBO();
    releaseVAO();
}


void MCGLObjectBase::initUpdateBufferData()
{
    bindVAO();
    bindVBO();

    m_bufferDataOffset = 0;
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
