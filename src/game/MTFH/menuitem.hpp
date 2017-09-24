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

    MenuItem(float width, float height, std::wstring text = L"", bool selectable = false);

    virtual ~MenuItem();

    void setIndex(float index);

    float index() const;

    float width() const;

    float height() const;

    void setPos(float x, float y);

    float x() const;

    float y() const;

    const std::wstring & text() const;

    void setView(MenuItemViewPtr view);

    MenuItemViewPtr view();

    void setAction(MenuItemActionPtr action);

    typedef std::function<void()> ActionFunction;
    void setAction(ActionFunction actionFunction);

    void setMenuOpenAction(const std::string & menuId);

    MenuItemActionPtr action() const;

    virtual void render();

    virtual void onLeft();

    virtual void onRight();

    virtual void onUp();

    virtual void onDown();

    virtual void setSelected(bool flag);

    virtual void setFocused(bool focused);

    virtual void stepTime(int msecs);

    void setContentsMargins(float left, float right, float top, float bottom);

    void getContentsMargins(float & left, float & right, float & top, float & bottom) const;

    bool selected() const;

    bool selectable() const;

    bool focused() const;

private:

    std::wstring m_text;

    std::string m_menuOpenActionMenuId;

    MenuItemActionPtr m_action;

    ActionFunction m_actionFunction;

    MenuItemViewPtr m_view;

    bool m_focused;

    bool m_selected;

    bool m_selectable;

    float m_width;

    float m_height;

    float m_x;

    float m_y;

    float m_lMargin;

    float m_rMargin;

    float m_tMargin;

    float m_bMargin;

    float m_index;
};

typedef std::shared_ptr<MTFH::MenuItem> MenuItemPtr;

} // namespace MTFH

#endif // MENUITEM_HPP
