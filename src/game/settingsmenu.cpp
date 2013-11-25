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

#include "settingsmenu.hpp"

#include "game.hpp"
#include "renderer.hpp"
#include "settings.hpp"
#include "textmenuitemview.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "trackloader.hpp"

#include <MenuItem>
#include <MenuItemAction>
#include <MenuItemView>
#include <MenuManager>

#include <MCLogger>

#include <sstream>

#include <QObject> // For QObject::tr()

static const QString FPS_KEY(Settings::fpsKey());
static const QString LAP_COUNT_KEY(Settings::lapCountKey());

class ResetAction : public MTFH::MenuItemAction
{
public:

    enum ResetType {RT_TIMES, RT_POSITIONS, RT_TRACKS};

    ResetAction(ResetType type, ConfirmationMenu & menu)
    : m_type(type)
    , m_confirmationMenu(menu)
    {
    }

private:

    //! \reimp
    void fire()
    {
        using MTFH::MenuManager;

        switch (m_type)
        {
        case RT_POSITIONS:
            MenuManager::instance().pushMenu(m_confirmationMenu.id());
            m_confirmationMenu.setText(QObject::tr("Reset best positions?").toStdString());
            m_confirmationMenu.setAcceptAction(
                []()
                {
                    MCLogger().info() << "Reset positions selected.";
                    Settings::instance().resetBestPos();
                });
            m_confirmationMenu.setCurrentIndex(1);
            break;

        case RT_TIMES:
            MenuManager::instance().pushMenu(m_confirmationMenu.id());
            m_confirmationMenu.setText(QObject::tr("Reset record times?").toStdString());
            m_confirmationMenu.setAcceptAction(
                []()
                {
                    MCLogger().info() << "Reset times selected.";
                    Settings::instance().resetLapRecords();
                    Settings::instance().resetRaceRecords();
                });
            m_confirmationMenu.setCurrentIndex(1);
            break;

        case RT_TRACKS:
            MenuManager::instance().pushMenu(m_confirmationMenu.id());
            m_confirmationMenu.setText(QObject::tr("Reset unlocked tracks?").toStdString());
            m_confirmationMenu.setAcceptAction(
                []()
                {
                    MCLogger().info() << "Reset tracks selected.";
                    TrackLoader & tl = TrackLoader::instance();
                    for (unsigned int i = 0; i < tl.tracks(); i++)
                    {
                        Track & track = *tl.track(i);
                        if (track.trackData().index() > 0)
                        {
                            track.trackData().setIsLocked(true);
                        }
                }
                Settings::instance().resetTrackUnlockStatuses();});
            m_confirmationMenu.setCurrentIndex(1);
            break;

        default:
            break;
        }
    }

    ResetType          m_type;
    ConfirmationMenu & m_confirmationMenu;
};

static const char * CONFIRMATION_MENU_ID           = "confirmationMenu";
static const char * FPS_MENU_ID                    = "fpsMenu";
static const char * FULL_SCREEN_RESOLUTION_MENU_ID = "fullScreenResolutionMenu";
static const char * GAME_MODE_MENU_ID              = "gameModeMenu";
static const char * KEY_CONFIG_MENU_ID             = "keyConfigMenu";
static const char * LAP_COUNT_MENU_ID              = "lapCountMenu";
static const char * SPLIT_TYPE_MENU_ID             = "splitTypeMenu";
static const char * WINDOWED_RESOLUTION_MENU_ID    = "windowedResolutionMenu";

