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

#include "inputhandler.hpp"
#include "menumanager.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "shaderprogram.hpp"
#include "../common/config.hpp"

#include "MiniCore/Core/MCCamera"
#include "MiniCore/Core/MCGLScene"
#include "MiniCore/Core/MCGLShaderProgram"
#include "MiniCore/Core/MCLogger"
#include "MiniCore/Core/MCSurface"
#include "MiniCore/Core/MCTextureManager"
#include "MiniCore/Core/MCTrigonom"
#include "MiniCore/Core/MCWorld"

#include <cmath>
#include <cassert>

#include <QKeyEvent>
#include <QGLShader>
#include <QGLShaderProgram>
#include <GL/glu.h>

Renderer * Renderer::m_instance = nullptr;

Renderer::Renderer(QWidget * parent)
: QGLWidget(parent)
, m_pScene(nullptr)
, m_pGLScene(new MCGLScene)
, m_pCamera(nullptr)
, m_pInputHandler(nullptr)
, m_viewAngle(45.0)
, m_pFadeProgram(nullptr)
, m_pFadeFragmentShader(nullptr)
, m_fadeShaderEnabled(false)
, m_fadeValue(1.0)
, m_tileProgram(nullptr)
, m_masterProgram(nullptr)
, m_masterShadowProgram(nullptr)
, m_textProgram(nullptr)
, m_textShadowProgram(nullptr)
, m_enabled(true)
{
    assert(!Renderer::m_instance);
    Renderer::m_instance = this;
    setFocusPolicy(Qt::StrongFocus);
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
    m_pFadeProgram        = new QGLShaderProgram(context(), this);
    m_pFadeFragmentShader = new QGLShader(QGLShader::Fragment, context(), this);

    // TODO: Error handling
    m_pFadeFragmentShader->compileSourceFile(
        QString(Config::Common::dataPath) + "/shaders/fade.fsh");
    m_pFadeProgram->addShader(m_pFadeFragmentShader);
    m_pFadeProgram->link();

    // TODO: Error handling
    m_tileProgram = new ShaderProgram(context());
    m_tileProgram->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/tile.fsh");
    m_tileProgram->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/tile.vsh");
    m_tileProgram->link();

    // TODO: Error handling
    m_masterProgram = new ShaderProgram(context());
    m_masterProgram->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/master.fsh");
    m_masterProgram->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/master.vsh");
    m_masterProgram->link();

    // TODO: Error handling
    m_masterShadowProgram = new ShaderProgram(context());
    m_masterShadowProgram->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/master2dShadow.fsh");
    m_masterShadowProgram->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/master.vsh");
    m_masterShadowProgram->link();

    // TODO: Error handling
    m_textProgram = new ShaderProgram(context());
    m_textProgram->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/text.fsh");
    m_textProgram->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/text.vsh");
    m_textProgram->link();

    // TODO: Error handling
    m_textShadowProgram = new ShaderProgram(context());
    m_textShadowProgram->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/text2dShadow.fsh");
    m_textShadowProgram->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/text.vsh");
    m_textShadowProgram->link();
}

void Renderer::setEnabled(bool enable)
{
    m_enabled = enable;
}

void Renderer::setFadeShaderEnabled(bool enable)
{
    m_fadeShaderEnabled = enable;
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

MCGLShaderProgram & Renderer::textProgram()
{
    return *m_textProgram;
}

MCGLShaderProgram & Renderer::textShadowProgram()
{
    return *m_textShadowProgram;
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
    if (m_fadeShaderEnabled && m_pFadeProgram)
    {
        m_pFadeProgram->bind();
        m_pFadeProgram->setUniformValue("value", m_fadeValue);
    }

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

    if (m_fadeShaderEnabled && m_pFadeProgram)
    {
        m_pFadeProgram->release();
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
    if (m_pInputHandler && !event->isAutoRepeat())
    {
        switch (event->key())
        {
        case Qt::Key_Left:
            m_pInputHandler->setActionState(0, InputHandler::IA_LEFT, true);
            break;
        case Qt::Key_Right:
            m_pInputHandler->setActionState(0, InputHandler::IA_RIGHT, true);
            break;
        case Qt::Key_Up:
            m_pInputHandler->setActionState(0, InputHandler::IA_UP, true);
            break;
        case Qt::Key_Down:
            m_pInputHandler->setActionState(0, InputHandler::IA_DOWN, true);
            break;
        default:
            QGLWidget::keyPressEvent(event);
            break;
        }
    }
}

void Renderer::keyReleaseEvent(QKeyEvent * event)
{
    if (m_pInputHandler && !event->isAutoRepeat())
    {
        switch (event->key())
        {
        case Qt::Key_Left:
            m_pInputHandler->setActionState(0, InputHandler::IA_LEFT, false);
            MenuManager::instance().left();
            break;
        case Qt::Key_Right:
            m_pInputHandler->setActionState(0, InputHandler::IA_RIGHT, false);
            MenuManager::instance().right();
            break;
        case Qt::Key_Up:
            m_pInputHandler->setActionState(0, InputHandler::IA_UP, false);
            break;
        case Qt::Key_Down:
            m_pInputHandler->setActionState(0, InputHandler::IA_DOWN, false);
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            MenuManager::instance().selectCurrentItem();
            break;
        default:
            QGLWidget::keyReleaseEvent(event);
            break;
        }
    }
}

void Renderer::setScene(Scene * pScene)
{
    m_pScene = pScene;
}

void Renderer::setInputHandler(InputHandler * pInputHandler)
{
    m_pInputHandler = pInputHandler;
}

Renderer::~Renderer()
{
    delete m_tileProgram;
    delete m_masterProgram;
    delete m_masterShadowProgram;
    delete m_textProgram;
    delete m_textShadowProgram;
    delete m_pGLScene;
}
