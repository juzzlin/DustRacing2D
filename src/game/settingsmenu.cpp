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
            m_confirmationMenu.setText("Reset best positions?");
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
            m_confirmationMenu.setText("Reset record times?");
            m_confirmationMenu.setAcceptAction(
                []()
                {
                    MCLogger().info() << "Reset times selected.";
                    Settings::instance().resetLapRecords();
                });
            m_confirmationMenu.setCurrentIndex(1);
            break;

        case RT_TRACKS:
            MenuManager::instance().pushMenu(m_confirmationMenu.id());
            m_confirmationMenu.setText("Reset unlocked tracks?");
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
static const char * WINDOWED_RESOLUTION_MENU_ID    = "windowedResolutionMenu";
static const char * GAME_MODE_MENU_ID              = "gameModeMenu";
static const char * FPS_MENU_ID                    = "fpsMenu";
static const char * KEY_CONFIG_MENU_ID             = "keyConfigMenu";

SettingsMenu::SettingsMenu(std::string id, int width, int height)
: SurfaceMenu("helpBack", id, width, height, Menu::MS_VERTICAL_LIST)
, m_confirmationMenu(CONFIRMATION_MENU_ID, width, height)
, m_fullScreenResolutionMenu(m_confirmationMenu, FULL_SCREEN_RESOLUTION_MENU_ID, width, height, true)
, m_windowedResolutionMenu(m_confirmationMenu, WINDOWED_RESOLUTION_MENU_ID, width, height, false)
, m_gameModeMenu("helpBack", GAME_MODE_MENU_ID, width, height, Menu::MS_VERTICAL_LIST)
, m_fpsMenu("helpBack", FPS_MENU_ID, width, height, Menu::MS_VERTICAL_LIST)
, m_keyConfigMenu(KEY_CONFIG_MENU_ID, width, height)
{
    populate(width, height);
    populateGameModeMenu(width, height);
    populateFpsMenu(width, height);

    using MTFH::MenuManager;

    MenuManager::instance().addMenu(m_confirmationMenu);
    MenuManager::instance().addMenu(m_windowedResolutionMenu);
    MenuManager::instance().addMenu(m_fullScreenResolutionMenu);
    MenuManager::instance().addMenu(m_gameModeMenu);
    MenuManager::instance().addMenu(m_fpsMenu);
    MenuManager::instance().addMenu(m_keyConfigMenu);
}

void SettingsMenu::populate(int width, int height)
{
    const int itemHeight = height / 10;

    using MTFH::MenuItem;
    using MTFH::MenuManager;

    MenuItem * resetRecordTimes = new MenuItem(width, itemHeight, "Reset record times");
    resetRecordTimes->setView(new TextMenuItemView(20, *resetRecordTimes), true);
    resetRecordTimes->setAction(
        new ResetAction(ResetAction::RT_TIMES, m_confirmationMenu), true);

    MenuItem * resetBestPositions = new MenuItem(width, itemHeight, "Reset best positions");
    resetBestPositions->setView(new TextMenuItemView(20, *resetBestPositions), true);
    resetBestPositions->setAction(
        new ResetAction(ResetAction::RT_POSITIONS, m_confirmationMenu), true);

    MenuItem * resetUnlockedTracks = new MenuItem(width, itemHeight, "Reset unlocked tracks");
    resetUnlockedTracks->setView(new TextMenuItemView(20, *resetUnlockedTracks), true);
    resetUnlockedTracks->setAction(
        new ResetAction(ResetAction::RT_TRACKS, m_confirmationMenu), true);

    MenuItem * selectFullScreenResolution = new MenuItem(width, itemHeight, "Select full screen resolution >");
    selectFullScreenResolution->setView(new TextMenuItemView(20, *selectFullScreenResolution), true);
    selectFullScreenResolution->setMenuOpenAction(FULL_SCREEN_RESOLUTION_MENU_ID);

    MenuItem * selectWindowedResolution = new MenuItem(width, itemHeight, "Select windowed resolution >");
    selectWindowedResolution->setView(new TextMenuItemView(20, *selectWindowedResolution), true);
    selectWindowedResolution->setMenuOpenAction(WINDOWED_RESOLUTION_MENU_ID);

    MenuItem * gameMode = new MenuItem(width, itemHeight, "Game mode >");
    gameMode->setView(new TextMenuItemView(20, *gameMode), true);
    gameMode->setMenuOpenAction(GAME_MODE_MENU_ID);

    MenuItem * selectFps = new MenuItem(width, itemHeight, "Select FPS >");
    selectFps->setView(new TextMenuItemView(20, *selectFps), true);
    selectFps->setMenuOpenAction(FPS_MENU_ID);

    MenuItem * configureKeys = new MenuItem(width, itemHeight, "Configure keys >");
    configureKeys->setView(new TextMenuItemView(20, *configureKeys), true);
    configureKeys->setMenuOpenAction(KEY_CONFIG_MENU_ID);

    addItem(*resetRecordTimes,           true);
    addItem(*resetBestPositions,         true);
    addItem(*resetUnlockedTracks,        true);
    addItem(*configureKeys,              true);
    addItem(*selectFps,                  true);
    addItem(*selectWindowedResolution,   true);
    addItem(*selectFullScreenResolution, true);
    addItem(*gameMode,                   true);
}

