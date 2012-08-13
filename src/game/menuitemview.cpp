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

#include "menuitemview.hpp"
#include "menuitem.hpp"

#include <MCTextureFont>
#include <MCTextureFontManager>
#include <MCTextureText>

MenuItemView::MenuItemView(MenuItem & owner)
: m_owner(owner)
{
}

void MenuItemView::render(int x, int y)
{
    MCTextureText text(m_owner.text());
    MCTextureFont defaultMonospace = MCTextureFontManager::instance().font("default");

    const int shadowY = -2;
    const int shadowX =  2;

    text.setGlyphSize(40, 40);

    if (m_owner.focused())
    {
        text.setColor(1.0, 1.0, 0.0, 1.0);
    }
    else
    {
        text.setColor(1.0, 1.0, 1.0, 1.0);
    }

    text.setShadowOffset(shadowX, shadowY);
    text.render(x - text.width() / 2 + 20, y, nullptr, defaultMonospace);
}

MenuItemView::~MenuItemView()
{
}
