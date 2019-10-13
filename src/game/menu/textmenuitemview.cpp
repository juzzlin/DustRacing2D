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

#include "textmenuitemview.hpp"
#include "game.hpp"
#include <MenuItem>

#include <MCAssetManager>
#include <MCRandom>
#include <MCTextureFont>
#include <MCTextureText>

#include <cmath>

TextMenuItemView::TextMenuItemView(float textSize, MTFH::MenuItem & owner)
  : MenuItemView(owner)
  , m_textSize(textSize)
  , m_angle(MCRandom::getValue() * 2.0f * 3.1415f)
{
}

void TextMenuItemView::setTextSize(float size)
{
    m_textSize = size;
}

void TextMenuItemView::stepTime(int)
{
    m_angle += 0.010f;
}

void TextMenuItemView::render(float x, float y)
{
    MCTextureText text(owner().text());

    const float amp = 0.05f;
    float animatedSize = m_textSize + std::sin(m_angle) * m_textSize * amp;
    if (owner().focused())
    {
        animatedSize *= 1.25f;
    }
    text.setGlyphSize(animatedSize, animatedSize);

    if (owner().focused())
    {
        const MCGLColor yellow(1.0f, 1.0f, 0.0f, 1.0f);
        text.setColor(yellow);
    }
    else if (owner().selected())
    {
        const MCGLColor red(1.0f, 0.0f, 0.0f, 1.0f);
        text.setColor(red);
    }
    else
    {
        const MCGLColor white(1.0f, 1.0f, 1.0f, 1.0f);
        text.setColor(white);
    }

    const float shadowY = -2;
    const float shadowX = 2;
    text.setShadowOffset(shadowX, shadowY);

    auto && font = MCAssetManager::textureFontManager().font(Game::instance().fontName());
    text.render(x - text.width(font) / 2, y, nullptr, font);
}

TextMenuItemView::~TextMenuItemView()
{
}
