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

namespace MTFH {

class Menu;
class MenuItemAction;
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

    void setView(MenuItemView * view, bool takeOwnership = false);

    MenuItemView * view();

    void setAction(MenuItemAction * action, bool takeOwnership = false);

    MenuItemAction * action() const;

    virtual void render(int x, int y);

    virtual void onLeft();

    virtual void onRight();

    virtual void onUp();

    virtual void onDown();

    //! Call action, if set.
    virtual void onSelect();

    virtual void setFocused(bool focused);

    void setContentsMargins(int left, int right, int top, int bottom);

    void getContentsMargins(int & left, int & right, int & top, int & bottom) const;

    bool focused() const;

private:

    std::string m_text;
    MenuItemAction * m_action;
    MenuItemView * m_view;
    typedef std::shared_ptr<MenuItemAction> MenuItemActionPtr;
    MenuItemActionPtr m_ownedAction;
    typedef std::shared_ptr<MenuItemView> MenuItemViewPtr;
    MenuItemViewPtr m_ownedView;
    bool m_focused;
    int m_width, m_height;
    int m_lMargin, m_rMargin, m_tMargin, m_bMargin;
};

} // namespace MTFH

#endif // MENUITEM_HPP
