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

#include <MCGLEW>
#include <MCLogger>

#include "mcglscene.hh"
#include "mcglambientlight.hh"
#include "mcgldiffuselight.hh"
#include "mcglshaderprogram.hh"
#include "mctrigonom.hh"

#include <algorithm>
#include <cmath>

MCGLScene::MCGLScene()
: m_splitType(ShowFullScreen)
, m_viewWidth(0)
, m_viewHeight(0)
, m_sceneWidth(0)
, m_sceneHeight(0)
, m_viewAngle(0)
, m_updateViewProjection(false)
{
}

void MCGLScene::addShaderProgram(MCGLShaderProgram & shader)
{
    if (std::find(m_shaders.begin(), m_shaders.end(), &shader) == m_shaders.end())
    {
        m_shaders.push_back(&shader);

        // Ensure current projection
        shader.setViewProjectionMatrix(viewProjectionMatrix());
    }
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
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_PROGRAM_POINT_SIZE);
#ifndef __MC_GLES__
    glClearDepth(1.0);
#else
    glClearDepthf(1.0);
#endif
}

void MCGLScene::resize(
    MCUint viewWidth, MCUint viewHeight, MCUint sceneWidth, MCUint sceneHeight, MCFloat viewAngle)
{
    m_viewWidth   = viewWidth;
    m_viewHeight  = viewHeight;
    m_sceneWidth  = sceneWidth;
    m_sceneHeight = sceneHeight;
    m_viewAngle   = viewAngle;

    updateViewport();
}

void MCGLScene::setViewerPosition(MCUint sceneWidth, MCUint sceneHeight, MCFloat viewAngle)
{
    // Set eye position so that the scene looks like a pure 2D-scene
    const MCUint  vH2  = sceneHeight / 2;
    const MCUint  vW2  = sceneWidth  / 2;
    const MCFloat eyeZ = vH2 /
        std::tan(static_cast<MCFloat>(MCTrigonom::degToRad(viewAngle / 2)));

    m_viewMatrix  = glm::mat4(1.0);
    m_viewMatrix *= glm::lookAt(
        glm::vec3(vW2, vH2, eyeZ),
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
    static const float zNear = 1.0;
    static const float zFar  = 1000.0;

    switch (m_splitType)
    {
    default:
    case ShowFullScreen:
        setProjection(static_cast<float>(m_sceneWidth) / m_sceneHeight, zNear, zFar, m_viewAngle);
        glViewport(0, 0, m_viewWidth, m_viewHeight);
        glDisable(GL_SCISSOR_TEST);
        setViewerPosition(m_sceneWidth, m_sceneHeight, m_viewAngle);
        break;

    case ShowOnLeft:
        setProjection(static_cast<float>(m_sceneWidth / 2) / m_sceneHeight, zNear, zFar, m_viewAngle);
        glViewport(0, 0, m_viewWidth / 2, m_viewHeight);
        glScissor(0, 0, m_viewWidth / 2, m_viewHeight);
        glEnable(GL_SCISSOR_TEST);
        setViewerPosition(m_sceneWidth / 2, m_sceneHeight, m_viewAngle);
        break;

    case ShowOnRight:
        setProjection(static_cast<float>(m_sceneWidth / 2) / m_sceneHeight, zNear, zFar, m_viewAngle);
        glViewport(m_viewWidth / 2, 0, m_viewWidth / 2, m_viewHeight);
        glScissor(m_viewWidth / 2, 0, m_viewWidth / 2, m_viewHeight);
        glEnable(GL_SCISSOR_TEST);
        setViewerPosition(m_sceneWidth / 2, m_sceneHeight, m_viewAngle);
        break;

    case ShowOnTop:
        setProjection(static_cast<float>(m_sceneWidth * 2) / m_sceneHeight, zNear, zFar, m_viewAngle / 2);
        glViewport(0, m_viewHeight / 2, m_viewWidth, m_viewHeight / 2);
        glScissor(0, m_viewHeight / 2, m_viewWidth, m_viewHeight / 2);
        glEnable(GL_SCISSOR_TEST);
        setViewerPosition(m_sceneWidth, m_sceneHeight / 2, m_viewAngle / 2);
        break;

    case ShowOnBottom:
        setProjection(static_cast<float>(m_sceneWidth * 2) / m_sceneHeight, zNear, zFar, m_viewAngle / 2);
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
    }
}

void MCGLScene::setFadeValue(MCFloat value)
{
    for (MCGLShaderProgram * p : m_shaders)
    {
        p->setFadeValue(value);
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

MCGLScene::~MCGLScene()
{
}

