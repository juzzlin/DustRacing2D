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

#include "confirmationmenu.hpp"
#include "game.hpp"
#include "textmenuitemview.hpp"

#include <MenuItemView>
#include <MenuManager>

#include <MCAssetManager>
#include <MCTextureFont>
#include <MCTextureText>

#include <QObject>

ConfirmationMenu::ConfirmationMenu(std::string id, int width, int height)
  : SurfaceMenu("settingsBack", id, width, height, MTFH::Menu::Style::HorizontalList, false)
  , m_acceptItem(std::make_shared<MTFH::MenuItem>(width / 4, height, QObject::tr("Ok").toStdWString()))
  , m_cancelItem(std::make_shared<MTFH::MenuItem>(width / 4, height, QObject::tr("Cancel").toStdWString()))
  , m_text(L"")
{
    m_acceptItem->setView(std::make_shared<TextMenuItemView>(20, *m_acceptItem));
    m_cancelItem->setView(std::make_shared<TextMenuItemView>(20, *m_cancelItem));

    addItem(m_acceptItem);
    addItem(m_cancelItem);
}

void ConfirmationMenu::setAcceptAction(MTFH::MenuItemActionPtr action)
{
    m_acceptItem->setAction(action);
}

void ConfirmationMenu::setAcceptAction(std::function<void()> actionFunction)
{
    m_acceptItem->setAction(actionFunction);
}

void ConfirmationMenu::setCancelAction(MTFH::MenuItemActionPtr action)
{
    m_cancelItem->setAction(action);
}

void ConfirmationMenu::setCancelAction(std::function<void()> actionFunction)
{
    m_cancelItem->setAction(actionFunction);
}

void ConfirmationMenu::setText(std::wstring text)
{
    m_text = text;
}

void ConfirmationMenu::selectCurrentItem()
{
    Menu::selectCurrentItem();
    exit();
}

void ConfirmationMenu::render()
{
    SurfaceMenu::render();

    MCTextureText text(m_text);
    text.setColor(MCGLColor(0.25, 0.75, 1.0, 1.0));
    text.setGlyphSize(20, 20);
    const int shadowY = -2;
    const int shadowX = 2;
    text.setShadowOffset(shadowX, shadowY);

    auto && font = MCAssetManager::textureFontManager().font(Game::instance().fontName());
    text.render(x() + width() / 2 - text.width(font) / 2 + 20, y() + height() / 2 + 60, nullptr, font);
}
