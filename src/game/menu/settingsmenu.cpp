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
#include "database.hpp"
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
                  Database::instance().resetBestPos();
              });
            m_confirmationMenu->setCurrentIndex(1);
            break;

        case Type::Times:
            MenuManager::instance().pushMenu(m_confirmationMenu->id());
            m_confirmationMenu->setText(QObject::tr("Reset record times?").toUpper().toStdWString());
            m_confirmationMenu->setAcceptAction(
              []() {
                  juzzlin::L().info() << "Reset times selected.";
                  Database::instance().resetLapRecords();
                  Database::instance().resetRaceRecords();
              });
            m_confirmationMenu->setCurrentIndex(1);
            break;

        case Type::Tracks:
            MenuManager::instance().pushMenu(m_confirmationMenu->id());
            m_confirmationMenu->setText(QObject::tr("Reset unlocked tracks?").toUpper().toStdWString());
            m_confirmationMenu->setAcceptAction(
              []() {
                    juzzlin::L().info() << "Reset tracks selected.";
                    auto && trackLoader = TrackLoader::instance();
                    for (size_t i = 0; i < trackLoader.tracks(); i++)
                    {
                        Track & track = *trackLoader.track(i);
                        if (track.trackData().index() > 0)
                        {
                            track.trackData().setIsLocked(true);
                        }
                }
                Database::instance().resetTrackUnlockStatuses(); });
            m_confirmationMenu->setCurrentIndex(1);
            break;
        }
    }

    Type m_type;

    std::shared_ptr<ConfirmationMenu> m_confirmationMenu;
};

static const auto CONFIRMATION_MENU_ID = "confirmationMenu";

static const auto FPS_MENU_ID = "fpsMenu";

static const auto FULL_SCREEN_RESOLUTION_MENU_ID = "fullScreenResolutionMenu";

static const auto GAME_MODE_MENU_ID = "gameModeMenu";

static const auto GFX_MENU_ID = "gfxMenu";

static const auto KEY_CONFIG_MENU_ID = "keyConfigMenu";

static const auto RESET_MENU_ID = "resetMenu";

static const auto SFX_MENU_ID = "sfxMenu";

static const auto SPLIT_TYPE_MENU_ID = "splitTypeMenu";

static const auto VSYNC_MENU_ID = "vsyncMenu";

static const auto WINDOWED_RESOLUTION_MENU_ID = "windowedResolutionMenu";

SettingsMenu::SettingsMenu(std::string id, int width, int height)
  : SurfaceMenu("settingsBack", id, width, height, Menu::Style::VerticalList)
  , m_confirmationMenu(std::make_shared<ConfirmationMenu>(CONFIRMATION_MENU_ID, width, height))
  , m_fpsMenu(std::make_shared<SurfaceMenu>("settingsBack", FPS_MENU_ID, width, height, Menu::Style::VerticalList))
  , m_fullScreenResolutionMenu(std::make_shared<ResolutionMenu>(m_confirmationMenu, FULL_SCREEN_RESOLUTION_MENU_ID, width, height, true))
  , m_windowedResolutionMenu(std::make_shared<ResolutionMenu>(m_confirmationMenu, WINDOWED_RESOLUTION_MENU_ID, width, height, false))
  , m_gameModeMenu(std::make_shared<SurfaceMenu>("settingsBack", GAME_MODE_MENU_ID, width, height, Menu::Style::VerticalList))
  , m_gfxMenu(std::make_shared<SurfaceMenu>("settingsBack", GFX_MENU_ID, width, height, Menu::Style::VerticalList))
  , m_resetMenu(std::make_shared<SurfaceMenu>("settingsBack", RESET_MENU_ID, width, height, Menu::Style::VerticalList))
  , m_sfxMenu(std::make_shared<SurfaceMenu>("settingsBack", SFX_MENU_ID, width, height, Menu::Style::VerticalList))
  , m_splitTypeMenu(std::make_shared<SurfaceMenu>("settingsBack", SPLIT_TYPE_MENU_ID, width, height, Menu::Style::VerticalList))
  , m_vsyncMenu(std::make_shared<VSyncMenu>(m_confirmationMenu, VSYNC_MENU_ID, width, height))
  , m_keyConfigMenu(std::make_shared<KeyConfigMenu>(KEY_CONFIG_MENU_ID, width, height))
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
    using MTFH::MenuManager;

    const auto gameMode = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Game mode >").toUpper().toStdWString());
    gameMode->setView(std::make_shared<TextMenuItemView>(textSize, *gameMode));
    gameMode->setMenuOpenAction(GAME_MODE_MENU_ID);

    const auto sfx = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Sounds >").toUpper().toStdWString());
    sfx->setView(std::make_shared<TextMenuItemView>(textSize, *sfx));
    sfx->setMenuOpenAction(SFX_MENU_ID);

    const auto gfx = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("GFX >").toUpper().toStdWString());
    gfx->setView(std::make_shared<TextMenuItemView>(textSize, *gfx));
    gfx->setMenuOpenAction(GFX_MENU_ID);

    const auto configureKeys = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Controls >").toUpper().toStdWString());
    configureKeys->setView(std::make_shared<TextMenuItemView>(textSize, *configureKeys));
    configureKeys->setMenuOpenAction(KEY_CONFIG_MENU_ID);

    const auto reset = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Reset >").toUpper().toStdWString());
    reset->setView(std::make_shared<TextMenuItemView>(textSize, *reset));
    reset->setMenuOpenAction(RESET_MENU_ID);

    addItem(reset);
    addItem(configureKeys);
    addItem(sfx);
    addItem(gfx);
    addItem(gameMode);
}

