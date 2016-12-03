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

#include <MCLogger>
#include <MCTrigonom>

#include <cassert>
#include <exception>

MCGLShaderProgram * MCGLShaderProgram::m_activeProgram = nullptr;

std::vector<MCGLShaderProgram *> MCGLShaderProgram::m_programStack;

MCGLShaderProgram::MCGLShaderProgram()
    : m_scene(MCGLScene::instance())
    , m_viewProjectionMatrixPending(false)
    , m_viewMatrixPending(false)
    , m_diffuseLightPending(false)
    , m_specularLightPending(false)
    , m_ambientLightPending(false)
{
#ifdef __MC_QOPENGLFUNCTIONS__
    initializeOpenGLFunctions();
#endif
    initUniformNameMap();

    m_program = glCreateProgram();
    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
}

MCGLShaderProgram::MCGLShaderProgram(
    const std::string & vertexShaderSource, const std::string & fragmentShaderSource)
    : m_scene(MCGLScene::instance())
    , m_viewProjectionMatrixPending(false)
    , m_viewMatrixPending(false)
    , m_diffuseLightPending(false)
    , m_specularLightPending(false)
    , m_ambientLightPending(false)
{
#ifdef __MC_QOPENGLFUNCTIONS__
    initializeOpenGLFunctions();
#endif
    initUniformNameMap();

    m_program = glCreateProgram();
    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);

    addVertexShaderFromSource(vertexShaderSource);
    addFragmentShaderFromSource(fragmentShaderSource);
    link();
}

void MCGLShaderProgram::initUniformNameMap()
{
    // Map uniform enums to uniform names used in the shaders
    m_uniforms[AmbientLightColor]  = "ac";
    m_uniforms[Camera]             = "camera";
    m_uniforms[Color]              = "color";
    m_uniforms[DiffuseLightDir]    = "dd";
    m_uniforms[DiffuseLightColor]  = "dc";
    m_uniforms[SpecularLightDir]   = "sd";
    m_uniforms[SpecularLightColor] = "sc";
    m_uniforms[SpecularCoeff]      = "sCoeff";
    m_uniforms[Tex0]               = "tex0";
    m_uniforms[Tex1]               = "tex1";
    m_uniforms[Tex2]               = "tex2";
    m_uniforms[UserData1]          = "userData1";
    m_uniforms[UserData2]          = "userData2";
    m_uniforms[ViewProjection]     = "vp";
    m_uniforms[View]               = "v";
    m_uniforms[Model]              = "model";
    m_uniforms[Scale]              = "scale";
    m_uniforms[Camera]             = "camera";
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
    MCGLShaderProgram::m_activeProgram = this;
    glUseProgram(m_program);

    setPendingAmbientLight();
    setPendingDiffuseLight();
    setPendingSpecularLight();
    setPendingViewMatrix();
    setPendingViewProjectionMatrix();
}

void MCGLShaderProgram::release()
{
    MCGLShaderProgram::m_activeProgram = nullptr;
}

