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

#include "settingsmenu.hpp"

#include "audioworker.hpp"
#include "confirmationmenu.hpp"
#include "game.hpp"
#include "keyconfigmenu.hpp"
#include "renderer.hpp"
#include "resolutionmenu.hpp"
#include "settings.hpp"
#include "surfacemenu.hpp"
#include "textmenuitemview.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "trackloader.hpp"
#include "vsyncmenu.hpp"

#include <MenuItem>
#include <MenuItemAction>
#include <MenuItemView>
#include <MenuManager>

#include "simple_logger.hpp"

#include <memory>
#include <sstream>

#include <QObject> // For QObject::tr()

// Swap interval supported only in Qt 5.3+, see also game.cpp
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
#define VSYNC_MENU
#endif

static const int ITEM_HEIGHT_DIV = 10;
static const int ITEM_TEXT_SIZE = 20;

class ResetAction : public MTFH::MenuItemAction
{
public:
    enum class Type
    {
        Times,
        Positions,
        Tracks
    };

    ResetAction(Type type, std::shared_ptr<ConfirmationMenu> menu)
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
        case Type::Positions:
            MenuManager::instance().pushMenu(m_confirmationMenu->id());
            m_confirmationMenu->setText(QObject::tr("Reset best positions?").toUpper().toStdWString());
            m_confirmationMenu->setAcceptAction(
              []() {
                  juzzlin::L().info() << "Reset positions selected.";
                  Settings::instance().resetBestPos();
              });
            m_confirmationMenu->setCurrentIndex(1);
            break;

        case Type::Times:
            MenuManager::instance().pushMenu(m_confirmationMenu->id());
            m_confirmationMenu->setText(QObject::tr("Reset record times?").toUpper().toStdWString());
            m_confirmationMenu->setAcceptAction(
              []() {
                  juzzlin::L().info() << "Reset times selected.";
                  Settings::instance().resetLapRecords();
                  Settings::instance().resetRaceRecords();
              });
            m_confirmationMenu->setCurrentIndex(1);
            break;

        case Type::Tracks:
            MenuManager::instance().pushMenu(m_confirmationMenu->id());
            m_confirmationMenu->setText(QObject::tr("Reset unlocked tracks?").toUpper().toStdWString());
            m_confirmationMenu->setAcceptAction(
              []() {
                    juzzlin::L().info() << "Reset tracks selected.";
                    TrackLoader & tl = TrackLoader::instance();
                    for (unsigned int i = 0; i < tl.tracks(); i++)
                    {
                        Track & track = *tl.track(i);
                        if (track.trackData().index() > 0)
                        {
                            track.trackData().setIsLocked(true);
                        }
                }
                Settings::instance().resetTrackUnlockStatuses(); });
            m_confirmationMenu->setCurrentIndex(1);
            break;
        }
    }

    Type m_type;

    std::shared_ptr<ConfirmationMenu> m_confirmationMenu;
};

static const char * CONFIRMATION_MENU_ID = "confirmationMenu";

static const char * FPS_MENU_ID = "fpsMenu";

static const char * FULL_SCREEN_RESOLUTION_MENU_ID = "fullScreenResolutionMenu";

static const char * GAME_MODE_MENU_ID = "gameModeMenu";

static const char * GFX_MENU_ID = "gfxMenu";

static const char * KEY_CONFIG_MENU_ID = "keyConfigMenu";

static const char * RESET_MENU_ID = "resetMenu";

static const char * SFX_MENU_ID = "sfxMenu";

static const char * SPLIT_TYPE_MENU_ID = "splitTypeMenu";

static const char * VSYNC_MENU_ID = "vsyncMenu";

static const char * WINDOWED_RESOLUTION_MENU_ID = "windowedResolutionMenu";

