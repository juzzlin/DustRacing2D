// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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
#include "game.hpp"
#include "renderer.hpp"

#include <MCAssetManager>
#include <MCGLShaderProgram>
#include <MCSurface>
#include <MCTextureText>

Intro::Intro()
  : m_back(MCAssetManager::surfaceManager().surface("intro"))
  , m_font(MCAssetManager::textureFontManager().font(Game::instance().fontName()))
{
    m_back->setShaderProgram(Renderer::instance().program("menu"));
    m_back->setColor(MCGLColor(1.0f, 1.0f, 1.0f, 1.0f));
}

void Intro::setDimensions(size_t width, size_t height)
{
    OverlayBase::setDimensions(width, height);
}

void Intro::render()
{
    const int w2 = width() / 2;
    const int h2 = height() / 2;
    assert(w2 > 0 && h2 > 0);
    m_back->bind();
    m_back->setSize(width(), width() * m_back->height() / m_back->width());
    m_back->render(nullptr, { static_cast<float>(w2), static_cast<float>(h2), 0 }, 0);

    static auto version = QString("v") + VERSION;
    static MCTextureText versionText(version.toStdWString());
    versionText.setGlyphSize(20, height() / 32);
    versionText.setColor(MCGLColor(0.75f, 0.75f, 0.75f));
    versionText.render(
      versionText.height(m_font),
      versionText.height(m_font), nullptr, m_font, false);
}
