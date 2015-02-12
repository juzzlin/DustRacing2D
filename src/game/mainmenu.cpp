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

#include "mainmenu.hpp"
#include "renderer.hpp"
#include "textmenuitemview.hpp"

#include <MenuItem>
#include <MenuItemAction>
#include <MenuItemView>
#include <MenuManager>

#include <MCLogger>
#include <MCSurface>
#include <MCSurfaceManager>

MainMenu::MainMenu(std::string id, int width, int height)
    : SurfaceMenu("mainMenuBack", id, width, height, Menu::Style::VerticalList, false)
{
    using MTFH::MenuItem;

    const int itemHeight = height / 8;

    MenuItem * play = new MenuItem(width, itemHeight, tr("Play").toStdWString());
    play->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(40, *play)));
    play->setMenuOpenAction("trackSelection");

    MenuItem * help = new MenuItem(width, itemHeight, tr("Help").toStdWString());
    help->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(40, *help)));
    help->setMenuOpenAction("help");

    MenuItem * credits = new MenuItem(width, itemHeight, tr("Credits").toStdWString());
    credits->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(40, *credits)));
    credits->setMenuOpenAction("credits");

    MTFH::MenuItem * quit = new MenuItem(width, itemHeight, tr("Quit").toStdWString());
    quit->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(40, *quit)));
    quit->setAction(
        [this]()
        {
            MCLogger().info() << "Quit selected from the main menu.";
            emit exitGameRequested();
        });

    MenuItem * settings = new MenuItem(width, itemHeight, tr("Settings").toStdWString());
    settings->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(40, *settings)));
    settings->setMenuOpenAction("settings");

    addItem(MTFH::MenuItemPtr(quit));
    addItem(MTFH::MenuItemPtr(credits));
    addItem(MTFH::MenuItemPtr(settings));
    addItem(MTFH::MenuItemPtr(help));
    addItem(MTFH::MenuItemPtr(play));
}
