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

Menu::Menu(std::string id, int width, int height, MenuStyle style)
: m_id(id)
, m_width(width)
, m_height(height)
, m_currentIndex(-1)
, m_style(style)
, m_done(false)
, m_wrapAround(true)
{
}

std::string Menu::id() const
{
    return m_id;
}

void Menu::addItem(MenuItem & menuItem, bool takeOwnership)
{
    m_items.push_back(&menuItem);
    m_currentIndex = m_items.size() - 1;

    if (takeOwnership)
    {
        m_ownedMenuItems.push_back(std::shared_ptr<MenuItem>(&menuItem));
    }

    updateFocus();
}

MenuItem * Menu::currentItem() const
{
    if (m_items.size())
    {
        return m_items.at(m_currentIndex);
    }

    return nullptr;
}

void Menu::render()
{
    if (m_style == Menu::MS_VERTICAL_LIST)
    {
        // Calculate total height
        int totalHeight = 0;
        for (MenuItem * item : m_items)
        {
            totalHeight += item->height();
        }

        // Render centered items
        int startY = m_height / 2 - totalHeight / 2 + totalHeight / m_items.size() / 2;
        for (MenuItem * item : m_items)
        {
            item->render(m_width / 2, startY);
            startY += item->height();
        }
    }
    else if (m_style == Menu::MS_HORIZONTAL_LIST)
    {
        // Not implemented
        assert(false);
    }
    else if (m_style == Menu::MS_SHOW_ONE)
    {
        if (m_items.size())
        {
            MenuItem & item = *m_items.at(m_currentIndex);
            item.render(m_width / 2, m_height / 2);
        }
    }
}

void Menu::up()
{
    if (m_wrapAround)
    {
        setCurrentIndexWrapAround(m_currentIndex + 1);
    }
    else
    {
        setCurrentIndex(m_currentIndex + 1);
    }

    updateFocus();
}

void Menu::down()
{
    if (m_wrapAround)
    {
        setCurrentIndexWrapAround(m_currentIndex - 1);
    }
    else
    {
        setCurrentIndex(m_currentIndex - 1);
    }

    updateFocus();
}

void Menu::left()
{
    down();
}

void Menu::right()
{
    up();
}

void Menu::selectCurrentItem()
{
    if (m_items.size())
    {
        m_items.at(m_currentIndex)->onSelect();
    }
}

void Menu::updateFocus()
{
    if (m_items.size())
    {
        for (MenuItem * item : m_items)
        {
            item->setFocused(false);
        }

        m_items.at(m_currentIndex)->setFocused(true);
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

int Menu::currentIndex() const
{
    return m_currentIndex;
}

void Menu::setCurrentIndex(int index)
{
    const int numItems = m_items.size();

    if (!numItems)
    {
        m_currentIndex = -1;
    }
    else
    {
        m_currentIndex = index;
        m_currentIndex = m_currentIndex < 0         ? 0            : m_currentIndex;
        m_currentIndex = m_currentIndex >= numItems ? numItems - 1 : m_currentIndex;

        updateFocus();
    }
}

void Menu::setCurrentIndexWrapAround(int index)
{
    const int numItems = m_items.size();

    if (!numItems)
    {
        m_currentIndex = -1;
    }
    else
    {
        m_currentIndex = index;
        m_currentIndex = m_currentIndex < 0         ? numItems - 1 : m_currentIndex;
        m_currentIndex = m_currentIndex >= numItems ? 0            : m_currentIndex;

        updateFocus();
    }
}

bool Menu::done() const
{
    return m_done;
}

void Menu::setDone(bool done)
{
    m_done = done;
}

void Menu::enter()
{
    m_done = false;

    updateFocus();
}

void Menu::setWrapAround(bool wrapAround)
{
    m_wrapAround = wrapAround;
}

Menu::~Menu()
{
}

