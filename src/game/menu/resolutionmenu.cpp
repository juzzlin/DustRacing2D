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

#include "resolutionmenu.hpp"
#include "confirmationmenu.hpp"
#include "game.hpp"
#include "renderer.hpp"
#include "settings.hpp"
#include "textmenuitemview.hpp"

#include <MenuItem>
#include <MenuItemAction>
#include <MenuItemView>
#include <MenuManager>

#include "simple_logger.hpp"

#include <QObject> // for tr()
#include <QScreen>

#include <memory>
#include <sstream>

struct Resolution
{
    int hRes, vRes;
};

class ResolutionItem : public MTFH::MenuItem
{
public:
    class SaveResolutionAction : public MTFH::MenuItemAction
    {
    public:
        //! Constructor.
        SaveResolutionAction(ResolutionItem & parent, bool fullScreen)
          : m_parent(parent)
          , m_fullScreen(fullScreen)
        {
        }

        bool fullScreen() const
        {
            return m_fullScreen;
        }

        //! \reimp
        virtual void fire()
        {
            const int hRes = m_parent.hRes();
            const int vRes = m_parent.vRes();
            Settings::instance().saveResolution(hRes, vRes, m_fullScreen);
            juzzlin::L().info()
              << "Resolution set: " << hRes << " " << vRes
              << " Full screen: " << m_fullScreen;

            if (Game::instance().renderer().fullScreen() && m_fullScreen)
            {
                Game::instance().renderer().setResolution(QSize(hRes, vRes));
            }
            else
            {
                Game::instance().exitGame();
            }
        }

    private:
        ResolutionItem & m_parent;
        bool m_fullScreen;
    };

    //! Constructor.
    ResolutionItem(ConfirmationMenuPtr confirmationMenu,
                   int hRes, int vRes, bool fullScreen, int width, int height, std::wstring text = L"")
      : MenuItem(width, height, text)
      , m_confirmationMenu(confirmationMenu)
      , m_saveResolutionAction(std::make_shared<SaveResolutionAction>(*this, fullScreen))
      , m_hRes(hRes)
      , m_vRes(vRes)
    {
    }

    //! Destructor.
    virtual ~ResolutionItem()
    {
    }

    //! \reimp
    virtual void setSelected(bool flag)
    {
        MenuItem::setSelected(flag);

        if (flag)
        {
            // Change the virtual resolution immediately if changing from fullscreen to fullscreen
            if (Game::instance().renderer().fullScreen() && std::dynamic_pointer_cast<SaveResolutionAction>(m_saveResolutionAction)->fullScreen())
            {
                m_saveResolutionAction->fire();
            }
            else
            {
                MTFH::MenuManager::instance().pushMenu(m_confirmationMenu->id());
                m_confirmationMenu->setText(QObject::tr("Restart to change the resolution.").toStdWString());
                m_confirmationMenu->setAcceptAction(m_saveResolutionAction);
                m_confirmationMenu->setCurrentIndex(1);
            }
        }
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
    ConfirmationMenuPtr m_confirmationMenu;

    MTFH::MenuItemActionPtr m_saveResolutionAction;

    int m_hRes;

    int m_vRes;
};

ResolutionMenu::ResolutionMenu(
  ConfirmationMenuPtr confirmationMenu, std::string id, int width, int height, bool fullScreen)
  : SurfaceMenu("settingsBack", id, width, height)
  , m_confirmationMenu(confirmationMenu)
{
    const int numResolutions = 8;
    const int fullHRes = Game::instance().screen()->geometry().width();
    const int fullVRes = Game::instance().screen()->geometry().height();
    const int itemHeight = height / (numResolutions + 3);

    int itemHRes = fullHRes;
    int itemVRes = fullVRes;
    for (int i = 0; i < numResolutions; i++)
    {
        std::wstringstream resString;
        resString << itemHRes << "x" << itemVRes;
        const auto resolution =
          std::make_shared<ResolutionItem>(m_confirmationMenu, itemHRes, itemVRes, fullScreen, width, itemHeight, resString.str());
        resolution->setView(std::make_shared<TextMenuItemView>(20, *resolution));
        addItem(resolution);

        itemHRes -= fullHRes / numResolutions;
        itemVRes -= fullVRes / numResolutions;
    }

    reverseItems();

    enter();
}

void ResolutionMenu::enter()
{
    setCurrentIndex(static_cast<int>(itemCount()) - 1);

    // Set the current active resolution selected
    for (unsigned int i = 0; i < itemCount(); i++)
    {
        if (auto resolution = std::dynamic_pointer_cast<ResolutionItem>(item(i)))
        {
            int hRes = 0;
            int vRes = 0;
            bool fullScreen = false;

            Settings::instance().loadResolution(hRes, vRes, fullScreen);
            if (resolution->hRes() == hRes && resolution->vRes() == vRes)
            {
                resolution->setCurrent();
                break;
            }
        }
    }
}
