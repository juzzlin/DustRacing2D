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

#include <MCLogger>

class ResetAction : public MenuItemAction
{
public:

    enum ResetType {RT_TIMES, RT_POSITIONS, RT_TRACKS};

    ResetAction(ResetType type)
    : m_type(type)
    {
    }

private:

    //! \reimp
    void fire()
    {
        switch (m_type)
        {
        case RT_TIMES:
            MCLogger().info() << "Reset times selected.";
            MenuManager::instance().pushMenu("confirmationMenu");
            break;
        case RT_POSITIONS:
            MCLogger().info() << "Reset positions selected.";
            MenuManager::instance().pushMenu("confirmationMenu");
            break;
        case RT_TRACKS:
            MCLogger().info() << "Reset tracks selected.";
            MenuManager::instance().pushMenu("confirmationMenu");
            break;
        default:
            break;
        }
    }

    ResetType m_type;
};

SettingsMenu::SettingsMenu(std::string id, int width, int height)
: SurfaceMenu("helpBack", id, width, height, Menu::MS_VERTICAL_LIST)
, m_confirmationMenu("confirmationMenu", width, height)
{
    MenuItem * resetRecordTimes = new MenuItem(width, height / 5, "Reset record times");
    resetRecordTimes->setView(new MenuItemView(*resetRecordTimes), true);
    resetRecordTimes->view()->setTextSize(20);
    resetRecordTimes->setAction(new ResetAction(ResetAction::RT_TIMES), true);

    MenuItem * resetBestPositions = new MenuItem(width, height / 5, "Reset best positions");
    resetBestPositions->setView(new MenuItemView(*resetBestPositions), true);
    resetBestPositions->view()->setTextSize(20);
    resetBestPositions->setAction(new ResetAction(ResetAction::RT_POSITIONS), true);

    MenuItem * resetUnlockedTracks = new MenuItem(width, height / 5, "Reset unlocked tracks");
    resetUnlockedTracks->setView(new MenuItemView(*resetUnlockedTracks), true);
    resetUnlockedTracks->view()->setTextSize(20);
    resetUnlockedTracks->setAction(new ResetAction(ResetAction::RT_TRACKS), true);

    addItem(*resetRecordTimes,    true);
    addItem(*resetBestPositions,  true);
    addItem(*resetUnlockedTracks, true);

    MenuManager::instance().addMenu(m_confirmationMenu);
}