SettingsMenu::SettingsMenu(std::string id, int width, int height)
: SurfaceMenu("settingsBack", id, width, height, Menu::MS_VERTICAL_LIST)
, m_confirmationMenu(CONFIRMATION_MENU_ID, width, height)
, m_fullScreenResolutionMenu(m_confirmationMenu, FULL_SCREEN_RESOLUTION_MENU_ID, width, height, true)
, m_windowedResolutionMenu(m_confirmationMenu, WINDOWED_RESOLUTION_MENU_ID, width, height, false)
, m_gameModeMenu("settingsBack", GAME_MODE_MENU_ID, width, height, Menu::MS_VERTICAL_LIST)
, m_splitTypeMenu("settingsBack", SPLIT_TYPE_MENU_ID, width, height, Menu::MS_VERTICAL_LIST)
, m_lapCountMenu("settingsBack", LAP_COUNT_MENU_ID, width, height, Menu::MS_VERTICAL_LIST)
, m_fpsMenu("settingsBack", FPS_MENU_ID, width, height, Menu::MS_VERTICAL_LIST)
, m_keyConfigMenu(KEY_CONFIG_MENU_ID, width, height)
{
    populate(width, height);
    populateGameModeMenu(width, height);
    populateSplitTypeMenu(width, height);
    populateFpsMenu(width, height);
    populateLapCountMenu(width, height);

    using MTFH::MenuManager;

    MenuManager::instance().addMenu(m_confirmationMenu);
    MenuManager::instance().addMenu(m_windowedResolutionMenu);
    MenuManager::instance().addMenu(m_fullScreenResolutionMenu);
    MenuManager::instance().addMenu(m_gameModeMenu);
    MenuManager::instance().addMenu(m_splitTypeMenu);
    MenuManager::instance().addMenu(m_lapCountMenu);
    MenuManager::instance().addMenu(m_fpsMenu);
    MenuManager::instance().addMenu(m_keyConfigMenu);
}

void SettingsMenu::populate(int width, int height)
{
    const int numItems   = 9;
    const int itemHeight = height / (numItems + 2);
    const int textSize   = 20;

    using MTFH::MenuItem;
    using MTFH::MenuManager;
    using MTFH::MenuItemViewPtr;
    using MTFH::MenuItemActionPtr;

    MenuItem * resetRecordTimes = new MenuItem(width, itemHeight, QObject::tr("Reset record times").toStdString());
    resetRecordTimes->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *resetRecordTimes)));
    resetRecordTimes->setAction(
        MenuItemActionPtr(new ResetAction(ResetAction::RT_TIMES, m_confirmationMenu)));

    MenuItem * resetBestPositions = new MenuItem(width, itemHeight, QObject::tr("Reset best positions").toStdString());
    resetBestPositions->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *resetBestPositions)));
    resetBestPositions->setAction(
        MenuItemActionPtr(new ResetAction(ResetAction::RT_POSITIONS, m_confirmationMenu)));

    MenuItem * resetUnlockedTracks = new MenuItem(width, itemHeight, QObject::tr("Reset unlocked tracks").toStdString());
    resetUnlockedTracks->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *resetUnlockedTracks)));
    resetUnlockedTracks->setAction(
        MenuItemActionPtr(new ResetAction(ResetAction::RT_TRACKS, m_confirmationMenu)));

    MenuItem * selectFullScreenResolution = new MenuItem(width, itemHeight, QObject::tr("Full screen resolution >").toStdString());
    selectFullScreenResolution->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *selectFullScreenResolution)));
    selectFullScreenResolution->setMenuOpenAction(FULL_SCREEN_RESOLUTION_MENU_ID);

    MenuItem * selectWindowedResolution = new MenuItem(width, itemHeight, QObject::tr("Windowed resolution >").toStdString());
    selectWindowedResolution->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *selectWindowedResolution)));
    selectWindowedResolution->setMenuOpenAction(WINDOWED_RESOLUTION_MENU_ID);

    MenuItem * splitType = new MenuItem(width, itemHeight, QObject::tr("Split type >").toStdString());
    splitType->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *splitType)));
    splitType->setMenuOpenAction(SPLIT_TYPE_MENU_ID);

    MenuItem * gameMode = new MenuItem(width, itemHeight, QObject::tr("Game mode >").toStdString());
    gameMode->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *gameMode)));
    gameMode->setMenuOpenAction(GAME_MODE_MENU_ID);

    MenuItem * lapCount = new MenuItem(width, itemHeight, QObject::tr("Lap Count >").toStdString());
    lapCount->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *lapCount)));
    lapCount->setMenuOpenAction(LAP_COUNT_MENU_ID);

    MenuItem * selectFps = new MenuItem(width, itemHeight, QObject::tr("FPS >").toStdString());
    selectFps->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *selectFps)));
    selectFps->setMenuOpenAction(FPS_MENU_ID);

    MenuItem * configureKeys = new MenuItem(width, itemHeight, QObject::tr("Key configuration >").toStdString());
    configureKeys->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *configureKeys)));
    configureKeys->setMenuOpenAction(KEY_CONFIG_MENU_ID);

    using MTFH::MenuItemPtr;

    addItem(MenuItemPtr(resetRecordTimes));
    addItem(MenuItemPtr(resetBestPositions));
    addItem(MenuItemPtr(resetUnlockedTracks));
    addItem(MenuItemPtr(configureKeys));
    addItem(MenuItemPtr(selectFps));
    addItem(MenuItemPtr(selectWindowedResolution));
    addItem(MenuItemPtr(selectFullScreenResolution));
    addItem(MenuItemPtr(lapCount));
    addItem(MenuItemPtr(splitType));
    addItem(MenuItemPtr(gameMode));
}

