// This file is part of Dust Racing 2D.
// Copyright (C) 2017 Jussi Lind <jussi.lind@iki.fi>
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

#include "difficultymenu.hpp"

#include "difficultyprofile.hpp"
#include "game.hpp"
#include "lapcountmenu.hpp"
#include "settings.hpp"
#include "textmenuitemview.hpp"

#include <MenuItem>
#include <MenuManager>

#include "simple_logger.hpp"

static const int ITEM_TEXT_SIZE = 40;

std::string DifficultyMenu::MenuId = "difficulty";

DifficultyMenu::DifficultyMenu(int width, int height)
  : SurfaceMenu("trackSelectionBack", MenuId, width, height, Menu::Style::VerticalList)
{
    const int itemHeight = height / 8;
    const int textSize = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuManager;

    const auto easyItem = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Easy").toUpper().toStdWString());
    easyItem->setView(std::make_shared<TextMenuItemView>(textSize, *easyItem));
    easyItem->setAction([]() {
        juzzlin::L().info() << "Easy selected.";
        const DifficultyProfile::Difficulty chosenDifficulty = DifficultyProfile::Difficulty::Easy;
        Settings::instance().saveDifficulty(chosenDifficulty);
        Game::instance().difficultyProfile().setDifficulty(chosenDifficulty);
        MenuManager::instance().pushMenu(LapCountMenu::MenuId);
    });

    const auto mediumItem = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Medium").toUpper().toStdWString());
    mediumItem->setView(std::make_shared<TextMenuItemView>(textSize, *mediumItem));
    mediumItem->setAction([]() {
        juzzlin::L().info() << "Medium selected.";
        const DifficultyProfile::Difficulty chosenDifficulty = DifficultyProfile::Difficulty::Medium;
        Settings::instance().saveDifficulty(chosenDifficulty);
        Game::instance().difficultyProfile().setDifficulty(chosenDifficulty);
        MenuManager::instance().pushMenu(LapCountMenu::MenuId);
    });

    const auto hardItem = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Hard").toUpper().toStdWString());
    hardItem->setView(std::make_shared<TextMenuItemView>(textSize, *hardItem));
    hardItem->setAction([]() {
        juzzlin::L().info() << "Hard selected.";
        const DifficultyProfile::Difficulty chosenDifficulty = DifficultyProfile::Difficulty::Hard;
        Settings::instance().saveDifficulty(chosenDifficulty);
        Game::instance().difficultyProfile().setDifficulty(chosenDifficulty);
        MenuManager::instance().pushMenu(LapCountMenu::MenuId);
    });

    addItem(hardItem);
    addItem(mediumItem);
    addItem(easyItem);

    const DifficultyProfile::Difficulty difficulty = Game::instance().difficultyProfile().difficulty();
    switch (difficulty)
    {
    case DifficultyProfile::Difficulty::Easy:
        easyItem->setCurrent();
        break;

    case DifficultyProfile::Difficulty::Medium:
        mediumItem->setCurrent();
        break;

    case DifficultyProfile::Difficulty::Hard:
        hardItem->setCurrent();
        break;
    }
}
