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

#include "mcglscene.hh"
#include "mctrigonom.hh"

#include <GL/glu.h>
#include <cmath>

MCGLScene::MCGLScene()
{
}

void MCGLScene::initialize()
{
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
}

void MCGLScene::resize(
    MCUint viewWidth, MCUint viewHeight,
    MCUint sceneWidth, MCUint sceneHeight, MCFloat viewAngle)
{
    if (viewHeight == 0)
    {
        viewHeight = 1;
    }

    glViewport(0, 0, viewWidth, viewHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    const float zNear = 1.0;
    const float zFar  = 1000.0;

    gluPerspective(viewAngle, static_cast<GLfloat>(sceneWidth) /
        sceneHeight, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    setViewerPosition(sceneWidth, sceneHeight, viewAngle);
}

void MCGLScene::setViewerPosition(MCUint sceneWidth, MCUint sceneHeight, MCFloat viewAngle)
{
    // Set eye position so that the scene looks like a pure 2D-scene
    const MCUint  vH2  = sceneHeight / 2;
    const MCUint  vW2  = sceneWidth  / 2;
    const MCFloat eyeZ = vH2 /
        std::tan(static_cast<MCFloat>(MCTrigonom::degToRad(viewAngle / 2)));
    gluLookAt(vW2, vH2, eyeZ, vW2, vH2, 0, 0, 1, 0);
}

MCGLScene::~MCGLScene()
{
}