void SettingsMenu::populateGameModeMenu(int width, int height)
{
    const int numItems   = 4;
    const int itemHeight = height / (numItems + 4);

    using MTFH::MenuItem;
    using MTFH::MenuManager;
    using MTFH::MenuItemViewPtr;

    MenuItem * twoPlayers = new MenuItem(width, itemHeight, QObject::tr("Two player race").toStdString());
    twoPlayers->setView(MenuItemViewPtr(new TextMenuItemView(20, *twoPlayers)));
    twoPlayers->setAction(
        []()
        {
            MCLogger().info() << "Two player race selected.";
            Game::instance().setMode(Game::TwoPlayerRace);
            MenuManager::instance().popMenu();
        });

    MenuItem * onePlayer = new MenuItem(width, itemHeight, QObject::tr("One player race").toStdString());
    onePlayer->setView(MenuItemViewPtr(new TextMenuItemView(20, *onePlayer)));
    onePlayer->setAction(
        []()
        {
            MCLogger().info() << "One player race selected.";
            Game::instance().setMode(Game::OnePlayerRace);
            MenuManager::instance().popMenu();
        });

    MenuItem * timeTrial = new MenuItem(width, itemHeight, QObject::tr("Time Trial").toStdString());
    timeTrial->setView(MenuItemViewPtr(new TextMenuItemView(20, *timeTrial)));
    timeTrial->setAction(
        []()
        {
            MCLogger().info() << "Time Trial selected.";
            Game::instance().setMode(Game::TimeTrial);
            MenuManager::instance().popMenu();
        });

    MenuItem * duel = new MenuItem(width, itemHeight, QObject::tr("Duel").toStdString());
    duel->setView(MenuItemViewPtr(new TextMenuItemView(20, *duel)));
    duel->setAction(
        []()
        {
            MCLogger().info() << "Duel selected.";
            Game::instance().setMode(Game::Duel);
            MenuManager::instance().popMenu();
        });

    using MTFH::MenuItemPtr;

    m_gameModeMenu.addItem(MenuItemPtr(duel));
    m_gameModeMenu.addItem(MenuItemPtr(timeTrial));
    m_gameModeMenu.addItem(MenuItemPtr(twoPlayers));
    m_gameModeMenu.addItem(MenuItemPtr(onePlayer));
}

