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

#include "menu.hpp"
#include "menumanager.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
namespace MTFH {

Menu::Menu(std::string id, int width, int height, Menu::Style style)
  : m_id(id)
  , m_width(width)
  , m_height(height)
  , m_x(0)
  , m_y(0)
  , m_targetX(0)
  , m_targetY(0)
  , m_currentIndex(-1)
  , m_selectedIndex(-1)
  , m_style(style)
  , m_isDone(false)
  , m_wrapAround(true)
  , m_animationCurve(15, 3)
{
}

std::string Menu::id() const
{
    return m_id;
}

void Menu::addItem(MenuItemPtr menuItem)
{
    m_items.push_back(menuItem);
    m_currentIndex = m_items.size() - 1;
    m_selectedIndex = m_currentIndex;
    menuItem->setMenu(this);
    menuItem->setIndex(m_items.size() - 1);

    updateFocus();
}

void Menu::reverseItems()
{
    std::reverse(m_items.begin(), m_items.end());
    m_currentIndex = (m_items.size() - 1) - m_currentIndex;
    m_selectedIndex = m_currentIndex;

    for (size_t i = 0; i < m_items.size(); i++)
    {
        m_items.at(i)->setIndex(i);
    }
}

void Menu::addMouseItem(Menu::MouseItemType type, MenuItemPtr menuItem)
{
    MouseItem mouseItem;
    mouseItem.item = menuItem;
    mouseItem.type = type;
    m_mouseItems.push_back(mouseItem);
}

MenuItemPtr Menu::currentItem() const
{
    if (m_items.size())
    {
        return m_items.at(m_currentIndex);
    }

    return nullptr;
}

MenuItemPtr Menu::selectedItem() const
{
    if (m_items.size())
    {
        return m_items.at(m_selectedIndex);
    }

    return nullptr;
}

MenuItemPtr Menu::item(unsigned int index) const
{
    if (index < m_items.size())
    {
        return m_items.at(index);
    }

    return nullptr;
}

unsigned int Menu::itemCount() const
{
    return m_items.size();
}

void Menu::render()
{
    renderItems();
    renderMouseItems();
}

void Menu::renderItems()
{
    if (!m_items.size())
    {
        return;
    }

    if (m_style == Menu::Style::VerticalList)
    {
        // Calculate total height
        int totalHeight = 0;
        for (auto item : m_items)
        {
            totalHeight += item->height();
        }

        // Render centered items
        int startY = y() + m_height / 2 - totalHeight / 2 + totalHeight / m_items.size() / 2;
        for (auto item : m_items)
        {
            item->setPos(x() + m_width / 2, startY);
            item->render();
            startY += item->height();
        }
    }
    else if (m_style == Menu::Style::HorizontalList)
    {
        // Calculate total width
        int totalWidth = 0;
        for (auto item : m_items)
        {
            totalWidth += item->width();
        }

        // Render centered items
        int startX = x() + m_width / 2 - totalWidth / 2 + totalWidth / m_items.size() / 2;
        for (auto item : m_items)
        {
            item->setPos(startX, y() + m_height / 2);
            item->render();
            startX += item->width();
        }
    }
    else if (m_style == Menu::Style::ShowOne)
    {
        auto item = m_items.at(m_currentIndex);
        item->setPos(x() + m_width / 2, y() + m_height / 2);
        item->render();
    }
    else if (m_style == Menu::Style::ShowMany)
    {
        for (auto index : m_itemsToShow)
        {
            if (index >= 0 && index < static_cast<int>(m_items.size()))
            {
                m_items.at(index)->render();
            }
        }
    }
}

bool Menu::isNextAllowed() const
{
    return m_wrapAround || m_currentIndex + 1 < static_cast<int>(m_items.size());
}

bool Menu::isPrevAllowed() const
{
    return m_wrapAround || m_currentIndex > 0;
}

void Menu::renderMouseItems()
{
    for (MouseItem item : m_mouseItems)
    {
        switch (item.type)
        {
        case Menu::MouseItemType::Quit:
            item.item->setPos(x() + m_width - item.item->width(), y() + m_height - item.item->height());
            item.item->render();
            break;

        case Menu::MouseItemType::Prev:
            if (isPrevAllowed())
            {
                item.item->setPos(x() + item.item->width(), y() + m_height / 2);
                item.item->render();
            }
            break;

        case Menu::MouseItemType::Next:
            if (isNextAllowed())
            {
                item.item->setPos(x() + m_width - item.item->width(), y() + m_height / 2);
                item.item->render();
            }
            break;
        }
    }
}

void Menu::incrementIndex(int increment)
{
    if (m_wrapAround)
    {
        setCurrentIndexWrapAround(m_currentIndex + increment);
    }
    else
    {
        setCurrentIndex(m_currentIndex + increment);
    }

    updateFocus();
}

void Menu::up()
{
    incrementIndex(1);
}

void Menu::down()
{
    incrementIndex(-1);
}

void Menu::left()
{
    incrementIndex(-1);
}

void Menu::right()
{
    incrementIndex(1);
}

void Menu::selectCurrentItem()
{
    if (m_items.size())
    {
        for (auto item : m_items)
        {
            item->setSelected(false);
        }

        m_items.at(m_currentIndex)->setSelected(true);
        m_selectedIndex = m_currentIndex;
    }
}

bool Menu::checkIfHit(MenuItemPtr item, int x, int y)
{
    const int x1 = item->x() - item->width() / 2;
    const int x2 = item->x() + item->width() / 2;
    const int y1 = item->y() - item->height() / 2;
    const int y2 = item->y() + item->height() / 2;

    if (x >= x1 && x <= x2 && y >= y1 && y <= y2)
    {
        return true;
    }

    return false;
}

bool Menu::handleMousePressOnMouseItem(int x, int y)
{
    for (MouseItem mouseItem : m_mouseItems)
    {
        auto item = mouseItem.item;
        switch (mouseItem.type)
        {
        case MouseItemType::Quit:
            if (checkIfHit(item, x, y))
            {
                item->setFocused(true);
                return true;
            }
            break;
        case MouseItemType::Prev:
            if (isPrevAllowed() && checkIfHit(item, x, y))
            {
                item->setFocused(true);
                return true;
            }
            break;
        case MouseItemType::Next:
            if (isNextAllowed() && checkIfHit(item, x, y))
            {
                item->setFocused(true);
                return true;
            }
            break;
        default:
            break;
        }
    }

    return false;
}

bool Menu::handleMousePressOnItem(int x, int y)
{
    for (MouseItem mouseItem : m_mouseItems)
    {
        auto item = mouseItem.item;
        if (checkIfHit(item, x, y))
        {
            item->setFocused(true);
            return true;
        }
    }

    if (m_style != Menu::Style::ShowOne)
    {
        for (unsigned int i = 0; i < m_items.size(); i++)
        {
            auto item = m_items.at(i);
            if (checkIfHit(item, x, y))
            {
                setCurrentIndex(i);
                return true;
            }
        }
    }

    return false;
}

bool Menu::handleMousePress(int x, int y, int screenWidth, int screenHeight)
{
    x = x * width() / screenWidth;
    y = y * height() / screenHeight;

    if (handleMousePressOnMouseItem(x, y) || handleMousePressOnItem(x, y))
    {
        return true;
    }

    return false;
}

bool Menu::handleMouseReleaseOnMouseItem(int x, int y)
{
    for (MouseItem mouseItem : m_mouseItems)
    {
        mouseItem.item->setFocused(false);
    }

    for (MouseItem mouseItem : m_mouseItems)
    {
        auto item = mouseItem.item;
        if (checkIfHit(item, x, y))
        {
            switch (mouseItem.type)
            {
            case Menu::MouseItemType::Quit:
                exit();
                return true;

            case Menu::MouseItemType::Prev:
                if (isPrevAllowed())
                {
                    left();
                    return true;
                }
                break;
            case Menu::MouseItemType::Next:
                if (isNextAllowed())
                {
                    right();
                    return true;
                }
                break;
            }
        }
    }

    return false;
}

bool Menu::handleMouseReleaseOnItem(int x, int y)
{
    if (m_style == Menu::Style::ShowOne)
    {
        auto item = currentItem();
        if (checkIfHit(item, x, y))
        {
            selectCurrentItem();
            return true;
        }
    }
    else
    {
        for (unsigned int i = 0; i < m_items.size(); i++)
        {
            auto item = m_items.at(i);
            if (checkIfHit(item, x, y))
            {
                if (static_cast<int>(i) == m_currentIndex)
                {
                    selectCurrentItem();
                }

                return true;
            }
        }
    }

    return false;
}

bool Menu::handleMouseRelease(int x, int y, int screenWidth, int screenHeight)
{
    x = x * width() / screenWidth;
    y = y * height() / screenHeight;

    if (handleMouseReleaseOnMouseItem(x, y) || handleMouseReleaseOnItem(x, y))
    {
        return true;
    }

    return false;
}

void Menu::updateFocus()
{
    if (m_items.size())
    {
        for (auto item : m_items)
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

void Menu::setPos(float x, float y)
{
    m_x = x;
    m_y = y;
    m_targetX = x;
    m_targetY = y;
}

void Menu::setPos(float x, float y, float targetX, float targetY)
{
    m_x = x;
    m_y = y;
    m_targetX = targetX;
    m_targetY = targetY;
}

float Menu::x() const
{
    return m_x;
}

float Menu::y() const
{
    return m_y;
}

float Menu::targetY() const
{
    return m_targetY;
}

float Menu::targetX() const
{
    return m_targetX;
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
        m_currentIndex = m_currentIndex < 0 ? 0 : m_currentIndex;
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
        m_currentIndex = m_currentIndex < 0 ? numItems - 1 : m_currentIndex;
        m_currentIndex = m_currentIndex >= numItems ? 0 : m_currentIndex;

        updateFocus();
    }
}

void Menu::setItemsToShow(const std::vector<int> & itemsToShow)
{
    m_itemsToShow = itemsToShow;
}

bool Menu::isDone() const
{
    return m_isDone;
}

void Menu::setIsDone(bool done)
{
    m_isDone = done;
}

void Menu::enter()
{
    m_isDone = false;

    m_animationCurve.reset();

    updateFocus();
}

void Menu::pushEnter()
{
    m_isDone = false;

    m_animationCurve.reset();

    updateFocus();
}

void Menu::pushExit()
{
    m_animationCurve.reset();
}

void Menu::popEnter()
{
    m_isDone = false;

    m_animationCurve.reset();

    updateFocus();
}

void Menu::popExit()
{
    m_animationCurve.reset();
}

void Menu::setWrapAround(bool wrapAround)
{
    m_wrapAround = wrapAround;
}

void Menu::exit()
{
    MenuManager::instance().popMenu();
}

void Menu::stepTime(int msecs)
{
    for (auto item : m_items)
    {
        item->stepTime(msecs);
    }

    positionAnimation(msecs);
}

void Menu::positionAnimation(int)
{
    m_animationCurve.step();

    m_x = m_x + (m_targetX - m_x) * m_animationCurve.value();
    m_y = m_y + (m_targetY - m_y) * m_animationCurve.value();
}

Menu::~Menu()
{
}

} // namespace MTFH
