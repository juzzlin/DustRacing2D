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

#include "menu.hpp"
#include "menuitem.hpp"

#include <cassert>

Menu::Menu(int width, int height, MenuStyle style)
: m_width(width)
, m_height(height)
, m_currentIndex(0)
, m_style(style)
, m_done(false)
{
}

void Menu::addItem(MenuItem & menuItem, bool takeOwnership)
{
    m_menuItems.push_back(&menuItem);

    if (takeOwnership)
    {
        m_ownedMenuItems.push_back(std::shared_ptr<MenuItem>(&menuItem));
    }

    updateFocus();
}

MenuItem * Menu::currentItem() const
{
    if (m_menuItems.size())
    {
        return m_menuItems.at(m_currentIndex);
    }

    return nullptr;
}

void Menu::render()
{
    if (m_style == Menu::MS_LIST)
    {
        // Calculate total height
        int totalHeight = 0;
        for (MenuItem * item : m_menuItems)
        {
            totalHeight += item->height();
        }

        // Render centered items
        int startY = m_height / 2 - totalHeight / 2;
        for (MenuItem * item : m_menuItems)
        {
            const int x = m_width / 2;
            const int y = startY;

            item->render(x, y);
            startY += item->height();
        }
    }
    else if (m_style == Menu::MS_SHOW_ONE)
    {
        if (m_menuItems.size())
        {
            MenuItem & item = *m_menuItems.at(m_currentIndex);
            item.render(m_width / 2, m_height / 2);
        }
    }
}

void Menu::up()
{
    if (m_menuItems.size())
    {
        m_menuItems[m_currentIndex]->exit();
    }

    m_currentIndex++;
    if (m_currentIndex >= m_menuItems.size())
    {
        m_currentIndex = 0;
    }

    if (m_menuItems.size())
    {
        m_menuItems[m_currentIndex]->enter();
    }

    updateFocus();
}

void Menu::down()
{
    if (m_menuItems.size())
    {
        m_menuItems[m_currentIndex]->exit();
    }

    m_currentIndex--;
    if (m_currentIndex >= m_menuItems.size()) // m_currentIndex is unsigned
    {
        m_currentIndex = m_menuItems.size() - 1;
    }

    if (m_menuItems.size())
    {
        m_menuItems[m_currentIndex]->enter();
    }

    updateFocus();
}

void Menu::left()
{
    up();
}

void Menu::right()
{
    down();
}

void Menu::selectCurrentItem()
{
    if (m_menuItems.size())
    {
        m_menuItems.at(m_currentIndex)->onSelect();
    }
}

void Menu::updateFocus()
{
    if (m_menuItems.size())
    {
        for (MenuItem * item : m_menuItems)
        {
            item->setFocused(false);
        }

        m_menuItems.at(m_currentIndex)->setFocused(true);
    }
}

int Menu::width() const
{
    return m_width;
}

int Menu::height() const
{
    return m_height;
}

bool Menu::done() const
{
    return m_done;
}

void Menu::enter()
{
    m_done = false;
}

Menu::~Menu()
{
}

