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

#include "vsyncmenu.hpp"
#include "confirmationmenu.hpp"
#include "game.hpp"
#include "settings.hpp"
#include "textmenuitemview.hpp"

#include <MenuItem>
#include <MenuItemAction>
#include <MenuItemView>
#include <MenuManager>

#include <MCLogger>

#include <QObject> // for tr()

class VSyncItem : public MTFH::MenuItem
{
public:

    class SaveVSyncAction : public MTFH::MenuItemAction
    {
    public:

        //! Constructor.
        SaveVSyncAction(VSyncItem & parent)
        : m_parent(parent)
        {
        }

        //! \reimp
        virtual void fire()
        {
            const int vsync = m_parent.vsync();
            Settings::instance().saveValue(Settings::vsyncKey(), vsync);
            MCLogger().info() << "VSync set: " << vsync;
            Game::instance().exitGame();
        }

    private:

        VSyncItem & m_parent;
    };

    //! Constructor.
    VSyncItem(
        ConfirmationMenu & confirmationMenu,
        int vsync, int width, int height, std::wstring text = L"")
    : MenuItem(width, height, text)
    , m_confirmationMenu(confirmationMenu)
    , m_saveVSyncAction(new SaveVSyncAction(*this))
    , m_vsync(vsync)
    {
    }

    //! Destructor.
    virtual ~VSyncItem() {}

    //! \reimp
    virtual void setSelected(bool flag)
    {
        MenuItem::setSelected(flag);
        MTFH::MenuManager::instance().pushMenu(m_confirmationMenu.id());
        m_confirmationMenu.setText(QObject::tr("Restart to change VSync setting.").toStdWString());
        m_confirmationMenu.setAcceptAction(m_saveVSyncAction);
        m_confirmationMenu.setCurrentIndex(1);
    }

    int vsync() const
    {
        return m_vsync;
    }

private:

    ConfirmationMenu &      m_confirmationMenu;
    MTFH::MenuItemActionPtr m_saveVSyncAction;
    int                     m_vsync;
};

VSyncMenu::VSyncMenu(
    ConfirmationMenu & confirmationMenu, std::string id, int width, int height)
: SurfaceMenu("settingsBack", id, width, height)
, m_confirmationMenu(confirmationMenu)
{
    const int itemHeight = height / 10;

    VSyncItem * off =
        new VSyncItem(m_confirmationMenu, 0, width, itemHeight, QObject::tr("Off").toStdWString());
    off->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(20, *off)));
    addItem(MTFH::MenuItemPtr(off));

    VSyncItem * on =
        new VSyncItem(m_confirmationMenu, 1, width, itemHeight, QObject::tr("On").toStdWString());
    on->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(20, *on)));
    addItem(MTFH::MenuItemPtr(on));

    enter();
}

void VSyncMenu::enter()
{
    // Set the currently selected VSync setting
    const int vsync = Settings::instance().loadValue(Settings::vsyncKey(), 0);
    for (unsigned int i = 0; i < itemCount(); i++)
    {
        if (auto vsyncItem = dynamic_cast<VSyncItem *>(item(i).get()))
        {
            if (vsyncItem->vsync() == vsync)
            {
                setCurrentIndex(vsyncItem->index());
                break;
            }
        }
    }
}