SettingsMenu::SettingsMenu(std::string id, int width, int height)
  : SurfaceMenu("settingsBack", id, width, height, Menu::Style::VerticalList)
  , m_confirmationMenu(new ConfirmationMenu(CONFIRMATION_MENU_ID, width, height))
  , m_fpsMenu(new SurfaceMenu("settingsBack", FPS_MENU_ID, width, height, Menu::Style::VerticalList))
  , m_fullScreenResolutionMenu(new ResolutionMenu(m_confirmationMenu, FULL_SCREEN_RESOLUTION_MENU_ID, width, height, true))
  , m_windowedResolutionMenu(new ResolutionMenu(m_confirmationMenu, WINDOWED_RESOLUTION_MENU_ID, width, height, false))
  , m_gameModeMenu(new SurfaceMenu("settingsBack", GAME_MODE_MENU_ID, width, height, Menu::Style::VerticalList))
  , m_gfxMenu(new SurfaceMenu("settingsBack", GFX_MENU_ID, width, height, Menu::Style::VerticalList))
  , m_resetMenu(new SurfaceMenu("settingsBack", RESET_MENU_ID, width, height, Menu::Style::VerticalList))
  , m_sfxMenu(new SurfaceMenu("settingsBack", SFX_MENU_ID, width, height, Menu::Style::VerticalList))
  , m_splitTypeMenu(new SurfaceMenu("settingsBack", SPLIT_TYPE_MENU_ID, width, height, Menu::Style::VerticalList))
  , m_vsyncMenu(new VSyncMenu(m_confirmationMenu, VSYNC_MENU_ID, width, height))
  , m_keyConfigMenu(new KeyConfigMenu(KEY_CONFIG_MENU_ID, width, height))
{
    populate(width, height);

    populateFpsMenu(width, height);

    populateGameModeMenu(width, height);

    populateGfxMenu(width, height);

    populateResetMenu(width, height);

    populateSfxMenu(width, height);

    populateSplitTypeMenu(width, height);

    using MTFH::MenuManager;

    MenuManager::instance().addMenu(m_confirmationMenu);
    MenuManager::instance().addMenu(m_fpsMenu);
    MenuManager::instance().addMenu(m_fullScreenResolutionMenu);
    MenuManager::instance().addMenu(m_gameModeMenu);
    MenuManager::instance().addMenu(m_gfxMenu);
    MenuManager::instance().addMenu(m_keyConfigMenu);
    MenuManager::instance().addMenu(m_resetMenu);
    MenuManager::instance().addMenu(m_sfxMenu);
    MenuManager::instance().addMenu(m_splitTypeMenu);
#ifdef VSYNC_MENU
    MenuManager::instance().addMenu(m_vsyncMenu);
#endif
    MenuManager::instance().addMenu(m_windowedResolutionMenu);
}

void SettingsMenu::populate(int width, int height)
{
    const int itemHeight = height / ITEM_HEIGHT_DIV;
    const int textSize = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuItemActionPtr;
    using MTFH::MenuItemViewPtr;
    using MTFH::MenuManager;

    MenuItem * gameMode = new MenuItem(width, itemHeight, QObject::tr("Game mode >").toUpper().toStdWString());
    gameMode->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *gameMode)));
    gameMode->setMenuOpenAction(GAME_MODE_MENU_ID);

    MenuItem * sfx = new MenuItem(width, itemHeight, QObject::tr("Sounds >").toUpper().toStdWString());
    sfx->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *sfx)));
    sfx->setMenuOpenAction(SFX_MENU_ID);

    MenuItem * gfx = new MenuItem(width, itemHeight, QObject::tr("GFX >").toUpper().toStdWString());
    gfx->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *gfx)));
    gfx->setMenuOpenAction(GFX_MENU_ID);

    MenuItem * configureKeys = new MenuItem(width, itemHeight, QObject::tr("Controls >").toUpper().toStdWString());
    configureKeys->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *configureKeys)));
    configureKeys->setMenuOpenAction(KEY_CONFIG_MENU_ID);

    MenuItem * reset = new MenuItem(width, itemHeight, QObject::tr("Reset >").toUpper().toStdWString());
    reset->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *reset)));
    reset->setMenuOpenAction(RESET_MENU_ID);

    using MTFH::MenuItemPtr;

    addItem(MenuItemPtr(reset));
    addItem(MenuItemPtr(configureKeys));
    addItem(MenuItemPtr(sfx));
    addItem(MenuItemPtr(gfx));
    addItem(MenuItemPtr(gameMode));
}

void SettingsMenu::populateFpsMenu(int width, int height)
{
    const int numItems = 2;
    const int itemHeight = height / (numItems + 4);
    const int textSize = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuItemViewPtr;
    using MTFH::MenuManager;

    MenuItem * fps30 = new MenuItem(width, itemHeight, QObject::tr("30 fps").toUpper().toStdWString());
    fps30->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *fps30)));
    fps30->setAction(
      []() {
          juzzlin::L().info() << "30 fps selected.";
          Game::instance().setFps(Game::Fps::Fps30);
          Settings::instance().saveValue(Settings::fpsKey(), 30);
          MenuManager::instance().popMenu();
      });

    MenuItem * fps60 = new MenuItem(width, itemHeight, QObject::tr("60 fps").toUpper().toStdWString());
    fps60->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *fps60)));
    fps60->setAction(
      []() {
          juzzlin::L().info() << "60 fps selected.";
          Game::instance().setFps(Game::Fps::Fps60);
          Settings::instance().saveValue(Settings::fpsKey(), 60);
          MenuManager::instance().popMenu();
      });

    m_fpsMenu->addItem(MTFH::MenuItemPtr(fps30));
    m_fpsMenu->addItem(MTFH::MenuItemPtr(fps60));

    if (Settings::instance().loadValue(Settings::fpsKey()) == 30)
    {
        fps30->setCurrent();
    }
    else
    {
        fps60->setCurrent();
    }
}

