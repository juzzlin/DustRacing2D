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

#include "audioworker.hpp"
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

static const QString LAP_COUNT_KEY(Settings::lapCountKey());

static const int ITEM_HEIGHT_DIV = 10;
static const int ITEM_TEXT_SIZE  = 20;

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
            m_confirmationMenu.setText(QObject::tr("Reset best positions?").toStdWString());
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
            m_confirmationMenu.setText(QObject::tr("Reset record times?").toStdWString());
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
            m_confirmationMenu.setText(QObject::tr("Reset unlocked tracks?").toStdWString());
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
static const char * FULL_SCREEN_RESOLUTION_MENU_ID = "fullScreenResolutionMenu";
static const char * GAME_MODE_MENU_ID              = "gameModeMenu";
static const char * GFX_MENU_ID                    = "gfxMenu";
static const char * KEY_CONFIG_MENU_ID             = "keyConfigMenu";
static const char * LAP_COUNT_MENU_ID              = "lapCountMenu";
static const char * RESET_MENU_ID                  = "resetMenu";
static const char * SFX_MENU_ID                    = "sfxMenu";
static const char * SPLIT_TYPE_MENU_ID             = "splitTypeMenu";
static const char * VSYNC_MENU_ID                  = "vsyncMenu";
static const char * WINDOWED_RESOLUTION_MENU_ID    = "windowedResolutionMenu";

SettingsMenu::SettingsMenu(std::string id, int width, int height)
: SurfaceMenu("settingsBack", id, width, height, Menu::MS_VERTICAL_LIST)
, m_confirmationMenu(CONFIRMATION_MENU_ID, width, height)
, m_fullScreenResolutionMenu(m_confirmationMenu, FULL_SCREEN_RESOLUTION_MENU_ID, width, height, true)
, m_windowedResolutionMenu(m_confirmationMenu, WINDOWED_RESOLUTION_MENU_ID, width, height, false)
, m_gameModeMenu("settingsBack",  GAME_MODE_MENU_ID,  width, height, Menu::MS_VERTICAL_LIST)
, m_gfxMenu("settingsBack",       GFX_MENU_ID,        width, height, Menu::MS_VERTICAL_LIST)
, m_lapCountMenu("settingsBack",  LAP_COUNT_MENU_ID,  width, height, Menu::MS_VERTICAL_LIST)
, m_resetMenu("settingsBack",     RESET_MENU_ID,      width, height, Menu::MS_VERTICAL_LIST)
, m_sfxMenu("settingsBack",       SFX_MENU_ID,        width, height, Menu::MS_VERTICAL_LIST)
, m_splitTypeMenu("settingsBack", SPLIT_TYPE_MENU_ID, width, height, Menu::MS_VERTICAL_LIST)
, m_vsyncMenu(m_confirmationMenu, VSYNC_MENU_ID,      width, height)
, m_keyConfigMenu(KEY_CONFIG_MENU_ID, width, height)
{
    populate              (width, height);
    populateGameModeMenu  (width, height);
    populateGfxMenu       (width, height);
    populateLapCountMenu  (width, height);
    populateResetMenu     (width, height);
    populateSfxMenu       (width, height);
    populateSplitTypeMenu (width, height);

    using MTFH::MenuManager;

    MenuManager::instance().addMenu(m_confirmationMenu);
    MenuManager::instance().addMenu(m_fullScreenResolutionMenu);
    MenuManager::instance().addMenu(m_gameModeMenu);
    MenuManager::instance().addMenu(m_gfxMenu);
    MenuManager::instance().addMenu(m_keyConfigMenu);
    MenuManager::instance().addMenu(m_lapCountMenu);
    MenuManager::instance().addMenu(m_resetMenu);
    MenuManager::instance().addMenu(m_sfxMenu);
    MenuManager::instance().addMenu(m_splitTypeMenu);
    MenuManager::instance().addMenu(m_vsyncMenu);
    MenuManager::instance().addMenu(m_windowedResolutionMenu);
}

