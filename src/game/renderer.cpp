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
#include "../common/config.hpp"

#include <MCCamera>
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

    MCGLShaderProgram * program = nullptr;
    program = new ShaderProgram(context());
    program->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/tile2d.fsh");
    program->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/tile.vsh");
    program->link();
    m_shaderHash["tile2d"].reset(program);

    program = new ShaderProgram(context());
    program->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/tile3d.fsh");
    program->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/tile.vsh");
    program->link();
    m_shaderHash["tile3d"].reset(program);

    program = new ShaderProgram(context());
    program->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/menu.fsh");
    program->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/menu.vsh");
    program->link();
    m_shaderHash["menu"].reset(program);

    program = new ShaderProgram(context());
    program->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/master.fsh");
    program->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/master.vsh");
    program->link();
    m_shaderHash["master"].reset(program);

    program = new ShaderProgram(context());
    program->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/master2dShadow.fsh");
    program->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/master2dShadow.vsh");
    program->link();
    m_shaderHash["masterShadow"].reset(program);

    program = new ShaderProgram(context());
    program->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/text.fsh");
    program->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/text.vsh");
    program->link();
    m_shaderHash["text"].reset(program);

    program = new ShaderProgram(context());
    program->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/text2dShadow.fsh");
    program->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/text.vsh");
    program->link();
    m_shaderHash["textShadow"].reset(program);

    program = new ShaderProgram(context());
    program->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/particle.fsh");
    program->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/particle.vsh");
    program->link();
    m_shaderHash["particle"].reset(program);

    program = new ShaderProgram(context());
    program->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/particle.fsh");
    program->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/pointParticle.vsh");
    program->link();
    m_shaderHash["pointParticle"].reset(program);
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
    delete m_pGLScene;
}
