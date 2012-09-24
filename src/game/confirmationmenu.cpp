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
#include "menuitemview.hpp"
#include "menumanager.hpp"

#include <MCTextureManager>
#include <MCSurface>

ConfirmationMenu::ConfirmationMenu(std::string id, int width, int height)
: Menu(id, width, height, Menu::MS_HORIZONTAL_LIST)
, m_back(MCTextureManager::instance().surface("helpBack"))
{
    MenuItem * yes = new MenuItem(width / 4, height, "Yes");
    yes->setView(new MenuItemView(*yes), true);

    MenuItem * no = new MenuItem(width / 4, height, "No");
    no->setView(new MenuItemView(*no), true);

    addItem(*yes, true);
    addItem(*no,  true);
}

void ConfirmationMenu::render()
{
    const int w2 = width()  / 2;
    const int h2 = height() / 2;

    m_back.renderScaled(nullptr, MCVector3dF(w2, h2, 0), w2, h2, 0);

    Menu::render();
}

void ConfirmationMenu::selectCurrentItem()
{
    Menu::selectCurrentItem();
    exit();
}
