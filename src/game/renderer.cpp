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
#include "statemachine.hpp"
#include "../common/config.hpp"

#include <MCCamera>
#include <MCGLScene>
#include <MCGLShaderProgram>
#include <MCLogger>
#include <MCSurface>
#include <MCTextureManager>
#include <MCTrigonom>
#include <MCWorld>

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
, m_fadeValue(1.0)
, m_tileProgram(nullptr)
, m_masterProgram(nullptr)
, m_masterShadowProgram(nullptr)
, m_textProgram(nullptr)
, m_textShadowProgram(nullptr)
, m_particleProgram(nullptr)
, m_pointParticleProgram(nullptr)
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

    // TODO: Error handling
    m_particleProgram = new ShaderProgram(context());
    m_particleProgram->addFragmentShader(
        std::string(Config::Common::dataPath) + "/shaders/particle.fsh");
    m_particleProgram->addVertexShader(
        std::string(Config::Common::dataPath) + "/shaders/particle.vsh");
    m_particleProgram->link();

    // TODO: Error handling
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
    if (StateMachine::instance().state() != StateMachine::Menu)
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
            case Qt::Key_P:
                emit pauseToggled();
                break;
            default:
                QGLWidget::keyPressEvent(event);
                break;
            }
        }
    }
    else
    {
        switch (event->key())
        {
        case Qt::Key_Left:
            MenuManager::instance().left();
            break;
        case Qt::Key_Right:
            MenuManager::instance().right();
            break;
        case Qt::Key_Up:
            MenuManager::instance().up();
            break;
        case Qt::Key_Down:
            MenuManager::instance().down();
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            MenuManager::instance().selectCurrentItem();
            break;
        case Qt::Key_Escape:
        case Qt::Key_Q:
            MenuManager::instance().exitCurrentMenu();
            if (MenuManager::instance().done())
            {
                QApplication::instance()->quit();
            }
            break;
        default:
            QGLWidget::keyReleaseEvent(event);
            break;
        }
    }
}

void Renderer::keyReleaseEvent(QKeyEvent * event)
{
    if (StateMachine::instance().state() != StateMachine::Menu)
    {
        if (m_pInputHandler && !event->isAutoRepeat())
        {
            switch (event->key())
            {
            case Qt::Key_Left:
                m_pInputHandler->setActionState(0, InputHandler::IA_LEFT, false);
                break;
            case Qt::Key_Right:
                m_pInputHandler->setActionState(0, InputHandler::IA_RIGHT, false);
                break;
            case Qt::Key_Up:
                m_pInputHandler->setActionState(0, InputHandler::IA_UP, false);
                break;
            case Qt::Key_Down:
                m_pInputHandler->setActionState(0, InputHandler::IA_DOWN, false);
                break;
            case Qt::Key_Escape:
            case Qt::Key_Q:
                StateMachine::instance().quit();
                break;
            default:
                QGLWidget::keyReleaseEvent(event);
                break;
            }
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
    delete m_particleProgram;
    delete m_pointParticleProgram;
    delete m_pGLScene;
}
