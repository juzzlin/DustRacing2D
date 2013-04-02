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
#include <MCAssetManager>

Intro::Intro()
: m_back(MCAssetManager::surfaceManager().surface("intro"))
{
    m_back.setShaderProgram(&Renderer::instance().program("text"));
    m_back.setColor(MCGLColor(0.9, 0.9, 0.9, 1.0));
}

void Intro::setFadeValue(float value)
{
    m_back.shaderProgram()->setFadeValue(value);
}

void Intro::setDimensions(int width, int height)
{
    OverlayBase::setDimensions(width, height);
}

void Intro::render()
{
    const int w2 = width()  / 2;
    const int h2 = height() / 2;
    m_back.setScale(width(), height());
    m_back.render(nullptr, MCVector3dF(w2, h2, 0), 0);
}
