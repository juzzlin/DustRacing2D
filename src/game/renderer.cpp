// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#include "renderer.hpp"

#include "eventhandler.hpp"
#include "scene.hpp"
#include "shaderprogram.hpp"
#include "../common/config.hpp"

#include <MCCamera>
#include <MCGLScene>
#include <MCGLShaderProgram>
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
#include <QGLShader>
#include <QGLShaderProgram>
#include <QKeyEvent>

#include <GL/glu.h>

Renderer * Renderer::m_instance = nullptr;

Renderer::Renderer(int hRes, int vRes, bool fullScreen, QWidget * parent)
: QGLWidget(parent)
, m_pScene(nullptr)
, m_pGLScene(new MCGLScene)
, m_pCamera(nullptr)
, m_eventHandler(nullptr)
, m_viewAngle(45.0)
, m_fadeValue(1.0)
, m_tileProgram(nullptr)
, m_masterProgram(nullptr)
, m_masterShadowProgram(nullptr)
, m_menuProgram(nullptr)
, m_textProgram(nullptr)
, m_textShadowProgram(nullptr)
, m_particleProgram(nullptr)
, m_pointParticleProgram(nullptr)
, m_enabled(true)
{
    assert(!Renderer::m_instance);
    Renderer::m_instance = this;
    setFocusPolicy(Qt::StrongFocus);

    setWindowTitle(QString(Config::Game::GAME_NAME) + " " + Config::Game::GAME_VERSION);
    setWindowIcon(QIcon(":/logo.png"));

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

    m_pGLScene->initialize();

    if (QGLShader::hasOpenGLShaders(QGLShader::Fragment, context()))
    {
        loadShaders();
    }
}

void Renderer::resizeGL(int viewWidth, int viewHeight)
{
    m_pGLScene->resize(
        viewWidth, viewHeight, Scene::width(), Scene::height(),
        m_viewAngle);
}

void Renderer::loadShaders()
{
    // Note: ShaderProgram throws on error.

    m_tileProgram = new ShaderProgram(context());
    m_tileProgram->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/tile.fsh");
    m_tileProgram->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/tile.vsh");
    m_tileProgram->link();

    m_menuProgram = new ShaderProgram(context());
    m_menuProgram->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/menu.fsh");
    m_menuProgram->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/menu.vsh");
    m_menuProgram->link();

    m_masterProgram = new ShaderProgram(context());
    m_masterProgram->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/master.fsh");
    m_masterProgram->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/master.vsh");
    m_masterProgram->link();

    m_masterShadowProgram = new ShaderProgram(context());
    m_masterShadowProgram->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/master2dShadow.fsh");
    m_masterShadowProgram->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/master2dShadow.vsh");
    m_masterShadowProgram->link();

    m_textProgram = new ShaderProgram(context());
    m_textProgram->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/text.fsh");
    m_textProgram->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/text.vsh");
    m_textProgram->link();

    m_textShadowProgram = new ShaderProgram(context());
    m_textShadowProgram->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/text2dShadow.fsh");
    m_textShadowProgram->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/text.vsh");
    m_textShadowProgram->link();

    m_particleProgram = new ShaderProgram(context());
    m_particleProgram->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/particle.fsh");
    m_particleProgram->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/particle.vsh");
    m_particleProgram->link();

    m_pointParticleProgram = new ShaderProgram(context());
    m_pointParticleProgram->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/particle.fsh");
    m_pointParticleProgram->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/pointParticle.vsh");
    m_pointParticleProgram->link();
}

void Renderer::setEnabled(bool enable)
{
    m_enabled = enable;
}

MCGLShaderProgram & Renderer::tileProgram()
{
    return *m_tileProgram;
}

MCGLShaderProgram & Renderer::masterProgram()
{
    return *m_masterProgram;
}

MCGLShaderProgram & Renderer::masterShadowProgram()
{
    return *m_masterShadowProgram;
}

MCGLShaderProgram & Renderer::menuProgram()
{
    return *m_menuProgram;
}

MCGLShaderProgram & Renderer::textProgram()
{
    return *m_textProgram;
}

MCGLShaderProgram & Renderer::textShadowProgram()
{
    return *m_textShadowProgram;
}

MCGLShaderProgram & Renderer::particleProgram()
{
    return *m_particleProgram;
}

MCGLShaderProgram & Renderer::pointParticleProgram()
{
    return *m_pointParticleProgram;
}

void Renderer::setFadeValue(float value)
{
    m_fadeValue = value;
}

float Renderer::fadeValue() const
{
    return m_fadeValue;
}

void Renderer::paintGL()
{
    if (m_enabled)
    {
        if (m_pCamera) // Qt might update the widget before camera is set
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (m_pScene)
            {
                m_pScene->render(*m_pCamera);
            }
        }
    }

    swapBuffers();
}

void Renderer::updateFrame(MCCamera & camera)
{
    m_pCamera = &camera;

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

void Renderer::closeEvent(QCloseEvent * event)
{
    event->accept();
    emit closed();
}

void Renderer::setScene(Scene & scene)
{
    m_pScene = &scene;
}

void Renderer::setEventHandler(EventHandler & eventHandler)
{
    m_eventHandler = &eventHandler;
}

Renderer::~Renderer()
{
    delete m_tileProgram;
    delete m_masterProgram;
    delete m_masterShadowProgram;
    delete m_menuProgram;
    delete m_textProgram;
    delete m_textShadowProgram;
    delete m_particleProgram;
    delete m_pointParticleProgram;
    delete m_pGLScene;
}
