#include "difficultymenu.hpp"

#include "difficultyprofile.hpp"
#include "game.hpp"
#include "lapcountmenu.hpp"
#include "settings.hpp"
#include "textmenuitemview.hpp"

#include <MenuItem>
#include <MenuManager>

#include <MCLogger>

static const int ITEM_TEXT_SIZE = 40;

std::string DifficultyMenu::MenuId = "difficulty";

DifficultyMenu::DifficultyMenu(int width, int height)
    : SurfaceMenu("trackSelectionBack", MenuId, width, height, Menu::Style::VerticalList)
{
    const int itemHeight = height / 8;
    const int textSize = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuManager;
    using MTFH::MenuItemViewPtr;

    MenuItem * easyItem = new MenuItem(width, itemHeight, QObject::tr("Easy").toUpper().toStdWString());
    easyItem->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *easyItem)));
    easyItem->setAction([] () {
        MCLogger().info() << "Easy selected.";
        const DifficultyProfile::Difficulty chosenDifficulty = DifficultyProfile::Difficulty::Easy;
        Settings::instance().saveDifficulty(chosenDifficulty);
        Game::instance().difficultyProfile().setDifficulty(chosenDifficulty);
        MenuManager::instance().pushMenu(LapCountMenu::MenuId);
    });

    MenuItem * mediumItem = new MenuItem(width, itemHeight, QObject::tr("Medium").toUpper().toStdWString());
    mediumItem->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *mediumItem)));
    mediumItem->setAction([] () {
        MCLogger().info() << "Medium selected.";
        const DifficultyProfile::Difficulty chosenDifficulty = DifficultyProfile::Difficulty::Medium;
        Settings::instance().saveDifficulty(chosenDifficulty);
        Game::instance().difficultyProfile().setDifficulty(chosenDifficulty);
        MenuManager::instance().pushMenu(LapCountMenu::MenuId);
    });

    MenuItem * hardItem = new MenuItem(width, itemHeight, QObject::tr("Hard").toUpper().toStdWString());
    hardItem->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *hardItem)));
    hardItem->setAction([] () {
        MCLogger().info() << "Hard selected.";
        const DifficultyProfile::Difficulty chosenDifficulty = DifficultyProfile::Difficulty::Hard;
        Settings::instance().saveDifficulty(chosenDifficulty);
        Game::instance().difficultyProfile().setDifficulty(chosenDifficulty);
        MenuManager::instance().pushMenu(LapCountMenu::MenuId);
    });

    addItem(MTFH::MenuItemPtr(hardItem));
    addItem(MTFH::MenuItemPtr(mediumItem));
    addItem(MTFH::MenuItemPtr(easyItem));

    const DifficultyProfile::Difficulty difficulty = Game::instance().difficultyProfile().difficulty();
    switch (difficulty)
    {
    case DifficultyProfile::Difficulty::Easy:
        setCurrentIndex(easyItem->index());
        break;

    case DifficultyProfile::Difficulty::Medium:
        setCurrentIndex(mediumItem->index());
        break;

    case DifficultyProfile::Difficulty::Hard:
        setCurrentIndex(hardItem->index());
        break;
    }
}