void SettingsMenu::populateGameModeMenu(int width, int height)
{
    const int itemHeight = height / 8;

    using MTFH::MenuItem;
    using MTFH::MenuManager;

    MenuItem * twoPlayers = new MenuItem(width, itemHeight, "Two player race");
    twoPlayers->setView(new TextMenuItemView(20, *twoPlayers), true);
    twoPlayers->setAction(
        []()
        {
            MCLogger().info() << "Two player race selected.";
            Game::instance().setMode(Game::TwoPlayerRace);
            MenuManager::instance().popMenu();
        });

    MenuItem * onePlayer = new MenuItem(width, itemHeight, "One player race");
    onePlayer->setView(new TextMenuItemView(20, *onePlayer), true);
    onePlayer->setAction(
        []()
        {
            MCLogger().info() << "One player race selected.";
            Game::instance().setMode(Game::OnePlayerRace);
            MenuManager::instance().popMenu();
        });

    MenuItem * timeTrial = new MenuItem(width, itemHeight, "Time Trial");
    timeTrial->setView(new TextMenuItemView(20, *timeTrial), true);
    timeTrial->setAction(
        []()
        {
            MCLogger().info() << "Time Trial selected.";
            Game::instance().setMode(Game::TimeTrial);
            MenuManager::instance().popMenu();
        });

    MenuItem * duel = new MenuItem(width, itemHeight, "Duel");
    duel->setView(new TextMenuItemView(20, *duel), true);
    duel->setAction(
        []()
        {
            MCLogger().info() << "Duel selected.";
            Game::instance().setMode(Game::Duel);
            MenuManager::instance().popMenu();
        });

    m_gameModeMenu.addItem(*duel,       true);
    m_gameModeMenu.addItem(*timeTrial,  true);
    m_gameModeMenu.addItem(*twoPlayers, true);
    m_gameModeMenu.addItem(*onePlayer,  true);
}

void SettingsMenu::populateFpsMenu(int width, int height)
{
    const int itemHeight = height / 8;

    using MTFH::MenuItem;
    using MTFH::MenuManager;

    MenuItem * fps30 = new MenuItem(width, itemHeight, "30 fps");
    fps30->setView(new TextMenuItemView(20, *fps30), true);
    fps30->setAction(
        []()
        {
            MCLogger().info() << "30 fps selected.";
            Game::instance().setFps(30);
            Settings::instance().saveFps(30);
            MenuManager::instance().popMenu();
        });

    MenuItem * fps60 = new MenuItem(width, itemHeight, "60 fps");
    fps60->setView(new TextMenuItemView(20, *fps60), true);
    fps60->setAction(
        []()
        {
            MCLogger().info() << "60 fps selected.";
            Game::instance().setFps(60);
            Settings::instance().saveFps(60);
            MenuManager::instance().popMenu();
        });

    m_fpsMenu.addItem(*fps30, true);
    m_fpsMenu.addItem(*fps60, true);

    MenuManager::instance().addMenu(m_confirmationMenu);
    MenuManager::instance().addMenu(m_fullScreenResolutionMenu);
    MenuManager::instance().addMenu(m_windowedResolutionMenu);
    MenuManager::instance().addMenu(m_gameModeMenu);
    MenuManager::instance().addMenu(m_fpsMenu);
    MenuManager::instance().addMenu(m_keyConfigMenu);
}
