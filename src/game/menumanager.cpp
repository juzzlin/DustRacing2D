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

#include "menumanager.hpp"
#include "menu.hpp"

#include <cassert>

MenuManager * MenuManager::m_pInstance = nullptr;

MenuManager::MenuManager()
{
    assert(!m_pInstance);
    m_pInstance = this;
}

Menu & MenuManager::activeMenu() const
{
    assert(m_menus.size());
    assert(m_menus.back());
    return *m_menus.back();
}

void MenuManager::enterMenu(Menu & newMenu)
{
    m_menus.push_back(&newMenu);
    newMenu.render();
}

void MenuManager::exitMenu()
{
    assert(m_menus.size() > 1);
    m_menus.pop_back();
}

MenuManager & MenuManager::instance()
{
    assert(m_pInstance);
    return *m_pInstance;
}
