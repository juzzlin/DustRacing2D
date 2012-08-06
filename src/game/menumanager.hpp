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

#ifndef MENUMANAGER_HPP
#define MENUMANAGER_HPP

#include <map>
#include <string>
#include <vector>

class Menu;

//! The singleton MenuManager manages menus.
class MenuManager
{
public:

    //! Constructor.
    MenuManager();

    //! Returns the active menu or nullptr.
    Menu * activeMenu() const;

    //! Add the given menu.
    void addMenu(Menu & newMenu);

    //! Enter the given menu & clear the menu stack.
    void enterMenu(Menu & newMenu);

    //! Enter the given menu & clear the menu stack.
    void enterMenu(std::string menuId);

    //! Enter the given menu & push to stack.
    void pushMenu(Menu & newMenu);

    //! Enter the given menu & push to stack.
    void pushMenu(std::string menuId);

    //! Force enter on current menu.
    void enterCurrentMenu();

    //! Force exit on current menu.
    void exitCurrentMenu();

    //! Force exit on all menus.
    void exit();

    //! Render the menu.
    void render();

    //! Selection up.
    void up();

    //! Selection down.
    void down();

    //! Current item left.
    void left();

    //! Current item right.
    void right();

    //! Select the current item.
    void selectCurrentItem();

    //! \returns true when done.
    bool done() const;

    static MenuManager & instance();

private:

    std::map<std::string, Menu *> m_idToMenuMap;
    std::vector<Menu *> m_menuStack;
    static MenuManager * m_pInstance;
};

#endif // MENUMANAGER_HPP
