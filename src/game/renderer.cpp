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

#include "renderer.h"
#include "scene.h"
#include "track.h"

#include "MiniCore/Core/MCCamera"
#include "MiniCore/Core/MCTrigonom"
#include <cmath>

Renderer::Renderer(QWidget * parent)
: QGLWidget(parent)
, m_pScene(nullptr)
, m_pCamera(nullptr)
{
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

void Renderer::resizeGL(int width, int height)
{
    if (height == 0)
    {
        height = 1;
    }

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    const float viewAngle = 135.0f;
    gluPerspective(viewAngle, static_cast<GLfloat>(width) / height, 0.1f, 1000.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Set eye position so that the scene looks like a pure 2D-scene
    const int sceneH = width;
    const int sceneW = height;
    const float eyeZ = sceneH / 2 /
            std::tan(static_cast<MCFloat>(MCTrigonom::degToRad(viewAngle / 2)));
    gluLookAt(sceneW / 2, sceneH / 2, eyeZ,
              sceneW / 2, sceneH / 2, 0,
              0, 1, 0);
}

void Renderer::paintGL()
{
    if (m_pCamera) // Qt might update the widget before camera is set
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (m_pScene && m_pScene->activeTrack())
        {
            m_pScene->activeTrack()->render(m_pCamera);
        }
    }

    QGLWidget::swapBuffers();
}

void Renderer::updateFrame(MCCamera * pCamera)
{
    m_pCamera = pCamera;

    updateGL();
}

void Renderer::setScene(Scene * scene)
{
    m_pScene = scene;
}

Renderer::~Renderer()
{
}