void SettingsMenu::populate(int width, int height)
{
    const int itemHeight = height / ITEM_HEIGHT_DIV;
    const int textSize   = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuManager;
    using MTFH::MenuItemViewPtr;
    using MTFH::MenuItemActionPtr;

    MenuItem * gameMode = new MenuItem(width, itemHeight, QObject::tr("Game mode >").toStdWString());
    gameMode->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *gameMode)));
    gameMode->setMenuOpenAction(GAME_MODE_MENU_ID);

    MenuItem * lapCount = new MenuItem(width, itemHeight, QObject::tr("Lap Count >").toStdWString());
    lapCount->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *lapCount)));
    lapCount->setMenuOpenAction(LAP_COUNT_MENU_ID);

    MenuItem * sfx = new MenuItem(width, itemHeight, QObject::tr("Sounds >").toStdWString());
    sfx->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *sfx)));
    sfx->setMenuOpenAction(SFX_MENU_ID);

    MenuItem * gfx = new MenuItem(width, itemHeight, QObject::tr("GFX >").toStdWString());
    gfx->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *gfx)));
    gfx->setMenuOpenAction(GFX_MENU_ID);

    MenuItem * configureKeys = new MenuItem(width, itemHeight, QObject::tr("Controls >").toStdWString());
    configureKeys->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *configureKeys)));
    configureKeys->setMenuOpenAction(KEY_CONFIG_MENU_ID);

    MenuItem * reset = new MenuItem(width, itemHeight, QObject::tr("Reset >").toStdWString());
    reset->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *reset)));
    reset->setMenuOpenAction(RESET_MENU_ID);

    using MTFH::MenuItemPtr;

    addItem(MenuItemPtr(reset));
    addItem(MenuItemPtr(configureKeys));
    addItem(MenuItemPtr(sfx));
    addItem(MenuItemPtr(gfx));
    addItem(MenuItemPtr(lapCount));
    addItem(MenuItemPtr(gameMode));
}

void SettingsMenu::populateGameModeMenu(int width, int height)
{
    const int itemHeight = height / ITEM_HEIGHT_DIV;
    const int textSize   = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuManager;
    using MTFH::MenuItemViewPtr;

    MenuItem * twoPlayers = new MenuItem(width, itemHeight, QObject::tr("Two player race").toStdWString());
    twoPlayers->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *twoPlayers)));
    twoPlayers->setAction(
        []()
        {
            MCLogger().info() << "Two player race selected.";
            Game::instance().setMode(Game::TwoPlayerRace);
            MenuManager::instance().popMenu();
        });

    MenuItem * onePlayer = new MenuItem(width, itemHeight, QObject::tr("One player race").toStdWString());
    onePlayer->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *onePlayer)));
    onePlayer->setAction(
        []()
        {
            MCLogger().info() << "One player race selected.";
            Game::instance().setMode(Game::OnePlayerRace);
            MenuManager::instance().popMenu();
        });

    MenuItem * timeTrial = new MenuItem(width, itemHeight, QObject::tr("Time Trial").toStdWString());
    timeTrial->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *timeTrial)));
    timeTrial->setAction(
        []()
        {
            MCLogger().info() << "Time Trial selected.";
            Game::instance().setMode(Game::TimeTrial);
            MenuManager::instance().popMenu();
        });

    MenuItem * duel = new MenuItem(width, itemHeight, QObject::tr("Duel").toStdWString());
    duel->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *duel)));
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
    const int textSize   = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuManager;
    using MTFH::MenuItemViewPtr;

    MenuItem * vertical = new MenuItem(width, itemHeight, QObject::tr("Vertical").toStdWString());
    vertical->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *vertical)));
    vertical->setAction(
        []()
        {
            MCLogger().info() << "Vertical split selected.";
            Game::instance().setSplitType(Game::Vertical);
            MenuManager::instance().popMenu();
        });

    MenuItem * horizontal = new MenuItem(width, itemHeight, QObject::tr("Horizontal").toStdWString());
    horizontal->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *horizontal)));
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

void SettingsMenu::populateGfxMenu(int width, int height)
{
    const int itemHeight = height / ITEM_HEIGHT_DIV;
    const int textSize   = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuItemPtr;
    using MTFH::MenuManager;
    using MTFH::MenuItemViewPtr;

    MenuItem * selectFullScreenResolution = new MenuItem(width, itemHeight, QObject::tr("Full screen resolution >").toStdWString());
    selectFullScreenResolution->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *selectFullScreenResolution)));
    selectFullScreenResolution->setMenuOpenAction(FULL_SCREEN_RESOLUTION_MENU_ID);

    MenuItem * selectWindowedResolution = new MenuItem(width, itemHeight, QObject::tr("Windowed resolution >").toStdWString());
    selectWindowedResolution->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *selectWindowedResolution)));
    selectWindowedResolution->setMenuOpenAction(WINDOWED_RESOLUTION_MENU_ID);

    MenuItem * splitType = new MenuItem(width, itemHeight, QObject::tr("Split type >").toStdWString());
    splitType->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *splitType)));
    splitType->setMenuOpenAction(SPLIT_TYPE_MENU_ID);

    MenuItem * vsync = new MenuItem(width, itemHeight, QObject::tr("VSync >").toStdWString());
    vsync->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *vsync)));
    vsync->setMenuOpenAction(VSYNC_MENU_ID);

    m_gfxMenu.addItem(MenuItemPtr(vsync));
    m_gfxMenu.addItem(MenuItemPtr(splitType));
    m_gfxMenu.addItem(MenuItemPtr(selectWindowedResolution));
    m_gfxMenu.addItem(MenuItemPtr(selectFullScreenResolution));
}

