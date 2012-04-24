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

#include "menuitem.hpp"
#include "menuitemview.hpp"
#include "menu.hpp"
#include "menumanager.hpp"

MenuItem::MenuItem(std::string text)
  : m_text(text)
  , m_menuOnClick(nullptr)
  , m_pView(nullptr)
{
}

void MenuItem::setView(MenuItemView * menuItemView, bool takeOwnership)
{
    m_pView = menuItemView;

    if (takeOwnership)
    {
        m_pOwnedView = std::shared_ptr<MenuItemView>(menuItemView);
    }
}

MenuItemView * MenuItem::view()
{
    return m_pView;
}

void MenuItem::setMenuOnClick(Menu & menuOnClick)
{
    m_menuOnClick = &menuOnClick;
}

void MenuItem::clicked()
{
    if (m_menuOnClick)
    {
        MenuManager::instance().enterMenu(*m_menuOnClick);
    }
}

void MenuItem::render()
{
    if (m_pView)
    {
        m_pView->render();
    }
}

std::string MenuItem::text() const
{
    return m_text;
}

MenuItem::~MenuItem()
{
}