bool MCGLShaderProgram::isBound() const
{
    return MCGLShaderProgram::m_activeProgram == this;
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

bool MCGLShaderProgram::isLinked()
{
    GLint status = GL_FALSE;
    glGetProgramiv(m_program, GL_LINK_STATUS, &status);
    return status == GL_TRUE;
}

std::string MCGLShaderProgram::getShaderLog(GLuint obj)
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
    GLint len = static_cast<GLint>(source.length());
    const GLchar * data = source.c_str();
    glShaderSource(m_vertexShader, 1, &data, &len);
    glCompileShader(m_vertexShader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        throw std::runtime_error("Compiling a vertex shader failed.\n" +
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
    GLint len = static_cast<GLint>(source.length());
    const GLchar * data = source.c_str();
    glShaderSource(m_fragmentShader, 1, &data, &len);
    glCompileShader(m_fragmentShader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        throw std::runtime_error("Compiling a fragment shader failed.\n" +
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

const char * MCGLShaderProgram::getDefaultFBOVertexShaderSource()
{
    return MCDefaultFBOVsh;
}

const char * MCGLShaderProgram::getDefaultFBOFragmentShaderSource()
{
    return MCDefaultFBOFsh;
}

bool MCGLShaderProgram::addGeometryShaderFromSource(const std::string &)
{
    return false;
}

void MCGLShaderProgram::pushProgram()
{
    if (MCGLShaderProgram::m_activeProgram) {
        m_programStack.push_back(MCGLShaderProgram::m_activeProgram);
    }
}

void MCGLShaderProgram::popProgram()
{
    if (m_programStack.size()) {
        m_programStack.back()->bind();
        m_programStack.pop_back();
    }
}

void MCGLShaderProgram::setViewProjectionMatrix(const glm::mat4x4 & viewProjectionMatrix)
{
    m_viewProjectionMatrix = viewProjectionMatrix;
    m_viewProjectionMatrixPending = true;

    if (isBound()) {
        setPendingViewProjectionMatrix();
    }
}

void MCGLShaderProgram::setPendingViewProjectionMatrix()
{
    if (m_viewProjectionMatrixPending) {
        m_viewProjectionMatrixPending = false;
        glUniformMatrix4fv(getUniformLocation(ViewProjection), 1, GL_FALSE, &m_viewProjectionMatrix[0][0]);
    }
}

void MCGLShaderProgram::setViewMatrix(const glm::mat4x4 & viewMatrix)
{
    m_viewMatrix = viewMatrix;
    m_viewMatrixPending = true;

    if (isBound()) {
        setPendingViewMatrix();
    }
}

void MCGLShaderProgram::setPendingViewMatrix()
{
    if (m_viewMatrixPending) {
        m_viewMatrixPending = false;
        glUniformMatrix4fv(getUniformLocation(View), 1, GL_FALSE, &m_viewMatrix[0][0]);
    }
}

void MCGLShaderProgram::setTransform(GLfloat angle, const MCVector3dF & pos)
{
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(pos.i(), pos.j(), pos.k()));
    glm::mat4 rotation  = glm::rotate(translate, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    glUniformMatrix4fv(getUniformLocation(Model), 1, GL_FALSE, &rotation[0][0]);
}

void MCGLShaderProgram::setUserData1(const MCVector2dF & data)
{
    glUniform2f(getUniformLocation(UserData1), data.i(), data.j());
}

void MCGLShaderProgram::setUserData2(const MCVector2dF & data)
{
    glUniform2f(getUniformLocation(UserData2), data.i(), data.j());
}

void MCGLShaderProgram::setCamera(const MCVector2dF & camera)
{
    glUniform2f(getUniformLocation(Camera), camera.i(), camera.j());
}

void MCGLShaderProgram::setColor(const MCGLColor & color)
{
    glUniform4f(getUniformLocation(Color), color.r(), color.g(), color.b(), color.a());
}

void MCGLShaderProgram::setScale(GLfloat x, GLfloat y, GLfloat z)
{
    glUniform4f(getUniformLocation(Scale), x, y, z, 1);
}

void MCGLShaderProgram::setDiffuseLight(const MCGLDiffuseLight & light)
{
    m_diffuseLight = light;
    m_diffuseLightPending = true;

    if (isBound()) {
        setPendingDiffuseLight();
    }
}

void MCGLShaderProgram::setPendingDiffuseLight()
{
    if (m_diffuseLightPending) {
        m_diffuseLightPending = false;
        glUniform4f(
            getUniformLocation(DiffuseLightDir),
                m_diffuseLight.direction().i(), m_diffuseLight.direction().j(), m_diffuseLight.direction().k(), 1);
        glUniform4f(
            getUniformLocation(DiffuseLightColor),
                m_diffuseLight.r(), m_diffuseLight.g(), m_diffuseLight.b(), m_diffuseLight.i());
    }
}

void MCGLShaderProgram::setSpecularLight(const MCGLDiffuseLight & light)
{
    m_specularLight = light;
    m_specularLightPending = true;

    if (isBound()) {
        setPendingSpecularLight();
    }
}

void MCGLShaderProgram::setPendingSpecularLight()
{
    if (m_specularLightPending) {
        m_specularLightPending = false;
        glUniform4f(
            getUniformLocation(SpecularLightDir),
                m_specularLight.direction().i(), m_specularLight.direction().j(), m_specularLight.direction().k(), 1);
        glUniform4f(
            getUniformLocation(SpecularLightColor),
                m_specularLight.r(), m_specularLight.g(), m_specularLight.b(), m_specularLight.i());
    }
}

void MCGLShaderProgram::setAmbientLight(const MCGLAmbientLight & light)
{
    m_ambientLight = light;
    m_ambientLightPending = true;

    if (isBound()) {
        setPendingAmbientLight();
    }
}

void MCGLShaderProgram::setPendingAmbientLight()
{
    if (m_ambientLightPending) {
        m_ambientLightPending = false;
        glUniform4f(
            getUniformLocation(AmbientLightColor),
                m_ambientLight.r(), m_ambientLight.g(), m_ambientLight.b(), m_ambientLight.i());
    }
}

void MCGLShaderProgram::bindTextureUnit(GLuint index, Uniform uniform)
{
    const int location = getUniformLocation(uniform);
    if (location != -1)
    {
        glUniform1i(getUniformLocation(uniform), index);
    }
}

void MCGLShaderProgram::bindMaterial(MCGLMaterialPtr material)
{
    assert(material);

    const GLuint texture1 = material->texture(0);
    const GLuint texture2 = material->texture(1);
    const GLuint texture3 = material->texture(2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture3);

    glActiveTexture(GL_TEXTURE0);

    glUniform1f(getUniformLocation(SpecularCoeff), material->specularCoeff());
}
