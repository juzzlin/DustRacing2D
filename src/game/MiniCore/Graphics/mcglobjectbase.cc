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
#include "mcglshaderprogram.hh"
#include "mcexception.hh"

#include <cassert>

MCGLObjectBase::MCGLObjectBase()
: m_texture1(0)
, m_texture2(0)
, m_texture3(0)
, m_vao(0)
, m_vbo(0)
, m_program(nullptr)
, m_shadowProgram(nullptr)
{
}

void MCGLObjectBase::setTexture1(GLuint handle)
{
    m_texture1 = handle;
}

void MCGLObjectBase::setTexture2(GLuint handle)
{
    m_texture2 = handle;
}

void MCGLObjectBase::setTexture3(GLuint handle)
{
    m_texture3 = handle;
}

GLuint MCGLObjectBase::texture1() const
{
    return m_texture1;
}

GLuint MCGLObjectBase::texture2() const
{
    return m_texture2;
}

GLuint MCGLObjectBase::texture3() const
{
    return m_texture3;
}

void MCGLObjectBase::setShaderProgram(MCGLShaderProgram * program)
{
    m_program = program;
}

void MCGLObjectBase::setShadowShaderProgram(MCGLShaderProgram * program)
{
    m_shadowProgram = program;
}

MCGLShaderProgram * MCGLObjectBase::shaderProgram() const
{
    return m_program;
}

MCGLShaderProgram * MCGLObjectBase::shadowShaderProgram() const
{
    return m_shadowProgram;
}

void MCGLObjectBase::bindVAO()
{
    glBindVertexArray(m_vao);
}

void MCGLObjectBase::releaseVAO()
{
    glBindVertexArray(0);
}

void MCGLObjectBase::createVAO()
{
    if (m_vao == 0)
    {
        glGenVertexArrays(1, &m_vao);
    }
}

void MCGLObjectBase::bindVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}

void MCGLObjectBase::releaseVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MCGLObjectBase::createVBO()
{
    if (m_vbo == 0)
    {
        glGenBuffers(1, &m_vbo);
    }
}

void MCGLObjectBase::bindTextures(bool bindOnlyFirstTexture, bool bindForShadow)
{
    MCGLShaderProgram * program =
        bindForShadow ? m_shadowProgram : m_program;
    assert(program);

    if (bindOnlyFirstTexture || (!m_texture2 && !m_texture3))
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture1);
        program->bindTextureUnit0(0);
    }
    else
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture1);
        program->bindTextureUnit0(0);

        if (m_texture2)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_texture2);
            program->bindTextureUnit1(1);

            if (m_texture3)
            {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, m_texture3);
                program->bindTextureUnit2(2);
            }
        }

        glActiveTexture(GL_TEXTURE0);
    }
}

void MCGLObjectBase::bind()
{
    bindVAO();
    bindTextures();
}

void MCGLObjectBase::bindShadow()
{
    bindVAO();
    bindTextures(true, true);
}

MCGLObjectBase::~MCGLObjectBase()
{
    if (m_vbo != 0)
    {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }

    if (m_vao != 0)
    {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}
