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

#include "menuitem.hpp"
#include "menu.hpp"
#include "menuitemaction.hpp"
#include "menumanager.hpp"

namespace MTFH {

MenuItem::MenuItem(float width, float height, std::wstring text, bool selectable)
  : m_menu(nullptr)
  , m_text(text)
  , m_menuOpenActionMenuId("")
  , m_action(nullptr)
  , m_actionFunction(nullptr)
  , m_view(nullptr)
  , m_focused(false)
  , m_selected(false)
  , m_selectable(selectable)
  , m_width(width)
  , m_height(height)
  , m_x(0)
  , m_y(0)
  , m_targetX(0)
  , m_targetY(0)
  , m_lMargin(1)
  , m_rMargin(1)
  , m_tMargin(1)
  , m_bMargin(1)
  , m_index(-1)
  , m_animationCurve(30, 3)
{
}

float MenuItem::width() const
{
    return m_width + m_lMargin + m_rMargin;
}

float MenuItem::height() const
{
    return m_height + m_tMargin + m_bMargin;
}

void MenuItem::setIndex(int index)
{
    m_index = index;
}

int MenuItem::index() const
{
    return m_index;
}

void MenuItem::setPos(float x, float y)
{
    m_x = x;
    m_y = y;
    m_targetX = x;
    m_targetY = y;
}

void MenuItem::setPos(float x, float y, float targetX, float targetY)
{
    m_x = x;
    m_y = y;
    m_targetX = targetX;
    m_targetY = targetY;
}

float MenuItem::x() const
{
    return m_x;
}

float MenuItem::y() const
{
    return m_y;
}

void MenuItem::resetAnimationCurve(int steps, int exp)
{
    m_animationCurve = AnimationCurve(steps, exp);
    m_animationCurve.reset();
}

void MenuItem::setView(MenuItemViewPtr view)
{
    m_view = view;
}

MenuItemViewPtr MenuItem::view()
{
    return m_view;
}

void MenuItem::setAction(MenuItemActionPtr action)
{
    m_action = action;
}

void MenuItem::setAction(ActionFunction actionFunction)
{
    m_actionFunction = actionFunction;
}

void MenuItem::setMenuOpenAction(const std::string & menuId)
{
    m_menuOpenActionMenuId = menuId;
}

MenuItemActionPtr MenuItem::action() const
{
    return m_action;
}

void MenuItem::onLeft()
{
}

void MenuItem::onRight()
{
}

void MenuItem::onUp()
{
}

void MenuItem::onDown()
{
}

void MenuItem::setCurrent()
{
    if (m_menu)
    {
        m_menu->setCurrentIndex(index());
    }
}

void MenuItem::setSelected(bool flag)
{
    if (flag)
    {
        if (m_action)
        {
            m_action->fire();
        }

        if (m_actionFunction)
        {
            m_actionFunction();
        }

        if (!m_menuOpenActionMenuId.empty())
        {
            MenuManager::instance().pushMenu(m_menuOpenActionMenuId);
        }
    }

    m_selected = flag;
}

void MenuItem::setFocused(bool focused)
{
    m_focused = focused;
}

void MenuItem::setContentsMargins(float left, float right, float top, float bottom)
{
    m_lMargin = left;
    m_rMargin = right;
    m_tMargin = top;
    m_bMargin = bottom;
}

void MenuItem::getContentsMargins(float & left, float & right, float & top, float & bottom) const
{
    left = m_lMargin;
    right = m_rMargin;
    top = m_tMargin;
    bottom = m_bMargin;
}

bool MenuItem::focused() const
{
    return m_focused;
}

float MenuItem::targetY() const
{
    return m_targetY;
}

Menu * MenuItem::menu() const
{
    return m_menu;
}

void MenuItem::setMenu(Menu * menu)
{
    m_menu = menu;
}

float MenuItem::targetX() const
{
    return m_targetX;
}

bool MenuItem::selected() const
{
    return m_selected && m_selectable;
}

bool MenuItem::selectable() const
{
    return m_selectable;
}

void MenuItem::render()
{
    if (m_view)
    {
        m_view->render(m_x, m_y);
    }
}

const std::wstring & MenuItem::text() const
{
    return m_text;
}

void MenuItem::stepTime(int msecs)
{
    if (m_view)
    {
        m_view->stepTime(msecs);
    }

    positionAnimation(msecs);
}

void MenuItem::positionAnimation(int)
{
    m_animationCurve.step();

    m_x = m_x + (m_targetX - m_x) * m_animationCurve.value();
    m_y = m_y + (m_targetY - m_y) * m_animationCurve.value();
}

MenuItem::~MenuItem()
{
}

} // namespace MTFH
