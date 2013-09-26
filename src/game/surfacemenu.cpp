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

#include "surfacemenu.hpp"
#include "renderer.hpp"
#include "textmenuitemview.hpp"

#include <MCSurface>
#include <MCTextureFont>
#include <MCAssetManager>

#include <MenuItem>

#include <cassert>

SurfaceMenu::SurfaceMenu(
    std::string surfaceId,
    std::string id,
    int width,
    int height,
    Menu::MenuStyle style,
    bool quitItem,
    bool prevItem,
    bool nextItem)
: Menu(id, width, height, style)
, m_back(MCAssetManager::surfaceManager().surface(surfaceId))
, m_font(MCAssetManager::textureFontManager().font("default"))
{
    m_back.setShaderProgram(&Renderer::instance().program("menu"));
    m_back.setColor(MCGLColor(0.5, 0.5, 0.5, 1.0));

    if (quitItem)
    {
        const int textSize = 40;
        MTFH::MenuItem * quit = new MTFH::MenuItem(textSize, textSize, "X");
        quit->setView(new TextMenuItemView(textSize, *quit), true);
        addMouseItem(MTFH::Menu::MI_QUIT, *quit, true);
    }

    if (prevItem)
    {
        const int textSize = 40;
        MTFH::MenuItem * prev = new MTFH::MenuItem(textSize, textSize, "<");
        prev->setView(new TextMenuItemView(textSize, *prev), true);
        addMouseItem(MTFH::Menu::MI_PREV, *prev, true);
    }

    if (nextItem)
    {
        const int textSize = 40;
        MTFH::MenuItem * next = new MTFH::MenuItem(textSize, textSize, ">");
        next->setView(new TextMenuItemView(textSize, *next), true);
        addMouseItem(MTFH::Menu::MI_NEXT, *next, true);
    }
}

void SurfaceMenu::render()
{
    const int w2 = width()  / 2;
    const int h2 = height() / 2;
    assert(w2 > 0 && h2 > 0);
    m_back.bindTextures();
    m_back.setSize(width(), height());
    m_back.render(nullptr, MCVector3dF(w2, h2, 0), 0);
    Menu::render();
}

MCTextureFont & SurfaceMenu::font() const
{
    return m_font;
}
