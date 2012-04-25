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

#ifndef MENU_HPP
#define MENU_HPP

#include <vector>
#include <memory>

class MenuItem;

class Menu
{
public:

    //! Constructor.
    Menu(unsigned int width, unsigned int height);

    //! Destructor.
    virtual ~Menu();

    //! Render the menu.
    virtual void render();

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

    //! Add item to the menu.
    void addItem(MenuItem & menuItem, bool takeOwnership = false);

private:

    void updateFocus();

    std::vector<MenuItem *> m_menuItems;
    std::vector<std::shared_ptr<MenuItem> > m_ownedMenuItems;
    unsigned int m_width, m_height;
    unsigned int m_currentIndex;
};

#endif // MENU_HPP
