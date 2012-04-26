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

    MenuItem(int width, int height, std::string text = "");

    virtual ~MenuItem();

    int width() const;

    int height() const;

    const std::string & text() const;

    void setView(MenuItemView * menuItemView, bool takeOwnership);

    MenuItemView * view();

    virtual void render(int x1, int y1, int x2, int y2);

    virtual void onLeft();

    virtual void onRight();

    virtual void onSelect();

    void setFocused(bool focused);

    void setContentsMargins(int left, int right, int top, int bottom);

    void getContentsMargins(int & left, int & right, int & top, int & bottom) const;

    bool focused() const;

private:

    std::string m_text;
    MenuItemView * m_pView;
    std::shared_ptr<MenuItemView> m_pOwnedView;
    bool m_focused;
    int m_width, m_height;
    int m_lMargin, m_rMargin, m_tMargin, m_bMargin;
};

#endif // MENUITEM_HPP
