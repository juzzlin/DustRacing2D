// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#include "renderer.hpp"

#include "eventhandler.hpp"
#include "scene.hpp"
#include "shaderprogram.hpp"
#include "shaders.h"
#include "../common/config.hpp"

#include <MCGLScene>
#include <MCGLShaderProgram>
#include <MCException>
#include <MCLogger>
#include <MCSurface>
#include <MCSurfaceManager>
#include <MCTrigonom>
#include <MCWorld>

#include <cmath>
#include <cassert>

#include <QApplication>
#include <QIcon>
#include <QDesktopWidget>
#include <QGLFramebufferObject>
#include <QGLShader>
#include <QGLShaderProgram>
#include <QKeyEvent>

Renderer * Renderer::m_instance = nullptr;

Renderer::Renderer(
    const QGLFormat & qglFormat,
    int hRes,
    int vRes,
    bool nativeResolution,
    bool fullScreen,
    QWidget * parent)
: QGLWidget(qglFormat, parent)
, m_scene(nullptr)
, m_glScene(new MCGLScene)
, m_eventHandler(nullptr)
, m_viewAngle(45.0)
, m_fadeValue(1.0)
, m_enabled(false)
, m_hRes(hRes)
, m_vRes(vRes)
, m_nativeResolution(nativeResolution)
, m_fullScreen(fullScreen)
{
    assert(!Renderer::m_instance);
    Renderer::m_instance = this;
    setFocusPolicy(Qt::StrongFocus);

    setWindowTitle(QString(Config::Game::GAME_NAME) + " " + Config::Game::GAME_VERSION);
    setWindowIcon(QIcon(":/dustrac-game.png"));

    if (!fullScreen)
    {
        // Set window size & disable resize
        resize(hRes, vRes);
        setMinimumSize(hRes, vRes);
        setMaximumSize(hRes, vRes);

        // Try to center the window.
        QRect geometry(QApplication::desktop()->availableGeometry());
        move(geometry.width() / 2 - width() / 2, geometry.height() / 2 - height() / 2);
    }
}

Renderer & Renderer::instance()
{
    assert(Renderer::m_instance);
    return *Renderer::m_instance;
}

void Renderer::initializeGL()
{
    MCLogger().info() << "OpenGL Version: " << glGetString(GL_VERSION);

    m_glScene->initialize();

    if (QGLShader::hasOpenGLShaders(QGLShader::Fragment, context()))
    {
        loadShaders();
    }
}

void Renderer::resizeGL(int viewWidth, int viewHeight)
{
    m_glScene->resize(
        viewWidth, viewHeight, Scene::width(), Scene::height(),
        m_viewAngle);
}

void Renderer::createProgramFromFile(
    const std::string & handle, const std::string & fshPath, const std::string & vshPath)
{
    // Note: ShaderProgram throws on error.

    MCGLShaderProgram * program = new ShaderProgram(context(), *m_glScene);
    program->addFragmentShaderFromFile(
        std::string(Config::Common::dataPath) + "/shaders/" + fshPath);
    program->addVertexShaderFromFile(
        std::string(Config::Common::dataPath) + "/shaders/" + vshPath);
    program->link();
    m_shaderHash[handle].reset(program);
}

void Renderer::createProgramFromSource(
    const std::string & handle, const std::string & fshSource, const std::string & vshSource)
{
    // Note: ShaderProgram throws on error.

    MCGLShaderProgram * program = new ShaderProgram(context(), *m_glScene);
    program->addFragmentShaderFromSource(fshSource);
    program->addVertexShaderFromSource(vshSource);
    program->link();
    m_shaderHash[handle].reset(program);
}

