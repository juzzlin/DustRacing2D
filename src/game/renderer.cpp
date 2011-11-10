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

#include "inputhandler.h"
#include "renderer.h"
#include "scene.h"
#include "track.h"

#include "MiniCore/Core/MCCamera"
#include "MiniCore/Core/MCTrigonom"
#include "MiniCore/Core/MCWorld"
#include <cmath>

#include <QKeyEvent>

Renderer::Renderer(QWidget * parent)
: QGLWidget(parent)
, m_pScene(nullptr)
, m_pCamera(nullptr)
, m_pInputHandler(nullptr)
, m_viewAngle(135.0f)
{
    setFocusPolicy(Qt::StrongFocus);
}

void Renderer::initializeGL()
{
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void Renderer::resizeGL(int viewWidth, int viewHeight)
{
    if (viewHeight == 0)
    {
        viewHeight = 1;
    }

    glViewport(0, 0, viewWidth, viewHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    const float zNear = 0.0f;
    const float zFar  = 1000.0f;

    gluPerspective(m_viewAngle, static_cast<GLfloat>(viewWidth) /
        viewHeight, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    setViewerPosition(viewWidth, viewHeight);
}

void Renderer::setViewerPosition(int viewWidth, int viewHeight) const
{
    // Set eye position so that the scene looks like a pure 2D-scene
    const float eyeZ = viewHeight / 2 /
        std::tan(static_cast<MCFloat>(MCTrigonom::degToRad(m_viewAngle / 2)));
    gluLookAt(viewWidth / 2, viewHeight / 2, eyeZ,
              viewWidth / 2, viewHeight / 2, 0,
              0, 1, 0);
}

void Renderer::paintGL()
{
    if (m_pCamera) // Qt might update the widget before camera is set
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_pScene)
        {
             if (m_pScene->activeTrack())
             {
                m_pScene->activeTrack()->render(m_pCamera);
             }

             if (m_pScene->world())
             {
                 m_pScene->world()->render(m_pCamera);
             }
        }
    }

    swapBuffers();
}

void Renderer::updateFrame(MCCamera * pCamera)
{
    m_pCamera = pCamera;

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
}
