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

#include "menumanager.hpp"
#include "menu.hpp"

#include <cassert>

namespace MTFH {

MenuManager * MenuManager::m_pInstance = nullptr;

MenuManager::MenuManager()
{
    assert(!m_pInstance);
    m_pInstance = this;
}

Menu * MenuManager::activeMenu() const
{
    if (m_menuStack.size())
    {
        return m_menuStack.back();
    }

    return nullptr;
}

void MenuManager::addMenu(Menu & newMenu)
{
    m_idToMenuMap[newMenu.id()] = &newMenu;
}

void MenuManager::enterMenu(Menu & newMenu)
{
    m_menuStack.clear();
    m_menuStack.push_back(&newMenu);
    newMenu.enter();
    newMenu.render();
}

void MenuManager::enterMenu(std::string menuId)
{
    assert(m_idToMenuMap[menuId]);
    enterMenu(*m_idToMenuMap[menuId]);
}

void MenuManager::pushMenu(Menu & newMenu)
{
    if (!m_menuStack.size() || m_menuStack.back() != &newMenu)
    {
        m_menuStack.push_back(&newMenu);
        newMenu.enter();
        newMenu.render();
    }
}

void MenuManager::pushMenu(std::string menuId)
{
    assert(m_idToMenuMap[menuId]);
    pushMenu(*m_idToMenuMap[menuId]);
}

void MenuManager::popMenu()
{
    if (m_menuStack.size())
    {
        m_menuStack.pop_back();
    }

    enterCurrentMenu();
}

void MenuManager::enterCurrentMenu()
{
    if (m_menuStack.size())
    {
        m_menuStack.back()->enter();
    }
}

void MenuManager::exit()
{
    m_menuStack.clear();
}

MenuManager & MenuManager::instance()
{
    assert(m_pInstance);
    return *m_pInstance;
}

void MenuManager::render()
{
    if (m_menuStack.size())
    {
        m_menuStack.back()->render();
    }
}

void MenuManager::up()
{
    if (m_menuStack.size())
    {
        m_menuStack.back()->up();
    }
}

void MenuManager::down()
{
    if (m_menuStack.size())
    {
        m_menuStack.back()->down();
    }
}

void MenuManager::left()
{
    if (m_menuStack.size())
    {
        m_menuStack.back()->left();
    }
}

void MenuManager::right()
{
    if (m_menuStack.size())
    {
        m_menuStack.back()->right();
    }
}

bool MenuManager::mousePress(int x, int y, int screenWidth, int screenHeight)
{
    if (m_menuStack.size())
    {
        return m_menuStack.back()->handleMousePress(x, y, screenWidth, screenHeight);
    }

    return false;
}

bool MenuManager::mouseRelease(int x, int y, int screenWidth, int screenHeight)
{
    if (m_menuStack.size())
    {
        return m_menuStack.back()->handleMouseRelease(x, y, screenWidth, screenHeight);
    }

    return false;
}

void MenuManager::selectCurrentItem()
{
    if (m_menuStack.size())
    {
        m_menuStack.back()->selectCurrentItem();
    }
}

bool MenuManager::isDone() const
{
    if (!m_menuStack.size())
    {
        return true;
    }
    else
    {
        return m_menuStack.back()->isDone();
    }
}

} // namespace MTFH