void SettingsMenu::populateGameModeMenu(int width, int height)
{
    const int itemHeight = height / ITEM_HEIGHT_DIV;
    const int textSize = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuItemViewPtr;
    using MTFH::MenuManager;

    MenuItem * twoPlayers = new MenuItem(width, itemHeight, QObject::tr("Two player race").toUpper().toStdWString());
    twoPlayers->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *twoPlayers)));
    twoPlayers->setAction(
      []() {
          juzzlin::L().info() << "Two player race selected.";
          Game::instance().setMode(Game::Mode::TwoPlayerRace);
          MenuManager::instance().popMenu();
      });

    MenuItem * onePlayer = new MenuItem(width, itemHeight, QObject::tr("One player race").toUpper().toStdWString());
    onePlayer->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *onePlayer)));
    onePlayer->setAction(
      []() {
          juzzlin::L().info() << "One player race selected.";
          Game::instance().setMode(Game::Mode::OnePlayerRace);
          MenuManager::instance().popMenu();
      });

    MenuItem * timeTrial = new MenuItem(width, itemHeight, QObject::tr("Time Trial").toUpper().toStdWString());
    timeTrial->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *timeTrial)));
    timeTrial->setAction(
      []() {
          juzzlin::L().info() << "Time Trial selected.";
          Game::instance().setMode(Game::Mode::TimeTrial);
          MenuManager::instance().popMenu();
      });

    MenuItem * duel = new MenuItem(width, itemHeight, QObject::tr("Duel").toUpper().toStdWString());
    duel->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *duel)));
    duel->setAction(
      []() {
          juzzlin::L().info() << "Duel selected.";
          Game::instance().setMode(Game::Mode::Duel);
          MenuManager::instance().popMenu();
      });

    using MTFH::MenuItemPtr;

    m_gameModeMenu->addItem(MenuItemPtr(duel));
    m_gameModeMenu->addItem(MenuItemPtr(timeTrial));
    m_gameModeMenu->addItem(MenuItemPtr(twoPlayers));
    m_gameModeMenu->addItem(MenuItemPtr(onePlayer));
}

void SettingsMenu::populateSplitTypeMenu(int width, int height)
{
    const int numItems = 2;
    const int itemHeight = height / (numItems + 4);
    const int textSize = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuItemViewPtr;
    using MTFH::MenuManager;

    MenuItem * vertical = new MenuItem(width, itemHeight, QObject::tr("Vertical").toUpper().toStdWString());
    vertical->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *vertical)));
    vertical->setAction(
      []() {
          juzzlin::L().info() << "Vertical split selected.";
          Game::instance().setSplitType(Game::SplitType::Vertical);
          MenuManager::instance().popMenu();
      });

    MenuItem * horizontal = new MenuItem(width, itemHeight, QObject::tr("Horizontal").toUpper().toStdWString());
    horizontal->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *horizontal)));
    horizontal->setAction(
      []() {
          juzzlin::L().info() << "Horizontal split selected.";
          Game::instance().setSplitType(Game::SplitType::Horizontal);
          MenuManager::instance().popMenu();
      });

    m_splitTypeMenu->addItem(MTFH::MenuItemPtr(horizontal));
    m_splitTypeMenu->addItem(MTFH::MenuItemPtr(vertical));
}

