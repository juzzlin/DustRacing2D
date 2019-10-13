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

#ifndef MENUMANAGER_HPP
#define MENUMANAGER_HPP

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace MTFH {

class Menu;

using MenuPtr = std::shared_ptr<Menu>;

//! The singleton MenuManager manages menus.
class MenuManager
{
public:
    //! Constructor.
    MenuManager();

    //! Returns the active menu or nullptr.
    MenuPtr activeMenu() const;

    //! Add the given menu.
    void addMenu(MenuPtr newMenu);

    //! Returns matching menu or explode.
    MenuPtr getMenuById(std::string menuId);

    //! Enter the given menu & clear the menu stack.
    void enterMenu(MenuPtr newMenu);

    //! Enter the given menu & clear the menu stack.
    void enterMenu(std::string menuId);

    //! Enter the given menu & push to stack.
    void pushMenu(MenuPtr newMenu);

    //! Enter the given menu & push to stack.
    void pushMenu(std::string menuId);

    //! Force exit on current menu and enter the previous menu.
    void popMenu();

    //! Force exit on current menu and enter the given menu.
    void popToMenu(std::string menuId);

    //! Force enter on current menu.
    void enterCurrentMenu();

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

    //! Give a time tick to the whole menu tree.
    void stepTime(int msecs);

    /*! Handle mouse press at given screen coordinates.
     *  \return true if on a valid item. */
    bool mousePress(int x, int y, int screenWidth, int screenHeight);

    /*! Handle mouse release at given screen coordinates.
     *  \return true if on a valid item. */
    bool mouseRelease(int x, int y, int screenWidth, int screenHeight);

    //! \returns true when done.
    bool isDone() const;

    static MenuManager & instance();

private:
    std::map<std::string, MenuPtr> m_idToMenuMap;

    std::vector<MenuPtr> m_menuStack;

    MenuPtr m_prevMenu;

    static MenuManager * m_instance;
};

} // namespace MTFH

#endif // MENUMANAGER_HPP
