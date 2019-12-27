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
#include "game.hpp"
#include "renderer.hpp"
#include "textmenuitemview.hpp"

#include <MCAssetManager>
#include <MCSurface>
#include <MCTextureFont>

#include <MenuItem>

#include <cassert>

SurfaceMenu::SurfaceMenu(std::string surfaceId, std::string id, int width, int height, Menu::Style style, bool quitItem, bool prevItem, bool nextItem)
  : Menu(id, width, height, style)
  , m_back(MCAssetManager::surfaceManager().surface(surfaceId))
  , m_font(MCAssetManager::textureFontManager().font(Game::instance().fontName()))
{
    m_back->setShaderProgram(Renderer::instance().program("menu"));
    m_back->setColor({ 0.5, 0.5, 0.5, 1.0 });

    if (quitItem)
    {
        const int textSize = 40;
        const auto quit = std::make_shared<MTFH::MenuItem>(textSize, textSize, L"X");
        quit->setView(std::make_shared<TextMenuItemView>(textSize, *quit));
        addMouseItem(MTFH::Menu::MouseItemType::Quit, quit);
    }

    if (prevItem)
    {
        const int textSize = 40;
        const auto prev = std::make_shared<MTFH::MenuItem>(textSize, textSize, L"<");
        prev->setView(std::make_shared<TextMenuItemView>(textSize, *prev));
        addMouseItem(MTFH::Menu::MouseItemType::Prev, prev);
    }

    if (nextItem)
    {
        const int textSize = 40;
        const auto next = std::make_shared<MTFH::MenuItem>(textSize, textSize, L">");
        next->setView(std::make_shared<TextMenuItemView>(textSize, *next));
        addMouseItem(MTFH::Menu::MouseItemType::Next, next);
    }
}

void SurfaceMenu::render()
{
    m_back->bind();
    m_back->setSize(width(), width() * m_back->height() / m_back->width());
    m_back->render(nullptr, { x() + width() / 2, y() + height() / 2, 0 }, 0);
    Menu::render();
}

void SurfaceMenu::enter()
{
    Menu::enter();
    setPos(0, 0, 0, 0);
}

void SurfaceMenu::pushEnter()
{
    Menu::pushEnter();
    setPos(width(), 0, 0, 0);
}

void SurfaceMenu::pushExit()
{
    Menu::pushExit();
    setPos(0, 0, -width(), 0);
}

void SurfaceMenu::popEnter()
{
    Menu::popEnter();
    setPos(-width(), 0, 0, 0);
}

void SurfaceMenu::popExit()
{
    Menu::popExit();
    setPos(0, 0, width(), 0);
}

MCTextureFont & SurfaceMenu::font() const
{
    return m_font;
}
