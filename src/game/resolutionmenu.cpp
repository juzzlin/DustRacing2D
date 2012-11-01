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

#include "resolutionmenu.hpp"
#include "confirmationmenu.hpp"
#include "game.hpp"
#include "settings.hpp"
#include "textmenuitemview.hpp"

#include <MenuItem>
#include <MenuItemAction>
#include <MenuItemView>
#include <MenuManager>

#include <MCLogger>

class ResolutionItem : public MTFH::MenuItem
{
public:

    class SaveResolutionAction : public MTFH::MenuItemAction
    {
    public:

        //! Constructor.
        SaveResolutionAction(ResolutionItem & parent)
        : m_parent(parent)
        {
        }

        //! \reimp
        virtual void fire()
        {
            const int  hRes = m_parent.hRes();
            const int  vRes = m_parent.vRes();
            const bool fullScreen = !hRes && !vRes;
            Settings::instance().saveResolution(hRes, vRes, fullScreen);
            MCLogger().info() << "Resolution set: " << hRes << " " << vRes << " " << fullScreen;
            MCLogger().info() << "Restart needed.";
            Game::instance().exitGame();
        }

    private:

        ResolutionItem & m_parent;
    };

    //! Constructor.
    ResolutionItem(
        ConfirmationMenu & confirmationMenu,
        int hRes, int vRes, int width, int height, std::string text = "")
    : MenuItem(width, height, text)
    , m_confirmationMenu(confirmationMenu)
    , m_saveResolutionAction(*this)
    , m_hRes(hRes)
    , m_vRes(vRes)
    {
    }

    //! Destructor.
    virtual ~ResolutionItem() {}

    //! \reimp
    virtual void onSelect()
    {
        MTFH::MenuManager::instance().pushMenu(m_confirmationMenu.id());
        m_confirmationMenu.setAcceptAction(m_saveResolutionAction);
        m_confirmationMenu.setCurrentIndex(1);
    }

    int hRes() const
    {
        return m_hRes;
    }

    int vRes() const
    {
        return m_vRes;
    }

private:

    ConfirmationMenu &   m_confirmationMenu;
    SaveResolutionAction m_saveResolutionAction;
    int                  m_hRes;
    int                  m_vRes;
};

ResolutionMenu::ResolutionMenu(
    ConfirmationMenu & confirmationMenu, std::string id, int width, int height)
: SurfaceMenu("helpBack", id, width, height)
, m_confirmationMenu(confirmationMenu)
{
    const int itemHeight = height / 8;

    MTFH::MenuItem * resolution =
        new ResolutionItem(m_confirmationMenu, 320, 200, width, itemHeight, "320x200");
    resolution->setView(new TextMenuItemView(20, *resolution), true);
    addItem(*resolution, true);

    resolution =
        new ResolutionItem(m_confirmationMenu, 640, 480, width, itemHeight, "640x480");
    resolution->setView(new TextMenuItemView(20, *resolution), true);
    addItem(*resolution, true);

    resolution =
        new ResolutionItem(m_confirmationMenu, 800, 600, width, itemHeight, "800x600");
    resolution->setView(new TextMenuItemView(20, *resolution), true);
    addItem(*resolution, true);

    resolution =
        new ResolutionItem(m_confirmationMenu, 1024, 768, width, itemHeight, "1024x768");
    resolution->setView(new TextMenuItemView(20, *resolution), true);
    addItem(*resolution, true);

    resolution =
        new ResolutionItem(m_confirmationMenu, 1280, 960, width, itemHeight, "1280x960");
    resolution->setView(new TextMenuItemView(20, *resolution), true);
    addItem(*resolution, true);

    resolution =
        new ResolutionItem(m_confirmationMenu, 0, 0, width, itemHeight, "Full screen");
    resolution->setView(new TextMenuItemView(20, *resolution), true);
    addItem(*resolution, true);
}
