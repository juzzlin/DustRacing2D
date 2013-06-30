// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include "shaderprogram.hpp"
#include <MCException>
#include <MCLogger>
#include <MCTrigonom>
#include <cassert>

// Uniform names
static const char * AMBIENT_LIGHT_COLOR = "ambientLightColor";
static const char * DIFFUSE_LIGHT_DIR   = "diffuseLightDir";
static const char * DIFFUSE_LIGHT_COLOR = "diffuseLightColor";
static const char * FADE                = "fade";
static const char * TEX0                = "tex0";
static const char * TEX1                = "tex1";
static const char * TEX2                = "tex2";
static const char * MVP                 = "mvp";
static const char * ANGLE               = "angle";
static const char * POS                 = "pos";
static const char * COLOR               = "color";
static const char * SCALE               = "scale";

ShaderProgram::ShaderProgram(const QGLContext * context, MCGLScene & scene)
: MCGLShaderProgram(scene)
, m_program(context)
, m_fragmentShader(QGLShader::Fragment, context)
, m_vertexShader(QGLShader::Vertex, context)
{
}

void ShaderProgram::bind()
{
    if (!isBound())
    {
        MCGLShaderProgram::bind();
        m_program.bind();
    }
}

void ShaderProgram::release()
{
    if (isBound())
    {
        MCGLShaderProgram::release();
        m_program.release();
    }
}

void ShaderProgram::link()
{
    m_program.link();
    assert(m_program.isLinked());
}

bool ShaderProgram::isLinked() const
{
    return m_program.isLinked();
}

bool ShaderProgram::addVertexShaderFromFile(const std::string & fileName)
{
    if (!m_vertexShader.compileSourceFile(fileName.c_str()))
    {
        MCLogger().error() << "Cannot compile '" << fileName << "'";
        throw MCException("Compiling a vertex shader failed.");
    }

    m_program.bindAttributeLocation("inVertex",   MCGLShaderProgram::VAL_Vertex);
    m_program.bindAttributeLocation("inNormal",   MCGLShaderProgram::VAL_Normal);
    m_program.bindAttributeLocation("inTexCoord", MCGLShaderProgram::VAL_TexCoords);
    m_program.bindAttributeLocation("inColor",    MCGLShaderProgram::VAL_Color);

    return m_program.addShader(&m_vertexShader);
}

bool ShaderProgram::addFragmentShaderFromFile(const std::string & fileName)
{
    if (!m_fragmentShader.compileSourceFile(fileName.c_str()))
    {
        MCLogger().error() << "Cannot compile '" << fileName << "'";
        throw MCException("Compiling a fragment shader failed.");
    }

    return m_program.addShader(&m_fragmentShader);
}

bool ShaderProgram::addVertexShaderFromSource(const std::string & source)
{
    if (!m_vertexShader.compileSourceCode(source.c_str()))
    {
        throw MCException("Compiling a vertex shader failed.");
    }

    m_program.bindAttributeLocation("inVertex",   MCGLShaderProgram::VAL_Vertex);
    m_program.bindAttributeLocation("inNormal",   MCGLShaderProgram::VAL_Normal);
    m_program.bindAttributeLocation("inTexCoord", MCGLShaderProgram::VAL_TexCoords);
    m_program.bindAttributeLocation("inColor",    MCGLShaderProgram::VAL_Color);

    return m_program.addShader(&m_vertexShader);
}

bool ShaderProgram::addFragmentShaderFromSource(const std::string & source)
{
    if (!m_fragmentShader.compileSourceCode(source.c_str()))
    {
        throw MCException("Compiling a fragment shader failed.");
    }

    return m_program.addShader(&m_fragmentShader);
}

void ShaderProgram::setModelViewProjectionMatrix(
    const glm::mat4x4 & modelViewProjectionMatrix)
{
    bind();
    glUniformMatrix4fv(
        m_program.uniformLocation(MVP), 1, GL_FALSE, &modelViewProjectionMatrix[0][0]);
}

void ShaderProgram::rotate(GLfloat angle)
{
    bind();
    m_program.setUniformValue(ANGLE, MCTrigonom::sin(angle), MCTrigonom::cos(angle));
}

void ShaderProgram::translate(const MCVector3dF & p)
{
    bind();
    m_program.setUniformValue(POS, p.i(), p.j(), p.k(), 0);
}

void ShaderProgram::setColor(const MCGLColor & color)
{
    bind();
    m_program.setUniformValue(COLOR, color.r(), color.g(), color.b(), color.a());
}

void ShaderProgram::setScale(GLfloat x, GLfloat y, GLfloat z)
{
    bind();
    m_program.setUniformValue(SCALE, x, y, z, 1);
}

void ShaderProgram::setDiffuseLight(const MCGLDiffuseLight & light)
{
    bind();
    m_program.setUniformValue(
        DIFFUSE_LIGHT_DIR, light.direction().i(), light.direction().j(), light.direction().k(), 1);
    m_program.setUniformValue(
        DIFFUSE_LIGHT_COLOR, light.r(), light.g(), light.b(), light.i());
}

void ShaderProgram::setAmbientLight(const MCGLAmbientLight & light)
{
    bind();
    m_program.setUniformValue(AMBIENT_LIGHT_COLOR, light.r(), light.g(), light.b(), light.i());
}

void ShaderProgram::setFadeValue(GLfloat f)
{
    bind();
    m_program.setUniformValue(FADE, f);
}

void ShaderProgram::bindTextureUnit0(GLuint index)
{
    bind();
    m_program.setUniformValue(TEX0, index);
}

void ShaderProgram::bindTextureUnit1(GLuint index)
{
    bind();
    m_program.setUniformValue(TEX1, index);
}

void ShaderProgram::bindTextureUnit2(GLuint index)
{
    bind();
    m_program.setUniformValue(TEX2, index);
}

ShaderProgram::~ShaderProgram()
{
}
