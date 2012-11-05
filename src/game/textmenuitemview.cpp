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
#include <MenuItem>

#include <MCTextureFont>
#include <MCTextureFontManager>
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
    MCTextureFont defaultMonospace = MCTextureFontManager::instance().font("default");

    const int shadowY = -2;
    const int shadowX =  2;

    text.setGlyphSize(m_textSize, m_textSize);

    if (owner().focused())
    {
        text.setColor(1.0, 1.0, 0.0, 1.0);
    }
    else if (owner().selected())
    {
        text.setColor(1.0, 0.0, 0.0, 1.0);
    }
    else
    {
        text.setColor(1.0, 1.0, 1.0, 1.0);
    }

    text.setShadowOffset(shadowX, shadowY);
    text.render(x - text.width() / 2 + 20, y, nullptr, defaultMonospace);
}

TextMenuItemView::~TextMenuItemView()
{
}
