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

#ifndef MENUITEM_HPP
#define MENUITEM_HPP

#include <string>
#include <memory>

class Menu;
class MenuItemView;

//! Base class for items used in menus.
class MenuItem
{
public:

    MenuItem(std::string text = "");

    virtual ~MenuItem();

    void setView(MenuItemView * menuItemView, bool takeOwnership);

    MenuItemView * view();

    void setMenuOnClick(Menu & menuOnClick);

    virtual void clicked();

    virtual void render();

    std::string text() const;

private:

    std::string m_text;
    Menu * m_menuOnClick;
    MenuItemView * m_pView;
    std::shared_ptr<MenuItemView> m_pOwnedView;
};

#endif // MENUITEM_HPP