void SettingsMenu::populateFpsMenu(int width, int height)
{
    const int numItems = 2;
    const int itemHeight = height / (numItems + 4);
    const int textSize = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuManager;

    const auto fps30 = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("30 fps").toUpper().toStdWString());
    fps30->setView(std::make_shared<TextMenuItemView>(textSize, *fps30));
    fps30->setAction(
      []() {
          juzzlin::L().info() << "30 fps selected.";
          Game::instance().setFps(Game::Fps::Fps30);
          Settings::instance().saveValue(Settings::fpsKey(), 30);
          MenuManager::instance().popMenu();
      });

    const auto fps60 = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("60 fps").toUpper().toStdWString());
    fps60->setView(std::make_shared<TextMenuItemView>(textSize, *fps60));
    fps60->setAction(
      []() {
          juzzlin::L().info() << "60 fps selected.";
          Game::instance().setFps(Game::Fps::Fps60);
          Settings::instance().saveValue(Settings::fpsKey(), 60);
          MenuManager::instance().popMenu();
      });

    m_fpsMenu->addItem(fps30);
    m_fpsMenu->addItem(fps60);

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
    using MTFH::MenuManager;

    const auto twoPlayers = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Two player race").toUpper().toStdWString());
    twoPlayers->setView(std::make_shared<TextMenuItemView>(textSize, *twoPlayers));
    twoPlayers->setAction(
      []() {
          juzzlin::L().info() << "Two player race selected.";
          Game::instance().setMode(Game::Mode::TwoPlayerRace);
          MenuManager::instance().popMenu();
      });

    const auto onePlayer = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("One player race").toUpper().toStdWString());
    onePlayer->setView(std::make_shared<TextMenuItemView>(textSize, *onePlayer));
    onePlayer->setAction(
      []() {
          juzzlin::L().info() << "One player race selected.";
          Game::instance().setMode(Game::Mode::OnePlayerRace);
          MenuManager::instance().popMenu();
      });

    const auto timeTrial = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Time Trial").toUpper().toStdWString());
    timeTrial->setView(std::make_shared<TextMenuItemView>(textSize, *timeTrial));
    timeTrial->setAction(
      []() {
          juzzlin::L().info() << "Time Trial selected.";
          Game::instance().setMode(Game::Mode::TimeTrial);
          MenuManager::instance().popMenu();
      });

    const auto duel = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Duel").toUpper().toStdWString());
    duel->setView(std::make_shared<TextMenuItemView>(textSize, *duel));
    duel->setAction(
      []() {
          juzzlin::L().info() << "Duel selected.";
          Game::instance().setMode(Game::Mode::Duel);
          MenuManager::instance().popMenu();
      });

    m_gameModeMenu->addItem(duel);
    m_gameModeMenu->addItem(timeTrial);
    m_gameModeMenu->addItem(twoPlayers);
    m_gameModeMenu->addItem(onePlayer);
}

void SettingsMenu::populateSplitTypeMenu(int width, int height)
{
    const int numItems = 2;
    const int itemHeight = height / (numItems + 4);
    const int textSize = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuManager;

    const auto vertical = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Vertical").toUpper().toStdWString());
    vertical->setView(std::make_shared<TextMenuItemView>(textSize, *vertical));
    vertical->setAction(
      []() {
          juzzlin::L().info() << "Vertical split selected.";
          Game::instance().setSplitType(Game::SplitType::Vertical);
          MenuManager::instance().popMenu();
      });

    const auto horizontal = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Horizontal").toUpper().toStdWString());
    horizontal->setView(std::make_shared<TextMenuItemView>(textSize, *horizontal));
    horizontal->setAction(
      []() {
          juzzlin::L().info() << "Horizontal split selected.";
          Game::instance().setSplitType(Game::SplitType::Horizontal);
          MenuManager::instance().popMenu();
      });

    m_splitTypeMenu->addItem(horizontal);
    m_splitTypeMenu->addItem(vertical);
}