void SettingsMenu::populateSplitTypeMenu(int width, int height)
{
    const int numItems   = 2;
    const int itemHeight = height / (numItems + 4);

    using MTFH::MenuItem;
    using MTFH::MenuManager;
    using MTFH::MenuItemViewPtr;

    MenuItem * vertical = new MenuItem(width, itemHeight, QObject::tr("Vertical").toStdString());
    vertical->setView(MenuItemViewPtr(new TextMenuItemView(20, *vertical)));
    vertical->setAction(
        []()
        {
            MCLogger().info() << "Vertical split selected.";
            Game::instance().setSplitType(Game::Vertical);
            MenuManager::instance().popMenu();
        });

    MenuItem * horizontal = new MenuItem(width, itemHeight, QObject::tr("Horizontal").toStdString());
    horizontal->setView(MenuItemViewPtr(new TextMenuItemView(20, *horizontal)));
    horizontal->setAction(
        []()
        {
            MCLogger().info() << "Horizontal split selected.";
            Game::instance().setSplitType(Game::Horizontal);
            MenuManager::instance().popMenu();
        });

    m_splitTypeMenu.addItem(MTFH::MenuItemPtr(horizontal));
    m_splitTypeMenu.addItem(MTFH::MenuItemPtr(vertical));
}

void SettingsMenu::populateFpsMenu(int width, int height)
{
    const int numItems   = 2;
    const int itemHeight = height / (numItems + 6);

    using MTFH::MenuItem;
    using MTFH::MenuManager;
    using MTFH::MenuItemViewPtr;

    MenuItem * fps30 = new MenuItem(width, itemHeight, QObject::tr("30 fps").toStdString());
    fps30->setView(MenuItemViewPtr(new TextMenuItemView(20, *fps30)));
    fps30->setAction(
        []()
        {
            MCLogger().info() << "30 fps selected.";
            Game::instance().setFps(30);
            Settings::instance().saveValue(FPS_KEY, 30);
            MenuManager::instance().popMenu();
        });

    MenuItem * fps60 = new MenuItem(width, itemHeight, QObject::tr("60 fps").toStdString());
    fps60->setView(MenuItemViewPtr(new TextMenuItemView(20, *fps60)));
    fps60->setAction(
        []()
        {
            MCLogger().info() << "60 fps selected.";
            Game::instance().setFps(60);
            Settings::instance().saveValue(FPS_KEY, 60);
            MenuManager::instance().popMenu();
        });

    m_fpsMenu.addItem(MTFH::MenuItemPtr(fps30));
    m_fpsMenu.addItem(MTFH::MenuItemPtr(fps60));

    const int fps = Settings::instance().loadValue(FPS_KEY, 60);
    if (fps == 30)
    {
        m_fpsMenu.setCurrentIndex(fps30->index());
    }
    else
    {
        m_fpsMenu.setCurrentIndex(fps60->index());
    }
}

void SettingsMenu::populateLapCountMenu(int width, int height)
{
    static int LAP_COUNTS[] =
    {
        100, 50, 25, 20, 15, 10, 5, 3, 1
    };

    const int numLapCounts = sizeof(LAP_COUNTS) / sizeof(int);
    const int itemHeight   = height / (numLapCounts + 2);

    using MTFH::MenuItem;
    using MTFH::MenuManager;
    using MTFH::MenuItemViewPtr;

    int selectedIndex = 0;
    for (int i = 0; i < numLapCounts; i++)
    {
        std::stringstream itemText;
        itemText << LAP_COUNTS[i];

        MenuItem * lapCountItem = new MenuItem(width, itemHeight, itemText.str());
        lapCountItem->setView(MenuItemViewPtr(new TextMenuItemView(20, *lapCountItem)));
        lapCountItem->setAction(
            [i]()
            {
                MCLogger().info() << LAP_COUNTS[i] << " laps selected.";
                Game::instance().setLapCount(LAP_COUNTS[i]);
                Settings::instance().saveValue(LAP_COUNT_KEY, LAP_COUNTS[i]);
                MenuManager::instance().popMenu();
            });

        m_lapCountMenu.addItem(MTFH::MenuItemPtr(lapCountItem));

        if (Game::instance().lapCount() == LAP_COUNTS[i])
        {
            selectedIndex = lapCountItem->index();
        }
    }

    m_lapCountMenu.setCurrentIndex(selectedIndex);
}
