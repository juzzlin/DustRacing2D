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

#ifndef MENU_HPP
#define MENU_HPP

#include <string>
#include <vector>
#include <memory>

#include "menumanager.hpp"
#include "menuitem.hpp"

namespace MTFH {

class MenuItem;

class Menu
{
public:

    //! Default styles
    enum class Style
    {
        VerticalList,
        HorizontalList,
        ShowOne // Show only one item at a time
    };

    enum class MouseItemType
    {
        Quit,
        Prev,
        Next
    };

    //! Constructor.
    Menu(std::string id, int width, int height, Style style = Style::VerticalList);

    //! Destructor.
    virtual ~Menu();

    std::string id() const;

    //! Renders the menu items. See also MenuItemView.
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

    /*! Handle mouse press at given screen coordinates.
     *  \return true if on a valid item. */
    virtual bool handleMousePress(int x, int y, int screenWidth, int screenHeight);

    /*! Handle mouse release at given screen coordinates.
     *  \return true if on a valid item. */
    virtual bool handleMouseRelease(int x, int y, int screenWidth, int screenHeight);

    //! \return current item or nullptr if no items added.
    MenuItemPtr currentItem() const;

    /*! \return selected item or nullptr if no item added.
     *  Pressing enter on an item selects it. */
    MenuItemPtr selectedItem() const;

    //! \return current item at given index or nullptr.
    MenuItemPtr item(unsigned int index) const;

    //! \return number of currently added items.
    unsigned int itemCount() const;

    //! Add item to the menu.
    virtual void addItem(MenuItemPtr menuItem);

    //! Reverse the item vector.
    virtual void reverseItems();

    //! Add mouse control item.
    virtual void addMouseItem(MouseItemType type, MenuItemPtr menuItem);

    //! Use this if the current menu stack needs to be
    //! completely quit, for example when the execution
    //! should move from the menu to the actual game.
    void setIsDone(bool done);

    //! \returns true if done.
    bool isDone() const;

    //! Returns the width of the menu.
    int width() const;

    //! Returns the height of the menu.
    int height() const;

    //! Return the current index.
    int currentIndex() const;

    //! Set default wrap behavior.
    void setWrapAround(bool wrapAround);

    //! Set current index. Clamp if out-of-range.
    void setCurrentIndex(int index);

    //! Set current index. Wrap around if out-of-range.
    void setCurrentIndexWrapAround(int index);

    //! Exits and pops the menu stack. The menu can call this
    //! also by itself.
    void exit();

protected:

    //! Called when the menu is entered. Call parent implementation if overridden.
    virtual void enter();

private:

    struct MouseItem
    {
        MenuItemPtr item;
        MouseItemType type;
    };

    bool isNextAllowed() const;

    bool isPrevAllowed() const;

    bool checkIfHit(MenuItemPtr item, int x, int y);

    bool doHandleMousePressOnMouseItem(MenuItemPtr item, int x, int y);

    bool handleMousePressOnItem(int x, int y);

    bool handleMousePressOnMouseItem(int x, int y);

    bool handleMouseReleaseOnItem(int x, int y);

    bool handleMouseReleaseOnMouseItem(int x, int y);

    void renderItems();

    void renderMouseItems();

    void updateFocus();

    std::vector<MenuItemPtr > m_items;

    std::vector<MouseItem> m_mouseItems;

    std::string m_id;

    int m_width;

    int m_height;

    int m_currentIndex;

    int m_selectedIndex;

    Style m_style;

    bool m_isDone;

    bool m_wrapAround;

    friend class MenuManager;
};

} // namespace MTFH

#endif // MENU_HPP
