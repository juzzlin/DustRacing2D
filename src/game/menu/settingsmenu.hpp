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

#ifndef SETTINGSMENU_HPP
#define SETTINGSMENU_HPP

#include <MenuManager>

#include "confirmationmenu.hpp"
#include "surfacemenu.hpp"

//! The settings menu.
class SettingsMenu : public SurfaceMenu
{
public:
    //! Constructor.
    SettingsMenu(std::string id, int width, int height);

private:
    void populate(int width, int height);

    void populateFpsMenu(int width, int height);

    void populateGameModeMenu(int width, int height);

    void populateGfxMenu(int width, int height);

    void populateSfxMenu(int width, int height);

    void populateSplitTypeMenu(int width, int height);

    void populateResetMenu(int width, int height);

    ConfirmationMenuPtr m_confirmationMenu;

    MTFH::MenuPtr m_fpsMenu;

    MTFH::MenuPtr m_fullScreenResolutionMenu;

    MTFH::MenuPtr m_windowedResolutionMenu;

    MTFH::MenuPtr m_gameModeMenu;

    MTFH::MenuPtr m_gfxMenu;

    MTFH::MenuPtr m_resetMenu;

    MTFH::MenuPtr m_sfxMenu;

    MTFH::MenuPtr m_splitTypeMenu;

    MTFH::MenuPtr m_vsyncMenu;

    MTFH::MenuPtr m_keyConfigMenu;
};

#endif // SETTINGSMENU_HPP
