// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "intro.hpp"
#include "renderer.hpp"
#include "shaderprogram.hpp"

#include <MCSurface>
#include <MCTextureFont>
#include <MCTextureFontManager>
#include <MCTextureManager>
#include <MCTextureText>

#include <MCVectorAnimation>

static MCVector3dF       fade;
static MCVectorAnimation fadeAnim;

static MCTextureText text1("DUST RACING 2D");

Intro::Intro()
: m_back(MCTextureManager::instance().surface("dustRacing"))
, m_monospace(MCTextureFontManager::instance().font("default"))
{
    m_back.setShaderProgram(&Renderer::instance().masterProgram());
    m_back.setColor(.9, .9, .9, 1.0);
}

void Intro::setFadeValue(float value)
{
    m_back.shaderProgram()->setFadeValue(value);
}

void Intro::setDimensions(int width, int height)
{
    OverlayBase::setDimensions(width, height);

    const int shadowY = -2;
    const int shadowX =  2;

    text1.setGlyphSize(40, 40);
    text1.setColor(1.0, 1.0, 1.0, 1.0);
    text1.setShadowOffset(shadowX, shadowY);

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

    Renderer::instance().masterProgram().setFadeValue(fade.i());
    Renderer::instance().textProgram().setFadeValue(fade.i());

    m_back.renderScaled(nullptr, MCVector3dF(w2, h2, 0), w2, h2, 0);

    text1.render(width() / 2 - text1.width() / 2 + 20, height() / 2 + 40, nullptr, m_monospace);
}

