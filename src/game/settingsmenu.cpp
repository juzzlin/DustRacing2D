// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#include "settingsmenu.hpp"

#include "menuitem.hpp"
#include "menuitemaction.hpp"
#include "menuitemview.hpp"
#include "menumanager.hpp"
#include "renderer.hpp"
#include "settings.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "trackloader.hpp"

#include <MCLogger>

class ResetAction : public MenuItemAction
{
public:

    enum ResetType {RT_TIMES, RT_POSITIONS, RT_TRACKS};

    ResetAction(ResetType type, ConfirmationMenu & menu)
    : m_type(type)
    , m_confirmationMenu(menu)
    {
    }

private:

    class ResetPositions : public MenuItemAction
    {
        //! \reimp
        void fire()
        {
            MCLogger().info() << "Reset positions selected.";
            Settings::instance().resetBestPos();
        }
    };

    class ResetTimes : public MenuItemAction
    {
        //! \reimp
        void fire()
        {
            MCLogger().info() << "Reset times selected.";
            Settings::instance().resetLapRecords();
        }
    };

    class ResetTracks : public MenuItemAction
    {
        //! \reimp
        void fire()
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
            Settings::instance().resetTrackUnlockStatuses();
        }
    };

    //! \reimp
    void fire()
    {
        switch (m_type)
        {
        case RT_POSITIONS:
            MenuManager::instance().pushMenu(m_confirmationMenu.id());
            m_confirmationMenu.setAcceptAction(m_resetPositions);
            m_confirmationMenu.setCurrentIndex(1);
            break;
        case RT_TIMES:
            MenuManager::instance().pushMenu(m_confirmationMenu.id());
            m_confirmationMenu.setAcceptAction(m_resetTimes);
            m_confirmationMenu.setCurrentIndex(1);
            break;
        case RT_TRACKS:
            MenuManager::instance().pushMenu(m_confirmationMenu.id());
            m_confirmationMenu.setAcceptAction(m_resetTracks);
            m_confirmationMenu.setCurrentIndex(1);
            break;
        default:
            break;
        }
    }

    ResetType          m_type;
    ConfirmationMenu & m_confirmationMenu;
    ResetPositions     m_resetPositions;
    ResetTimes         m_resetTimes;
    ResetTracks        m_resetTracks;
};

static const char * CONFIRMATION_MENU_ID = "confirmationMenu";
static const char * RESOLUTION_MENU_ID   = "resolutionMenu";

class ResolutionAction : public MenuItemAction
{
public:

    //! \reimp
    void fire()
    {
        MenuManager::instance().pushMenu(RESOLUTION_MENU_ID);
    }
};

SettingsMenu::SettingsMenu(std::string id, int width, int height)
: SurfaceMenu("helpBack", id, width, height, Menu::MS_VERTICAL_LIST)
, m_confirmationMenu(CONFIRMATION_MENU_ID, width, height)
, m_resolutionMenu(m_confirmationMenu, RESOLUTION_MENU_ID, width, height)
{
    const int itemHeight = height / 8;

    MenuItem * resetRecordTimes = new MenuItem(width, itemHeight, "Reset record times");
    resetRecordTimes->setView(new MenuItemView(*resetRecordTimes), true);
    resetRecordTimes->view()->setTextSize(20);
    resetRecordTimes->setAction(
        new ResetAction(ResetAction::RT_TIMES, m_confirmationMenu), true);

    MenuItem * resetBestPositions = new MenuItem(width, itemHeight, "Reset best positions");
    resetBestPositions->setView(new MenuItemView(*resetBestPositions), true);
    resetBestPositions->view()->setTextSize(20);
    resetBestPositions->setAction(
        new ResetAction(ResetAction::RT_POSITIONS, m_confirmationMenu), true);

    MenuItem * resetUnlockedTracks = new MenuItem(width, itemHeight, "Reset unlocked tracks");
    resetUnlockedTracks->setView(new MenuItemView(*resetUnlockedTracks), true);
    resetUnlockedTracks->view()->setTextSize(20);
    resetUnlockedTracks->setAction(
        new ResetAction(ResetAction::RT_TRACKS, m_confirmationMenu), true);

    MenuItem * selectResolution = new MenuItem(width, itemHeight, "Select resolution >");
    selectResolution->setView(new MenuItemView(*selectResolution), true);
    selectResolution->view()->setTextSize(20);
    selectResolution->setAction(new ResolutionAction, true);

    addItem(*resetRecordTimes,    true);
    addItem(*resetBestPositions,  true);
    addItem(*resetUnlockedTracks, true);
    addItem(*selectResolution,    true);

    MenuManager::instance().addMenu(m_confirmationMenu);
    MenuManager::instance().addMenu(m_resolutionMenu);
}