void SettingsMenu::populateSfxMenu(int width, int height)
{
    const int itemHeight = height / ITEM_HEIGHT_DIV;
    const int textSize   = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuManager;
    using MTFH::MenuItemViewPtr;

    MenuItem * offItem = new MenuItem(width, itemHeight, QObject::tr("Off").toStdWString());
    offItem->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *offItem)));
    offItem->setAction(
        []()
        {
            MCLogger().info() << "Sounds off selected.";
            Game::instance().audioWorker().setEnabled(false);
            Settings::instance().saveValue(Settings::soundsKey(), false);
            MenuManager::instance().popMenu();
        });

    m_sfxMenu.addItem(MTFH::MenuItemPtr(offItem));

    MenuItem * onItem = new MenuItem(width, itemHeight, QObject::tr("On").toStdWString());
    onItem->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *onItem)));
    onItem->setAction(
        []()
        {
            MCLogger().info() << "Sounds on selected.";
            Game::instance().audioWorker().setEnabled(true);
            Settings::instance().saveValue(Settings::soundsKey(), true);
            MenuManager::instance().popMenu();
        });

    m_sfxMenu.addItem(MTFH::MenuItemPtr(onItem));

    if (Game::instance().audioWorker().enabled())
    {
        m_sfxMenu.setCurrentIndex(onItem->index());
    }
    else
    {
        m_sfxMenu.setCurrentIndex(offItem->index());
    }
}

void SettingsMenu::populateResetMenu(int width, int height)
{
    const int itemHeight = height / ITEM_HEIGHT_DIV;
    const int textSize   = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuItemPtr;
    using MTFH::MenuItemActionPtr;
    using MTFH::MenuManager;
    using MTFH::MenuItemViewPtr;

    MenuItem * resetRecordTimes = new MenuItem(width, itemHeight, QObject::tr("Reset record times").toStdWString());
    resetRecordTimes->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *resetRecordTimes)));
    resetRecordTimes->setAction(
        MenuItemActionPtr(new ResetAction(ResetAction::RT_TIMES, m_confirmationMenu)));

    MenuItem * resetBestPositions = new MenuItem(width, itemHeight, QObject::tr("Reset best positions").toStdWString());
    resetBestPositions->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *resetBestPositions)));
    resetBestPositions->setAction(
        MenuItemActionPtr(new ResetAction(ResetAction::RT_POSITIONS, m_confirmationMenu)));

    MenuItem * resetUnlockedTracks = new MenuItem(width, itemHeight, QObject::tr("Reset unlocked tracks").toStdWString());
    resetUnlockedTracks->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *resetUnlockedTracks)));
    resetUnlockedTracks->setAction(
        MenuItemActionPtr(new ResetAction(ResetAction::RT_TRACKS, m_confirmationMenu)));

    m_resetMenu.addItem(MenuItemPtr(resetRecordTimes));
    m_resetMenu.addItem(MenuItemPtr(resetBestPositions));
    m_resetMenu.addItem(MenuItemPtr(resetUnlockedTracks));
}

void SettingsMenu::populateLapCountMenu(int width, int height)
{
    static int LAP_COUNTS[] =
    {
        100, 50, 25, 20, 15, 10, 5, 3, 1
    };

    const int numLapCounts = sizeof(LAP_COUNTS) / sizeof(int);
    const int itemHeight   = height / (numLapCounts + 2);
    const int textSize     = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuManager;
    using MTFH::MenuItemViewPtr;

    int selectedIndex = 0;
    for (int i = 0; i < numLapCounts; i++)
    {
        std::wstringstream itemText;
        itemText << LAP_COUNTS[i];

        MenuItem * lapCountItem = new MenuItem(width, itemHeight, itemText.str());
        lapCountItem->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *lapCountItem)));
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