void SettingsMenu::populateGfxMenu(int width, int height)
{
    const int itemHeight = height / ITEM_HEIGHT_DIV;
    const int textSize = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuItemPtr;
    using MTFH::MenuItemViewPtr;
    using MTFH::MenuManager;

    MenuItem * selectFullScreenResolution = new MenuItem(width, itemHeight, QObject::tr("Full screen resolution >").toUpper().toStdWString());
    selectFullScreenResolution->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *selectFullScreenResolution)));
    selectFullScreenResolution->setMenuOpenAction(FULL_SCREEN_RESOLUTION_MENU_ID);

    MenuItem * selectWindowedResolution = new MenuItem(width, itemHeight, QObject::tr("Windowed resolution >").toUpper().toStdWString());
    selectWindowedResolution->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *selectWindowedResolution)));
    selectWindowedResolution->setMenuOpenAction(WINDOWED_RESOLUTION_MENU_ID);

    MenuItem * selectFps = new MenuItem(width, itemHeight, QObject::tr("FPS >").toUpper().toStdWString());
    selectFps->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *selectFps)));
    selectFps->setMenuOpenAction(FPS_MENU_ID);

    MenuItem * splitType = new MenuItem(width, itemHeight, QObject::tr("Split type >").toUpper().toStdWString());
    splitType->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *splitType)));
    splitType->setMenuOpenAction(SPLIT_TYPE_MENU_ID);
#ifdef VSYNC_MENU
    MenuItem * vsync = new MenuItem(width, itemHeight, QObject::tr("VSync >").toUpper().toStdWString());
    vsync->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *vsync)));
    vsync->setMenuOpenAction(VSYNC_MENU_ID);
    m_gfxMenu->addItem(MenuItemPtr(vsync));
#endif
    m_gfxMenu->addItem(MenuItemPtr(splitType));
    m_gfxMenu->addItem(MenuItemPtr(selectFps));
    m_gfxMenu->addItem(MenuItemPtr(selectWindowedResolution));
    m_gfxMenu->addItem(MenuItemPtr(selectFullScreenResolution));
}

void SettingsMenu::populateSfxMenu(int width, int height)
{
    const int itemHeight = height / ITEM_HEIGHT_DIV;
    const int textSize = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuItemViewPtr;
    using MTFH::MenuManager;

    MenuItem * offItem = new MenuItem(width, itemHeight, QObject::tr("Off").toUpper().toStdWString());
    offItem->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *offItem)));
    offItem->setAction(
      []() {
          juzzlin::L().info() << "Sounds off selected.";
          Game::instance().audioWorker().setEnabled(false);
          Settings::instance().saveValue(Settings::soundsKey(), false);
          MenuManager::instance().popMenu();
      });

    m_sfxMenu->addItem(MTFH::MenuItemPtr(offItem));

    MenuItem * onItem = new MenuItem(width, itemHeight, QObject::tr("On").toUpper().toStdWString());
    onItem->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *onItem)));
    onItem->setAction(
      []() {
          juzzlin::L().info() << "Sounds on selected.";
          Game::instance().audioWorker().setEnabled(true);
          Settings::instance().saveValue(Settings::soundsKey(), true);
          MenuManager::instance().popMenu();
      });

    m_sfxMenu->addItem(MTFH::MenuItemPtr(onItem));

    if (Game::instance().audioWorker().enabled())
    {
        onItem->setCurrent();
    }
    else
    {
        offItem->setCurrent();
    }
}

void SettingsMenu::populateResetMenu(int width, int height)
{
    const int itemHeight = height / ITEM_HEIGHT_DIV;
    const int textSize = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuItemActionPtr;
    using MTFH::MenuItemPtr;
    using MTFH::MenuItemViewPtr;
    using MTFH::MenuManager;

    MenuItem * resetRecordTimes = new MenuItem(width, itemHeight, QObject::tr("Reset record times").toUpper().toStdWString());
    resetRecordTimes->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *resetRecordTimes)));
    resetRecordTimes->setAction(
      MenuItemActionPtr(new ResetAction(ResetAction::Type::Times, m_confirmationMenu)));

    MenuItem * resetBestPositions = new MenuItem(width, itemHeight, QObject::tr("Reset best positions").toUpper().toStdWString());
    resetBestPositions->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *resetBestPositions)));
    resetBestPositions->setAction(
      MenuItemActionPtr(new ResetAction(ResetAction::Type::Positions, m_confirmationMenu)));

    MenuItem * resetUnlockedTracks = new MenuItem(width, itemHeight, QObject::tr("Reset unlocked tracks").toUpper().toStdWString());
    resetUnlockedTracks->setView(MenuItemViewPtr(new TextMenuItemView(textSize, *resetUnlockedTracks)));
    resetUnlockedTracks->setAction(
      MenuItemActionPtr(new ResetAction(ResetAction::Type::Tracks, m_confirmationMenu)));

    m_resetMenu->addItem(MenuItemPtr(resetRecordTimes));
    m_resetMenu->addItem(MenuItemPtr(resetBestPositions));
    m_resetMenu->addItem(MenuItemPtr(resetUnlockedTracks));
}
