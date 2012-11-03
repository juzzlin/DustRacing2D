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
#include "textmenuitemview.hpp"

#include <MenuItem>
#include <MenuItemAction>
#include <MenuItemView>
#include <MenuManager>

ConfirmationMenu::ConfirmationMenu(std::string id, int width, int height)
: SurfaceMenu("helpBack", id, width, height, MTFH::Menu::MS_HORIZONTAL_LIST)
, m_acceptItem(new MTFH::MenuItem(width / 4, height, "Yes"))
, m_cancelItem(new MTFH::MenuItem(width / 4, height, "No"))
{
    m_acceptItem->setView(new TextMenuItemView(40, *m_acceptItem), true);
    m_cancelItem->setView(new TextMenuItemView(40, *m_cancelItem), true);

    addItem(*m_acceptItem, true);
    addItem(*m_cancelItem, true);
}

void ConfirmationMenu::setAcceptAction(MTFH::MenuItemAction & action)
{
    m_acceptItem->setAction(&action);
}

void ConfirmationMenu::setCancelAction(MTFH::MenuItemAction & action)
{
    m_cancelItem->setAction(&action);
}

void ConfirmationMenu::selectCurrentItem()
{
    Menu::selectCurrentItem();
    exit();
}
