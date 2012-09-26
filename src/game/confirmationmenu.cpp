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

#include "confirmationmenu.hpp"
#include "menuitem.hpp"
#include "menuitemaction.hpp"
#include "menuitemview.hpp"
#include "menumanager.hpp"

ConfirmationMenu::ConfirmationMenu(std::string id, int width, int height)
: SurfaceMenu("helpBack", id, width, height, Menu::MS_HORIZONTAL_LIST)
, m_acceptItem(new MenuItem(width / 4, height, "Yes"))
, m_cancelItem(new MenuItem(width / 4, height, "No"))
{
    m_acceptItem->setView(new MenuItemView(*m_acceptItem), true);
    m_cancelItem->setView(new MenuItemView(*m_cancelItem), true);

    addItem(*m_acceptItem, true);
    addItem(*m_cancelItem, true);
}

void ConfirmationMenu::setAcceptAction(MenuItemAction & action)
{
    m_acceptItem->setAction(&action);
}

void ConfirmationMenu::setCancelAction(MenuItemAction & action)
{
    m_cancelItem->setAction(&action);
}

void ConfirmationMenu::selectCurrentItem()
{
    Menu::selectCurrentItem();
    exit();
}
