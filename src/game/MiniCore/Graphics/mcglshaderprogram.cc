// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "mcglshaderprogram.hh"
#include "mcglscene.hh"

#ifdef __MC_GLES__
#include "mcshadersGLES.hh"
#elif defined(__MC_GL30__)
#include "mcshaders30.hh"
#else
#include "mcshaders.hh"
#endif

#include <MCException>
#include <MCLogger>
#include <MCTrigonom>
#include <cassert>

// Uniform names
static const char * AMBIENT_LIGHT_COLOR = "ac";
static const char * DIFFUSE_LIGHT_DIR   = "dd";
static const char * DIFFUSE_LIGHT_COLOR = "dc";
static const char * FADE                = "fade";
static const char * TEX0                = "tex0";
static const char * TEX1                = "tex1";
static const char * TEX2                = "tex2";
static const char * VP                  = "vp";
static const char * MODEL               = "model";
static const char * COLOR               = "color";
static const char * SCALE               = "scale";
static const char * POINT_SIZE          = "pointSize";

MCGLShaderProgram * MCGLShaderProgram::m_activeProgram = nullptr;

MCGLShaderProgram::MCGLShaderProgram(MCGLScene & scene)
: m_scene(scene)
, m_isBound(false)
, m_program(glCreateProgram())
, m_fragmentShader(glCreateShader(GL_FRAGMENT_SHADER))
, m_vertexShader(glCreateShader(GL_VERTEX_SHADER))
{
    m_scene.addShaderProgram(*this);
}

MCGLShaderProgram::~MCGLShaderProgram()
{
    glDeleteProgram(m_program);
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);
}

void MCGLShaderProgram::bind()
{
    if (!m_isBound)
    {
        m_isBound = true;

        if (MCGLShaderProgram::m_activeProgram && MCGLShaderProgram::m_activeProgram != this)
        {
            MCGLShaderProgram::m_activeProgram->release();
        }

        MCGLShaderProgram::m_activeProgram = this;

        glUseProgram(m_program);
    }
}

void MCGLShaderProgram::release()
{
    if (m_isBound)
    {
        m_isBound = false;

        if (MCGLShaderProgram::m_activeProgram == this)
        {
            MCGLShaderProgram::m_activeProgram = nullptr;
        }
    }
}

bool MCGLShaderProgram::isBound() const
{
    return m_isBound;
}

void MCGLShaderProgram::link()
{
    glLinkProgram(m_program);
    assert(isLinked());
}

bool MCGLShaderProgram::isLinked() const
{
    GLint status = GL_FALSE;
    glGetProgramiv(m_program, GL_LINK_STATUS, &status);
    return status == GL_TRUE;
}

std::string getShaderLog(GLuint obj)
{
    int logLength = 0;
    int charsWritten = 0;
    char *rawLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &logLength);

    if (logLength > 0)
    {
        rawLog = (char *)malloc(logLength);
        glGetShaderInfoLog(obj, logLength, &charsWritten, rawLog);
        std::string log = rawLog;
        free(rawLog);
        return log;
    }

    return "";
}

bool MCGLShaderProgram::addVertexShaderFromSource(const std::string & source)
{
    GLint len = source.length();
    const GLchar * data = source.c_str();
    glShaderSource(m_vertexShader, 1, &data, &len);
    glCompileShader(m_vertexShader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        throw MCException("Compiling a vertex shader failed.\n" +
            getShaderLog(m_vertexShader) + "\n" + source);
    }

    glBindAttribLocation(m_program, MCGLShaderProgram::VAL_Vertex,    "inVertex");
    glBindAttribLocation(m_program, MCGLShaderProgram::VAL_Normal,    "inNormal");
    glBindAttribLocation(m_program, MCGLShaderProgram::VAL_TexCoords, "inTexCoord");
    glBindAttribLocation(m_program, MCGLShaderProgram::VAL_Color,     "inColor");

    glAttachShader(m_program, m_vertexShader);

    return true;
}

bool MCGLShaderProgram::addFragmentShaderFromSource(const std::string & source)
{
    GLint len = source.length();
    const GLchar * data = source.c_str();
    glShaderSource(m_fragmentShader, 1, &data, &len);
    glCompileShader(m_fragmentShader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        throw MCException("Compiling a fragment shader failed.\n" +
            getShaderLog(m_fragmentShader) + "\n" + source);
    }

    glAttachShader(m_program, m_fragmentShader);

    return true;
}

const char * MCGLShaderProgram::getDefaultVertexShaderSource()
{
    return MCMasterVsh;
}

const char * MCGLShaderProgram::getDefaultFragmentShaderSource()
{
    return MCMasterFsh;
}

const char * MCGLShaderProgram::getDefaultShadowVertexShaderSource()
{
    return MCMasterShadowVsh;
}

const char * MCGLShaderProgram::getDefaultShadowFragmentShaderSource()
{
    return MCMasterShadowFsh;
}

bool MCGLShaderProgram::addGeometryShaderFromSource(const std::string &)
{
    return false;
}

void MCGLShaderProgram::setViewProjectionMatrix(
    const glm::mat4x4 & viewProjectionMatrix)
{
    bind();
    glUniformMatrix4fv(
        glGetUniformLocation(m_program, VP), 1, GL_FALSE, &viewProjectionMatrix[0][0]);
}

void MCGLShaderProgram::setTransform(GLfloat angle, const MCVector3dF & pos)
{
    bind();
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(pos.i(), pos.j(), pos.k()));
    glm::mat4 rotation  = glm::rotate(translate, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(
        glGetUniformLocation(m_program, MODEL), 1, GL_FALSE, &rotation[0][0]);
}

void MCGLShaderProgram::setColor(const MCGLColor & color)
{
    bind();
    glUniform4f(
        glGetUniformLocation(m_program, COLOR), color.r(), color.g(), color.b(), color.a());
}

void MCGLShaderProgram::setScale(GLfloat x, GLfloat y, GLfloat z)
{
    bind();
    glUniform4f(
        glGetUniformLocation(m_program, SCALE), x, y, z, 1);
}

void MCGLShaderProgram::setDiffuseLight(const MCGLDiffuseLight & light)
{
    bind();
    glUniform4f(
        glGetUniformLocation(m_program, DIFFUSE_LIGHT_DIR),
            light.direction().i(), light.direction().j(), light.direction().k(), 1);
    glUniform4f(
        glGetUniformLocation(m_program, DIFFUSE_LIGHT_COLOR),
            light.r(), light.g(), light.b(), light.i());
}

void MCGLShaderProgram::setAmbientLight(const MCGLAmbientLight & light)
{
    bind();
    glUniform4f(
        glGetUniformLocation(m_program, AMBIENT_LIGHT_COLOR),
            light.r(), light.g(), light.b(), light.i());
}

void MCGLShaderProgram::setFadeValue(GLfloat f)
{
    bind();
    glUniform1f(
        glGetUniformLocation(m_program, FADE), f);
}

void MCGLShaderProgram::bindTextureUnit0(GLuint index)
{
    bind();
    glUniform1i(
        glGetUniformLocation(m_program, TEX0), index);
}

void MCGLShaderProgram::bindTextureUnit1(GLuint index)
{
    bind();
    glUniform1i(
        glGetUniformLocation(m_program, TEX1), index);
}

void MCGLShaderProgram::bindTextureUnit2(GLuint index)
{
    bind();
    glUniform1i(
        glGetUniformLocation(m_program, TEX2), index);
}

void MCGLShaderProgram::setPointSize(GLfloat pointSize)
{
    bind();
    glUniform1f(
        glGetUniformLocation(m_program, POINT_SIZE), pointSize);
}
