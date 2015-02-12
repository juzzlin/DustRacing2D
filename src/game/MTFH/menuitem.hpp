// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MENUITEM_HPP
#define MENUITEM_HPP

#include <functional>
#include <string>
#include <memory>

#include "menuitemview.hpp"
#include "menuitemaction.hpp"

namespace MTFH {

class Menu;
class MenuItemAction;

//! Base class for items used in menus.
class MenuItem
{
public:

    MenuItem(int width, int height, std::wstring text = L"", bool selectable = false);

    virtual ~MenuItem();

    void setIndex(int index);

    int index() const;

    int width() const;

    int height() const;

    void setPos(int x, int y);

    int x() const;

    int y() const;

    const std::wstring & text() const;

    void setView(MenuItemViewPtr view);

    MenuItemViewPtr view();

    void setAction(MenuItemActionPtr action);

    void setAction(std::function<void()> actionFunction);

    void setMenuOpenAction(const std::string & menuId);

    MenuItemActionPtr action() const;

    virtual void render();

    virtual void onLeft();

    virtual void onRight();

    virtual void onUp();

    virtual void onDown();

    virtual void setSelected(bool flag);

    virtual void setFocused(bool focused);

    void setContentsMargins(int left, int right, int top, int bottom);

    void getContentsMargins(int & left, int & right, int & top, int & bottom) const;

    bool selected() const;

    bool selectable() const;

    bool focused() const;

private:

    std::wstring m_text;

    std::string m_menuOpenActionMenuId;

    MenuItemActionPtr m_action;

    std::function<void()> m_actionFunction;

    MenuItemViewPtr m_view;

    bool m_focused;

    bool m_selected;

    bool m_selectable;

    int m_width;

    int m_height;

    int m_x;

    int m_y;

    int m_lMargin;

    int m_rMargin;

    int m_tMargin;

    int m_bMargin;

    int m_index;
};

typedef std::shared_ptr<MTFH::MenuItem> MenuItemPtr;

} // namespace MTFH

#endif // MENUITEM_HPP
