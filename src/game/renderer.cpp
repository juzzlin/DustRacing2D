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

#ifdef __MC_GL30__
#include "shaders30.h"
#else
#include "shaders.h" // These are also for GLES now
#endif

#include "../common/config.hpp"

#include <MCGLScene>
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

    setWindowTitle(QString(Config::Game::GAME_NAME) + " " + Config::Game::GAME_VERSION);
    setWindowIcon(QIcon(":/dustrac-game.png"));
    setMouseTracking(true);

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

    loadShaders();
}

void Renderer::resizeGL(int viewWidth, int viewHeight)
{
    m_glScene->resize(
        viewWidth, viewHeight, Scene::width(), Scene::height(),
        m_viewAngle);
}

void Renderer::createProgramFromSource(
    const std::string & handle, std::string vshSource, std::string fshSource)
{
    // Inject precision qualifiers
#ifdef __MC_GLES__
    QString origVsh(vshSource.c_str());
    origVsh.replace("#version 120", "#version 100\nprecision mediump float;\nprecision mediump int;\n");
    vshSource = origVsh.toStdString();

    QString origFsh(fshSource.c_str());
    origFsh.replace("#version 120", "#version 100\nprecision mediump float;\nprecision mediump int;\n");
    fshSource = origFsh.toStdString();
#endif

    // Note: ShaderProgram throws on error.
    MCGLShaderProgram * program = new MCGLShaderProgram(vshSource, fshSource);
    m_shaderHash[handle].reset(program);
}

void Renderer::loadShaders()
{
    // Engine defaults
    m_shaderHash["default"] =  MCGLScene::instance().defaultShaderProgram();
    m_shaderHash["defaultSpecular"] = MCGLScene::instance().defaultSpecularShaderProgram();
    m_shaderHash["defaultShadow"] = MCGLScene::instance().defaultShadowShaderProgram();

    // Custom shaders
    createProgramFromSource("car",           carVsh,              carFsh);
    createProgramFromSource("fbo",           fboVsh,              fboFsh);
    createProgramFromSource("menu",          menuVsh,             MCGLShaderProgram::getDefaultFragmentShaderSource());
    createProgramFromSource("particle",      MCGLShaderProgram::getDefaultVertexShaderSource(), particleFsh);
    createProgramFromSource("pointParticle", pointParticleVsh,    pointParticleFsh);
    createProgramFromSource("pointParticleRotate", pointParticleVsh, pointParticleRotateFsh);
    createProgramFromSource("text",          textVsh,             MCGLShaderProgram::getDefaultFragmentShaderSource());
    createProgramFromSource("textShadow",    textVsh,             textShadowFsh);
    createProgramFromSource("tile2d",        tileVsh,             MCGLShaderProgram::getDefaultFragmentShaderSource());
    createProgramFromSource("tile3d",        tileVsh,             tile3dFsh);
}

void Renderer::setEnabled(bool enable)
{
    m_enabled = enable;
}

MCGLShaderProgram & Renderer::program(const std::string & id)
{
    MCGLShaderProgramPtr program(m_shaderHash[id]);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_scene)
    {
        m_scene->render();
    }
}

void Renderer::renderCustomResolution()
{
    // Render the game scene to the frame buffer object
    resizeGL(m_hRes, m_vRes);

    static QGLFramebufferObject fbo(m_hRes, m_vRes);
    static MCGLMaterialPtr dummyMaterial(new MCGLMaterial);

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

    dummyMaterial->setTexture(fbo.texture(), 0);
    MCSurface sd(dummyMaterial, Scene::width(), Scene::height());
    sd.setShaderProgram(&program("fbo"));
    sd.bindMaterial();
    sd.render(nullptr, MCVector3dF(Scene::width() / 2, Scene::height() / 2, 0), 0);
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
    }
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

void Renderer::mouseMoveEvent(QMouseEvent * event)
{
    assert(m_eventHandler);
    m_eventHandler->handleMouseMoveEvent(event);
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
