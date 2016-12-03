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

#include <MCGLEW>

#include "mcglscene.hh"
#include "mcglambientlight.hh"
#include "mcgldiffuselight.hh"
#include "mclogger.hh"
#include "mctrigonom.hh"

#include <algorithm>
#include <cmath>
#include <exception>

MCGLScene * MCGLScene::m_instance = nullptr;

MCGLScene::MCGLScene()
: m_splitType(ShowFullScreen)
, m_viewWidth(0)
, m_viewHeight(0)
, m_sceneWidth(0)
, m_sceneHeight(0)
, m_viewAngle(0)
, m_eyeZ(0)
, m_zNear(0.1f)
, m_zFar(1000.0f)
, m_updateViewProjection(false)
{
    if (!MCGLScene::m_instance) {
        MCGLScene::m_instance = this;
    } else {
        throw std::runtime_error("MCGLScene already created!");
    }
}

MCGLScene & MCGLScene::instance()
{
    assert(MCGLScene::m_instance);
    return *MCGLScene::m_instance;
}

void MCGLScene::addShaderProgram(MCGLShaderProgram & shader)
{
    if (std::find(m_shaders.begin(), m_shaders.end(), &shader) == m_shaders.end())
    {
        m_shaders.push_back(&shader);

        // For some very "funny" reason we have to bind here after adding or
        // else the multi-texturing breaks. Like..wtf?
        shader.bind();

        // Ensure current projection
        shader.setViewProjectionMatrix(viewProjectionMatrix());

        // Lighting defaults
        shader.setAmbientLight(MCGLAmbientLight(1, 1, 1, 1));

        shader.release();
    }
}

MCGLShaderProgramPtr MCGLScene::defaultShaderProgram()
{
    assert(m_defaultShader.get());
    return m_defaultShader;
}

MCGLShaderProgramPtr MCGLScene::defaultSpecularShaderProgram()
{
    assert(m_defaultSpecularShader.get());
    return m_defaultSpecularShader;
}

MCGLShaderProgramPtr MCGLScene::defaultShadowShaderProgram()
{
    assert(m_defaultShadowShader.get());
    return m_defaultShadowShader;
}

MCGLShaderProgramPtr MCGLScene::defaultTextShaderProgram()
{
    assert(m_defaultTextShader.get());
    return m_defaultTextShader;
}

MCGLShaderProgramPtr MCGLScene::defaultTextShadowShaderProgram()
{
    assert(m_defaultTextShadowShader.get());
    return m_defaultTextShadowShader;
}

MCGLShaderProgramPtr MCGLScene::defaultFBOShaderProgram()
{
    assert(m_defaultFBOShader.get());
    return m_defaultFBOShader;
}

void MCGLScene::initialize()
{
#ifndef __MC_NO_GLEW__
    glewExperimental = GL_TRUE; // Needed with CoreProfile 3.3
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        MCLogger().fatal() << "Initing GLEW failed: " << glewGetErrorString(err);
    }
    MCLogger().info() << "Using GLEW " << glewGetString(GLEW_VERSION);
#endif
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
#ifndef __MC_GLES__
    glShadeModel(GL_SMOOTH);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glClearDepth(1.0);
#else
    glClearDepthf(1.0);
#endif

    createDefaultShaderPrograms();
}

void MCGLScene::createDefaultShaderPrograms()
{
    m_defaultShader.reset(new MCGLShaderProgram(
        MCGLShaderProgram::getDefaultVertexShaderSource(), MCGLShaderProgram::getDefaultFragmentShaderSource()));

    m_defaultSpecularShader.reset(new MCGLShaderProgram(
        MCGLShaderProgram::getDefaultSpecularVertexShaderSource(), MCGLShaderProgram::getDefaultFragmentShaderSource()));

    m_defaultShadowShader.reset(new MCGLShaderProgram(
        MCGLShaderProgram::getDefaultShadowVertexShaderSource(), MCGLShaderProgram::getDefaultShadowFragmentShaderSource()));

    m_defaultTextShader.reset(new MCGLShaderProgram(
        MCGLShaderProgram::getDefaultTextVertexShaderSource(), MCGLShaderProgram::getDefaultTextFragmentShaderSource()));

    m_defaultTextShadowShader.reset(new MCGLShaderProgram(
        MCGLShaderProgram::getDefaultTextVertexShaderSource(), MCGLShaderProgram::getDefaultTextShadowFragmentShaderSource()));

    m_defaultFBOShader.reset(new MCGLShaderProgram(
        MCGLShaderProgram::getDefaultFBOVertexShaderSource(), MCGLShaderProgram::getDefaultFBOFragmentShaderSource()));
}

void MCGLScene::resize(
    MCUint viewWidth, MCUint viewHeight, MCUint sceneWidth, MCUint sceneHeight, MCFloat viewAngle, MCFloat zNear, MCFloat zFar)
{
    m_viewWidth   = viewWidth;
    m_viewHeight  = viewHeight;
    m_sceneWidth  = sceneWidth;
    m_sceneHeight = sceneHeight;
    m_viewAngle   = viewAngle;
    m_zNear       = zNear;
    m_zFar        = zFar;

    updateViewport();
}

