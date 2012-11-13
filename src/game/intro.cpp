// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "intro.hpp"
#include "renderer.hpp"
#include "shaderprogram.hpp"

#include <MCSurface>
#include <MCSurfaceManager>
#include <MCVectorAnimation>

static MCVector3dF       fade;
static MCVectorAnimation fadeAnim;

Intro::Intro()
: m_back(MCSurfaceManager::instance().surface("intro"))
{
    m_back.setShaderProgram(&Renderer::instance().program("text"));
    m_back.setColor(.9, .9, .9, 1.0);
}

void Intro::setFadeValue(float value)
{
    m_back.shaderProgram()->setFadeValue(value);
}

void Intro::setDimensions(int width, int height)
{
    OverlayBase::setDimensions(width, height);

    static MCVector3dF fade0(0.0, 0.0, 0.0);
    static MCVector3dF fade1(1.0, 1.0, 1.0);

    fadeAnim.init(fade, fade0, fade1, 120, 0, 120);
}

bool Intro::update()
{
    Renderer::instance().setEnabled(true);
    return fadeAnim.update();
}

void Intro::render()
{
    const int w2 = width()  / 2;
    const int h2 = height() / 2;

    m_back.shaderProgram()->setFadeValue(fade.i());
    m_back.renderScaled(nullptr, MCVector3dF(w2, h2, 0), w2, h2, 0);
}

