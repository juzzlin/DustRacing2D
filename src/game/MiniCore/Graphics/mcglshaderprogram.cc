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

MCGLShaderProgram * MCGLShaderProgram::m_activeProgram = nullptr;
std::vector<GLuint> MCGLShaderProgram::m_activeTexture(MCGLMaterial::MAX_TEXTURES, 0);

MCGLShaderProgram::MCGLShaderProgram()
    : m_scene(MCGLScene::instance())
    , m_isBound(false)
    , m_program(glCreateProgram())
    , m_fragmentShader(glCreateShader(GL_FRAGMENT_SHADER))
    , m_vertexShader(glCreateShader(GL_VERTEX_SHADER))
{
    initUniformNameMap();
}

MCGLShaderProgram::MCGLShaderProgram(
    const std::string & vertexShaderSource, const std::string & fragmentShaderSource)
    : m_scene(MCGLScene::instance())
    , m_isBound(false)
    , m_program(glCreateProgram())
    , m_fragmentShader(glCreateShader(GL_FRAGMENT_SHADER))
    , m_vertexShader(glCreateShader(GL_VERTEX_SHADER))
{
    initUniformNameMap();

    addVertexShaderFromSource(vertexShaderSource);
    addFragmentShaderFromSource(fragmentShaderSource);
    link();
}

void MCGLShaderProgram::initUniformNameMap()
{
    // Map uniform enums to uniform names used in the shaders
    m_uniforms[AmbientLightColor]  = "ac";
    m_uniforms[DiffuseLightDir]    = "dd";
    m_uniforms[DiffuseLightColor]  = "dc";
    m_uniforms[SpecularLightDir]   = "sd";
    m_uniforms[SpecularLightColor] = "sc";
    m_uniforms[SpecularCoeff]      = "sCoeff";
    m_uniforms[Fade]               = "fade";
    m_uniforms[Tex0]               = "tex0";
    m_uniforms[Tex1]               = "tex1";
    m_uniforms[Tex2]               = "tex2";
    m_uniforms[ViewProjection]     = "vp";
    m_uniforms[View]               = "v";
    m_uniforms[Model]              = "model";
    m_uniforms[Color]              = "color";
    m_uniforms[Scale]              = "scale";
    m_uniforms[PointSize]          = "pointSize";
}

MCGLShaderProgram::~MCGLShaderProgram()
{
    glDeleteProgram(m_program);
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);
}

int MCGLShaderProgram::getUniformLocation(Uniform uniform)
{
    return m_uniformLocationHash[uniform];
}

void MCGLShaderProgram::initUniformLocationCache()
{
    assert(isLinked());

    auto iter = m_uniforms.begin();
    while (iter != m_uniforms.end())
    {
        m_uniformLocationHash[iter->first] = glGetUniformLocation(m_program, iter->second.c_str());
        iter++;
    }
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

    initUniformLocationCache();
    m_scene.addShaderProgram(*this);

    bindTextureUnit(0, Tex0);
    bindTextureUnit(1, Tex1);
    bindTextureUnit(2, Tex2);
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
    return MCDefaultVsh;
}

const char * MCGLShaderProgram::getDefaultSpecularVertexShaderSource()
{
    return MCDefaultVshSpecular;
}

const char * MCGLShaderProgram::getDefaultFragmentShaderSource()
{
    return MCDefaultFsh;
}

const char * MCGLShaderProgram::getDefaultShadowVertexShaderSource()
{
    return MCDefaultShadowVsh;
}

const char * MCGLShaderProgram::getDefaultShadowFragmentShaderSource()
{
    return MCDefaultShadowFsh;
}

const char * MCGLShaderProgram::getDefaultPointParticleVertexShaderSource()
{
    return MCDefaultPointParticleVsh;
}

const char * MCGLShaderProgram::getDefaultParticleFragmentShaderSource()
{
    return MCDefaultParticleFsh;
}

const char * MCGLShaderProgram::getDefaultPointParticleFragmentShaderSource()
{
    return MCDefaultPointParticleFsh;
}

const char * MCGLShaderProgram::getDefaultPointParticleRotateFragmentShaderSource()
{
    return MCDefaultPointParticleRotateFsh;
}