void Renderer::loadShaders()
{
    createProgramFromSource("car",           carFshDesktop,              carVshDesktop);
    createProgramFromSource("fbo",           fboFshDesktop,              fboVshDesktop);
    createProgramFromSource("master",        masterFshDesktop,           masterVshDesktop);
    createProgramFromSource("masterShadow",  masterShadowFshDesktop,     masterShadowVshDesktop);
    createProgramFromSource("menu",          menuFshDesktop,             menuVshDesktop);
    createProgramFromSource("particle",      particleFshDesktop,         masterVshDesktop);
    createProgramFromSource("pointParticle", pointParticleFshDesktop,    pointParticleVshDesktop);
    createProgramFromSource("pointParticleDiscard", pointParticleDiscardFshDesktop, pointParticleVshDesktop);
    createProgramFromSource("text",          textFshDesktop,             textVshDesktop);
    createProgramFromSource("textShadow",    textShadowFshDesktop,       textVshDesktop);
    createProgramFromSource("tile2d",        tile2dFshDesktop,           tileVshDesktop);
    createProgramFromSource("tile3d",        tile3dFshDesktop,           tileVshDesktop);

    // Make sure that shaders have the current model view projection matrix.
    m_glScene->updateModelViewProjectionMatrixAndShaders();
}

void Renderer::setEnabled(bool enable)
{
    m_enabled = enable;
}

MCGLShaderProgram & Renderer::program(const std::string & id)
{
    MCGLShaderProgramPtr program = m_shaderHash[id];
    if (!program.get())
    {
        throw MCException("Cannot find shader program '" + id +"'");
    }
    return *program;
}

MCGLScene & Renderer::glScene()
{
    return *m_glScene;
}

void Renderer::setFadeValue(float value)
{
    m_fadeValue = value;
}

float Renderer::fadeValue() const
{
    return m_fadeValue;
}

void Renderer::renderNativeResolutionOrWindowed()
{
    if (m_enabled)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_scene)
        {
            m_scene->render();
        }
    }
}

void Renderer::renderCustomResolution()
{
    // Render the game scene to the frame buffer object
    if (m_enabled)
    {
        resizeGL(m_hRes, m_vRes);

        static QGLFramebufferObject fbo(m_hRes, m_vRes);

        fbo.bind();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_scene)
        {
            m_scene->render();
        }

        fbo.release();

        // Render the frame buffer object onto the screen

        const int fullVRes = QApplication::desktop()->height();
        const int fullHRes = QApplication::desktop()->width();

        resizeGL(fullHRes, fullVRes);

        MCSurface sd(fbo.texture(), 0, 0, Scene::width(), Scene::height());
        sd.setShaderProgram(&program("fbo"));
        sd.bindTexture();
        sd.render(nullptr, MCVector3dF(Scene::width() / 2, Scene::height() / 2, 0), 0);
    }
}

void Renderer::paintGL()
{
    if (m_enabled)
    {
        if (!m_fullScreen || m_nativeResolution)
        {
            renderNativeResolutionOrWindowed();
        }
        else
        {
            renderCustomResolution();
        }

        swapBuffers();
    }
}

void Renderer::updateFrame()
{
    paintGL();
}

void Renderer::keyPressEvent(QKeyEvent * event)
{
    assert(m_eventHandler);
    if (!m_eventHandler->handleKeyPressEvent(event))
    {
        QGLWidget::keyPressEvent(event);
    }
}

void Renderer::keyReleaseEvent(QKeyEvent * event)
{
    assert(m_eventHandler);
    if (!m_eventHandler->handleKeyReleaseEvent(event))
    {
        QGLWidget::keyReleaseEvent(event);
    }
}

void Renderer::mousePressEvent(QMouseEvent * event)
{
    assert(m_eventHandler);
    m_eventHandler->handleMousePressEvent(event, width(), height(), true);
}

void Renderer::mouseReleaseEvent(QMouseEvent * event)
{
    assert(m_eventHandler);
    m_eventHandler->handleMouseReleaseEvent(event, width(), height(), true);
}

void Renderer::closeEvent(QCloseEvent * event)
{
    event->accept();
    emit closed();
}

void Renderer::setScene(Scene & scene)
{
    m_scene = &scene;
}

void Renderer::setEventHandler(EventHandler & eventHandler)
{
    m_eventHandler = &eventHandler;
}

Renderer::~Renderer()
{
    delete m_glScene;
}
