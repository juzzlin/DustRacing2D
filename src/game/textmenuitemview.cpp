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
#include <MCTextureFont>
#include <MCTextureText>

TextMenuItemView::TextMenuItemView(int textSize, MTFH::MenuItem & owner)
: MenuItemView(owner)
, m_textSize(textSize)
{
}

void TextMenuItemView::setTextSize(int size)
{
    m_textSize = size;
}

void TextMenuItemView::render(int x, int y)
{
    MCTextureText text(owner().text());

    const int shadowY = -2;
    const int shadowX =  2;

    text.setGlyphSize(m_textSize, m_textSize);

    if (owner().focused())
    {
        const MCGLColor yellow(1.0, 1.0, 0.0, 1.0);
        text.setColor(yellow);
    }
    else if (owner().selected())
    {
        const MCGLColor red(1.0, 0.0, 0.0, 1.0);
        text.setColor(red);
    }
    else
    {
        const MCGLColor white(1.0, 1.0, 1.0, 1.0);
        text.setColor(white);
    }

    text.setShadowOffset(shadowX, shadowY);

    auto && font = MCAssetManager::textureFontManager().font(Game::instance().fontName());
    text.render(x - text.width(font) / 2 + 20, y, nullptr, font);
}

TextMenuItemView::~TextMenuItemView()
{
}