void MCGLScene::setViewerPosition(MCUint sceneWidth, MCUint sceneHeight, MCFloat viewAngle)
{
    // Set eye position so that the scene looks like a pure 2D-scene
    const MCUint vH2 = sceneHeight / 2;
    const MCUint vW2 = sceneWidth  / 2;
    m_eyeZ = vH2 / std::tan(static_cast<MCFloat>(MCTrigonom::degToRad(viewAngle / 2)));

    m_viewMatrix  = glm::mat4(1.0);
    m_viewMatrix *= glm::lookAt(
        glm::vec3(vW2, vH2, m_eyeZ),
        glm::vec3(vW2, vH2, 0),
        glm::vec3(0,   1,   0));

    m_updateViewProjection = true;
}

void MCGLScene::setSplitType(SplitType splitType)
{
    m_splitType = splitType;

    updateViewport();
}

void MCGLScene::setProjection(MCFloat aspectRatio, MCFloat zNear, MCFloat zFar, MCFloat viewAngle)
{
    m_projectionMatrix  = glm::mat4(1.0);
    m_projectionMatrix *= glm::perspective(viewAngle, aspectRatio, zNear, zFar);

    m_updateViewProjection = true;
}

void MCGLScene::updateViewport()
{
    switch (m_splitType)
    {
    default:
    case ShowFullScreen:
        setProjection(static_cast<float>(m_sceneWidth) / m_sceneHeight, m_zNear, m_zFar, m_viewAngle);
        glViewport(0, 0, m_viewWidth, m_viewHeight);
        glDisable(GL_SCISSOR_TEST);
        setViewerPosition(m_sceneWidth, m_sceneHeight, m_viewAngle);
        break;

    case ShowOnLeft:
        setProjection(static_cast<float>(m_sceneWidth / 2) / m_sceneHeight, m_zNear, m_zFar, m_viewAngle);
        glViewport(0, 0, m_viewWidth / 2, m_viewHeight);
        glScissor(0, 0, m_viewWidth / 2, m_viewHeight);
        glEnable(GL_SCISSOR_TEST);
        setViewerPosition(m_sceneWidth / 2, m_sceneHeight, m_viewAngle);
        break;

    case ShowOnRight:
        setProjection(static_cast<float>(m_sceneWidth / 2) / m_sceneHeight, m_zNear, m_zFar, m_viewAngle);
        glViewport(m_viewWidth / 2, 0, m_viewWidth / 2, m_viewHeight);
        glScissor(m_viewWidth / 2 + 2, 0, m_viewWidth / 2 - 2, m_viewHeight);
        glEnable(GL_SCISSOR_TEST);
        setViewerPosition(m_sceneWidth / 2, m_sceneHeight, m_viewAngle);
        break;

    case ShowOnTop:
        setProjection(static_cast<float>(m_sceneWidth * 2) / m_sceneHeight, m_zNear, m_zFar, m_viewAngle / 2);
        glViewport(0, m_viewHeight / 2, m_viewWidth, m_viewHeight / 2);
        glScissor(0, m_viewHeight / 2 + 2, m_viewWidth - 2, m_viewHeight / 2);
        glEnable(GL_SCISSOR_TEST);
        setViewerPosition(m_sceneWidth, m_sceneHeight / 2, m_viewAngle / 2);
        break;

    case ShowOnBottom:
        setProjection(static_cast<float>(m_sceneWidth * 2) / m_sceneHeight, m_zNear, m_zFar, m_viewAngle / 2);
        glViewport(0, 0, m_viewWidth, m_viewHeight / 2);
        glScissor(0, 0, m_viewWidth, m_viewHeight / 2);
        glEnable(GL_SCISSOR_TEST);
        setViewerPosition(m_sceneWidth, m_sceneHeight / 2, m_viewAngle / 2);
        break;
    }

    updateViewProjectionMatrixAndShaders();
}

const glm::mat4 & MCGLScene::viewProjectionMatrix() const
{
    if (m_updateViewProjection)
    {
        m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
        m_updateViewProjection = false;
    }

    return m_viewProjectionMatrix;
}

void MCGLScene::updateViewProjectionMatrixAndShaders()
{
    for (MCGLShaderProgram * p : m_shaders)
    {
        p->setViewProjectionMatrix(viewProjectionMatrix());
        p->setViewMatrix(m_viewMatrix);
    }
}

void MCGLScene::setAmbientLight(const MCGLAmbientLight & light)
{
    for (MCGLShaderProgram * p : m_shaders)
    {
        p->setAmbientLight(light);
    }
}

void MCGLScene::setDiffuseLight(const MCGLDiffuseLight & light)
{
    for (MCGLShaderProgram * p : m_shaders)
    {
        p->setDiffuseLight(light);
    }
}

void MCGLScene::setSpecularLight(const MCGLDiffuseLight & light)
{
    for (MCGLShaderProgram * p : m_shaders)
    {
        p->setSpecularLight(light);
    }
}

MCFloat MCGLScene::viewAngle() const
{
    return m_viewAngle;
}

MCFloat MCGLScene::eyeZ() const
{
    return m_eyeZ;
}

MCGLScene::~MCGLScene()
{
    MCGLScene::m_instance = nullptr;
}