const char * MCGLShaderProgram::getDefaultTextVertexShaderSource()
{
    return MCDefaultTextVsh;
}

const char * MCGLShaderProgram::getDefaultTextFragmentShaderSource()
{
    return MCDefaultFsh;
}

const char * MCGLShaderProgram::getDefaultTextShadowFragmentShaderSource()
{
    return MCDefaultTextShadowFsh;
}

bool MCGLShaderProgram::addGeometryShaderFromSource(const std::string &)
{
    return false;
}

void MCGLShaderProgram::setViewProjectionMatrix(
    const glm::mat4x4 & viewProjectionMatrix)
{
    bind();
    glUniformMatrix4fv(getUniformLocation(ViewProjection), 1, GL_FALSE, &viewProjectionMatrix[0][0]);
}

void MCGLShaderProgram::setViewMatrix(
    const glm::mat4x4 & viewMatrix)
{
    bind();
    glUniformMatrix4fv(getUniformLocation(View), 1, GL_FALSE, &viewMatrix[0][0]);
}

void MCGLShaderProgram::setTransform(GLfloat angle, const MCVector3dF & pos)
{
    bind();
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(pos.i(), pos.j(), pos.k()));
    glm::mat4 rotation  = glm::rotate(translate, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(getUniformLocation(Model), 1, GL_FALSE, &rotation[0][0]);
}

void MCGLShaderProgram::setColor(const MCGLColor & color)
{
    bind();
    glUniform4f(getUniformLocation(Color), color.r(), color.g(), color.b(), color.a());
}

void MCGLShaderProgram::setScale(GLfloat x, GLfloat y, GLfloat z)
{
    bind();
    glUniform4f(getUniformLocation(Scale), x, y, z, 1);
}

void MCGLShaderProgram::setDiffuseLight(const MCGLDiffuseLight & light)
{
    bind();
    glUniform4f(
        getUniformLocation(DiffuseLightDir),
            light.direction().i(), light.direction().j(), light.direction().k(), 1);
    glUniform4f(
        getUniformLocation(DiffuseLightColor),
            light.r(), light.g(), light.b(), light.i());
}

void MCGLShaderProgram::setSpecularLight(const MCGLDiffuseLight & light)
{
    bind();
    glUniform4f(
        getUniformLocation(SpecularLightDir),
            light.direction().i(), light.direction().j(), light.direction().k(), 1);
    glUniform4f(
        getUniformLocation(SpecularLightColor),
            light.r(), light.g(), light.b(), light.i());
}

void MCGLShaderProgram::setAmbientLight(const MCGLAmbientLight & light)
{
    bind();
    glUniform4f(
        getUniformLocation(AmbientLightColor),
            light.r(), light.g(), light.b(), light.i());
}

void MCGLShaderProgram::setFadeValue(GLfloat f)
{
    bind();
    glUniform1f(getUniformLocation(Fade), f);
}

void MCGLShaderProgram::bindTextureUnit(GLuint index, Uniform uniform)
{
    bind();
    const int location = getUniformLocation(uniform);
    if (location != -1)
    {
        glUniform1i(getUniformLocation(uniform), index);
    }
}

void MCGLShaderProgram::bindMaterial(MCGLMaterialPtr material)
{
    bind();

    const GLuint texture1 = material->texture(0);
    const GLuint texture2 = material->texture(1);
    const GLuint texture3 = material->texture(2);

    if (MCGLShaderProgram::m_activeTexture[0] != texture1)
    {
        MCGLShaderProgram::m_activeTexture[0] = texture1;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
    }

    if (texture2 && MCGLShaderProgram::m_activeTexture[1] != texture2)
    {
        MCGLShaderProgram::m_activeTexture[1] = texture2;
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
    }

    if (texture3 && MCGLShaderProgram::m_activeTexture[2] != texture3)
    {
        MCGLShaderProgram::m_activeTexture[2] = texture3;
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texture3);
    }

    glActiveTexture(GL_TEXTURE0);

    glUniform1f(getUniformLocation(SpecularCoeff), material->specularCoeff());
}

void MCGLShaderProgram::setPointSize(GLfloat pointSize)
{
    bind();
    glUniform1f(getUniformLocation(PointSize), pointSize);
}
