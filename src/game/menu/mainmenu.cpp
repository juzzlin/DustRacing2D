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

#include "credits.hpp"
#include "difficultymenu.hpp"
#include "help.hpp"
#include "lapcountmenu.hpp"
#include "renderer.hpp"
#include "settingsmenu.hpp"
#include "textmenuitemview.hpp"
#include "trackselectionmenu.hpp"

#include <MenuItem>
#include <MenuItemAction>
#include <MenuItemView>
#include <MenuManager>

#include <MCSurface>
#include <MCSurfaceManager>

#include "simple_logger.hpp"

static const int ITEM_TEXT_SIZE = 40;

std::string MainMenu::MenuId = "main";

MainMenu::MainMenu(MTFH::MenuManager & menuManager, Scene & scene, int width, int height)
  : SurfaceMenu("mainMenuBack", MenuId, width, height, Menu::Style::VerticalList, false)
  , m_menuManager(menuManager)
  , m_scene(scene)
{
    createMenuItems();
    createSubMenus();
}

void MainMenu::createMenuItems()
{
    using MTFH::MenuItem;

    const int itemHeight = height() / 8;
    const int textSize = ITEM_TEXT_SIZE;

    const auto play = std::make_shared<MenuItem>(width(), itemHeight, tr("Play").toUpper().toStdWString());
    play->setView(std::make_shared<TextMenuItemView>(textSize, *play));
    play->setMenuOpenAction("difficulty");

    const auto help = std::make_shared<MenuItem>(width(), itemHeight, tr("Help").toUpper().toStdWString());
    help->setView(std::make_shared<TextMenuItemView>(textSize, *help));
    help->setMenuOpenAction("help");

    const auto credits = std::make_shared<MenuItem>(width(), itemHeight, tr("Credits").toUpper().toStdWString());
    credits->setView(std::make_shared<TextMenuItemView>(textSize, *credits));
    credits->setMenuOpenAction("credits");

    const auto quit = std::make_shared<MenuItem>(width(), itemHeight, tr("Quit").toUpper().toStdWString());
    quit->setView(std::make_shared<TextMenuItemView>(textSize, *quit));
    quit->setAction(
      [this]() {
          juzzlin::L().info() << "Quit selected from the main menu.";
          emit exitGameRequested();
      });

    const auto settings = std::make_shared<MenuItem>(width(), itemHeight, tr("Settings").toUpper().toStdWString());
    settings->setView(std::make_shared<TextMenuItemView>(textSize, *settings));
    settings->setMenuOpenAction("settings");

    addItem(quit);
    addItem(credits);
    addItem(settings);
    addItem(help);
    addItem(play);
}

void MainMenu::createSubMenus()
{
    m_menuManager.addMenu(std::make_shared<Help>("help", width(), height()));

    m_menuManager.addMenu(std::make_shared<Credits>("credits", width(), height()));

    m_menuManager.addMenu(std::make_shared<LapCountMenu>(width(), height()));

    m_menuManager.addMenu(std::make_shared<SettingsMenu>("settings", width(), height()));

    m_menuManager.addMenu(std::make_shared<TrackSelectionMenu>(width(), height(), m_scene));

    m_menuManager.addMenu(std::make_shared<DifficultyMenu>(width(), height()));
}