void SettingsMenu::populateGfxMenu(int width, int height)
{
    const int itemHeight = height / ITEM_HEIGHT_DIV;
    const int textSize = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuManager;

    const auto selectFullScreenResolution = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Full screen resolution >").toUpper().toStdWString());
    selectFullScreenResolution->setView(std::make_shared<TextMenuItemView>(textSize, *selectFullScreenResolution));
    selectFullScreenResolution->setMenuOpenAction(FULL_SCREEN_RESOLUTION_MENU_ID);

    const auto selectWindowedResolution = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Windowed resolution >").toUpper().toStdWString());
    selectWindowedResolution->setView(std::make_shared<TextMenuItemView>(textSize, *selectWindowedResolution));
    selectWindowedResolution->setMenuOpenAction(WINDOWED_RESOLUTION_MENU_ID);

    const auto selectFps = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("FPS >").toUpper().toStdWString());
    selectFps->setView(std::make_shared<TextMenuItemView>(textSize, *selectFps));
    selectFps->setMenuOpenAction(FPS_MENU_ID);

    const auto splitType = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Split type >").toUpper().toStdWString());
    splitType->setView(std::make_shared<TextMenuItemView>(textSize, *splitType));
    splitType->setMenuOpenAction(SPLIT_TYPE_MENU_ID);
#ifdef VSYNC_MENU
    const auto vsync = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("VSync >").toUpper().toStdWString());
    vsync->setView(std::make_shared<TextMenuItemView>(textSize, *vsync));
    vsync->setMenuOpenAction(VSYNC_MENU_ID);
    m_gfxMenu->addItem(vsync);
#endif
    m_gfxMenu->addItem(splitType);
    m_gfxMenu->addItem(selectFps);
    m_gfxMenu->addItem(selectWindowedResolution);
    m_gfxMenu->addItem(selectFullScreenResolution);
}

void SettingsMenu::populateSfxMenu(int width, int height)
{
    const int itemHeight = height / ITEM_HEIGHT_DIV;
    const int textSize = ITEM_TEXT_SIZE;

    using MTFH::MenuItem;
    using MTFH::MenuManager;

    const auto offItem = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Off").toUpper().toStdWString());
    offItem->setView(std::make_shared<TextMenuItemView>(textSize, *offItem));
    offItem->setAction(
      []() {
          juzzlin::L().info() << "Sounds off selected.";
          Game::instance().audioWorker().setEnabled(false);
          Settings::instance().saveValue(Settings::soundsKey(), false);
          MenuManager::instance().popMenu();
      });

    m_sfxMenu->addItem(MTFH::MenuItemPtr(offItem));

    const auto onItem = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("On").toUpper().toStdWString());
    onItem->setView(std::make_shared<TextMenuItemView>(textSize, *onItem));
    onItem->setAction(
      []() {
          juzzlin::L().info() << "Sounds on selected.";
          Game::instance().audioWorker().setEnabled(true);
          Settings::instance().saveValue(Settings::soundsKey(), true);
          MenuManager::instance().popMenu();
      });

    m_sfxMenu->addItem(onItem);

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
    using MTFH::MenuManager;

    const auto resetRecordTimes = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Reset record times").toUpper().toStdWString());
    resetRecordTimes->setView(std::make_shared<TextMenuItemView>(textSize, *resetRecordTimes));
    resetRecordTimes->setAction(std::make_shared<ResetAction>(ResetAction::Type::Times, m_confirmationMenu));

    const auto resetBestPositions = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Reset best positions").toUpper().toStdWString());
    resetBestPositions->setView(std::make_shared<TextMenuItemView>(textSize, *resetBestPositions));
    resetBestPositions->setAction(std::make_shared<ResetAction>(ResetAction::Type::Positions, m_confirmationMenu));

    const auto resetUnlockedTracks = std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Reset unlocked tracks").toUpper().toStdWString());
    resetUnlockedTracks->setView(std::make_shared<TextMenuItemView>(textSize, *resetUnlockedTracks));
    resetUnlockedTracks->setAction(std::make_shared<ResetAction>(ResetAction::Type::Tracks, m_confirmationMenu));

    m_resetMenu->addItem(resetRecordTimes);
    m_resetMenu->addItem(resetBestPositions);
    m_resetMenu->addItem(resetUnlockedTracks);
}
