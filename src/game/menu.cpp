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

Menu::Menu(unsigned int width, unsigned int height)
  : m_width(width)
  , m_height(height)
  , m_currentIndex(0)
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

void Menu::render()
{
    for (MenuItem * item : m_menuItems)
    {
        item->render();
    }
}

void Menu::up()
{
    assert(m_menuItems.size());
    m_currentIndex++;
    if (m_currentIndex >= m_menuItems.size())
    {
        m_currentIndex = 0;
    }

    updateFocus();
}

void Menu::down()
{
    assert(m_menuItems.size());
    m_currentIndex--;
    if (m_currentIndex >= m_menuItems.size()) // m_currentIndex is unsisgned.
    {
        m_currentIndex = m_menuItems.size() - 1;
    }

    updateFocus();
}

void Menu::left()
{
    assert(m_menuItems.size());
    assert(m_currentIndex < m_menuItems.size());

    m_menuItems.at(m_currentIndex)->onLeft();
}

void Menu::right()
{
    assert(m_menuItems.size());
    assert(m_currentIndex < m_menuItems.size());

    m_menuItems.at(m_currentIndex)->onRight();
}

void Menu::selectCurrentItem()
{
    assert(m_menuItems.size());

    m_menuItems.at(m_currentIndex)->onSelect();
}

void Menu::updateFocus()
{
    assert(m_menuItems.size());
    assert(m_currentIndex < m_menuItems.size());

    for (MenuItem * item : m_menuItems)
    {
        item->setFocused(false);
    }

    m_menuItems.at(m_currentIndex)->setFocused(true);
}

Menu::~Menu()
{
}

