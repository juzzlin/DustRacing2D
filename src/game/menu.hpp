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

    //! Default styles
    enum MenuStyle
    {
        MS_LIST,
        MS_SHOW_ONE // Show only one item at a time
    };

    //! Constructor.
    Menu(int width, int height, MenuStyle style = MS_LIST);

    //! Destructor.
    virtual ~Menu();

    //! Render the menu.
    virtual void render();

    //! Selection up.
    virtual void up();

    //! Selection down.
    virtual void down();

    //! Current item left.
    virtual void left();

    //! Current item right.
    virtual void right();

    //! Select the current item.
    virtual void selectCurrentItem();

    //! Return current item of nullptr.
    MenuItem * currentItem() const;

    //! Add item to the menu.
    virtual void addItem(MenuItem & menuItem, bool takeOwnership = false);

    //! \returns true if done.
    bool done() const;

    //! Called when the menu is entered. Call parent implementation if overridden.
    virtual void enter();

    int width() const;

    int height() const;

private:

    void updateFocus();

    std::vector<MenuItem *> m_menuItems;
    std::vector<std::shared_ptr<MenuItem> > m_ownedMenuItems;
    int m_width, m_height;
    unsigned int m_currentIndex;
    MenuStyle m_style;
    bool m_done;
};

#endif // MENU_HPP
