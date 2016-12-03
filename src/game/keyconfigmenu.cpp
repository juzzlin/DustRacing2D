// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#include "keyconfigmenu.hpp"
#include "eventhandler.hpp"
#include "inputhandler.hpp"
#include "game.hpp"
#include "textmenuitemview.hpp"

#include <MenuItem>
#include <MenuItemAction>
#include <MenuItemView>
#include <MenuManager>

#include <MCAssetManager>
#include <MCLogger>
#include <MCTextureText>
#include <MCTextureFont>

#include <QObject> // For QObject::tr()

//! "Menu" to select a key.
class PressKeyMenu : public SurfaceMenu
{
public:

    //! Constructor.
    PressKeyMenu(std::string id, int width, int height);

    //! \reimp
    virtual void render() override;

private:

    std::wstring m_text;
};

PressKeyMenu::PressKeyMenu(std::string id, int width, int height)
    : SurfaceMenu("settingsBack", id, width, height, MTFH::Menu::Style::HorizontalList)
, m_text(QObject::tr("Press a key..").toStdWString())
{
}

void PressKeyMenu::render()
{
    SurfaceMenu::render();

    MCTextureText text(m_text);
    MCTextureFont defaultMonospace = MCAssetManager::textureFontManager().font(Game::instance().fontName());

    const int shadowY = -2;
    const int shadowX =  2;

    text.setColor(MCGLColor(0.25, 0.75, 1.0, 1.0));
    text.setGlyphSize(20, 20);
    text.setShadowOffset(shadowX, shadowY);

    text.render(width() / 2 - text.width() / 2 + 20, height() / 2 + 60, nullptr, defaultMonospace);
}

static const char * PRESS_KEY_MENU_ID = "pressKeyMenu";

KeyConfigMenu::KeyConfigMenu(std::string id, int width, int height)
    : SurfaceMenu("settingsBack", id, width, height, Menu::Style::VerticalList)
, m_pressKeyMenu(new PressKeyMenu(PRESS_KEY_MENU_ID, width, height))
{
    MTFH::MenuManager::instance().addMenu(*m_pressKeyMenu);

    addPlayerTwoConfig(width, height);
    addPlayerOneConfig(width, height);
}

void KeyConfigMenu::addPlayerOneConfig(int width, int height)
{
    const int itemHeight = height / 10;

    using MTFH::MenuItem;
    using MTFH::MenuManager;

    MenuItem * playerOneAccelerate =
        new MenuItem(width, itemHeight, QObject::tr("Player One Accelerate").toStdWString());
    playerOneAccelerate->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(20, *playerOneAccelerate)));
    playerOneAccelerate->setAction(
        []()
        {
            Game::instance().eventHandler().enableCaptureMode(InputHandler::Action::Up, 0);
            MenuManager::instance().pushMenu(PRESS_KEY_MENU_ID);
        });

    MenuItem * playerOneBrake =
        new MenuItem(width, itemHeight, QObject::tr("Player One Brake").toStdWString());
    playerOneBrake->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(20, *playerOneBrake)));
    playerOneBrake->setAction(
        []()
        {
            Game::instance().eventHandler().enableCaptureMode(InputHandler::Action::Down, 0);
            MenuManager::instance().pushMenu(PRESS_KEY_MENU_ID);
        });

    MenuItem * playerOneTurnLeft =
        new MenuItem(width, itemHeight, QObject::tr("Player One Turn Left").toStdWString());
    playerOneTurnLeft->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(20, *playerOneTurnLeft)));
    playerOneTurnLeft->setAction(
        []()
        {
            Game::instance().eventHandler().enableCaptureMode(InputHandler::Action::Left, 0);
            MenuManager::instance().pushMenu(PRESS_KEY_MENU_ID);
        });

    MenuItem * playerOneTurnRight =
        new MenuItem(width, itemHeight, QObject::tr("Player One Turn Right").toStdWString());
    playerOneTurnRight->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(20, *playerOneTurnRight)));
    playerOneTurnRight->setAction(
        []()
        {
            Game::instance().eventHandler().enableCaptureMode(InputHandler::Action::Right, 0);
            MenuManager::instance().pushMenu(PRESS_KEY_MENU_ID);
        });

    addItem(MTFH::MenuItemPtr(playerOneBrake));
    addItem(MTFH::MenuItemPtr(playerOneAccelerate));
    addItem(MTFH::MenuItemPtr(playerOneTurnRight));
    addItem(MTFH::MenuItemPtr(playerOneTurnLeft));
}

void KeyConfigMenu::addPlayerTwoConfig(int width, int height)
{
    const int itemHeight = height / 10;

    using MTFH::MenuItem;
    using MTFH::MenuManager;

    MenuItem * playerTwoAccelerate =
        new MenuItem(width, itemHeight, QObject::tr("Player Two Accelerate").toStdWString());
    playerTwoAccelerate->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(20, *playerTwoAccelerate)));
    playerTwoAccelerate->setAction(
        []()
        {
            Game::instance().eventHandler().enableCaptureMode(InputHandler::Action::Up, 1);
            MenuManager::instance().pushMenu(PRESS_KEY_MENU_ID);
        });

    MenuItem * playerTwoBrake =
        new MenuItem(width, itemHeight, QObject::tr("Player Two Brake").toStdWString());
    playerTwoBrake->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(20, *playerTwoBrake)));
    playerTwoBrake->setAction(
        []()
        {
            Game::instance().eventHandler().enableCaptureMode(InputHandler::Action::Down, 1);
            MenuManager::instance().pushMenu(PRESS_KEY_MENU_ID);
         });

    MenuItem * playerTwoTurnLeft =
        new MenuItem(width, itemHeight, QObject::tr("Player Two Turn Left").toStdWString());
    playerTwoTurnLeft->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(20, *playerTwoTurnLeft)));
    playerTwoTurnLeft->setAction(
        []()
        {
            Game::instance().eventHandler().enableCaptureMode(InputHandler::Action::Left, 1);
            MenuManager::instance().pushMenu(PRESS_KEY_MENU_ID);
        });

    MenuItem * playerTwoTurnRight =
        new MenuItem(width, itemHeight, QObject::tr("Player Two Turn Right").toStdWString());
    playerTwoTurnRight->setView(MTFH::MenuItemViewPtr(new TextMenuItemView(20, *playerTwoTurnRight)));
    playerTwoTurnRight->setAction(
        []()
        {
            Game::instance().eventHandler().enableCaptureMode(InputHandler::Action::Right, 1);
            MenuManager::instance().pushMenu(PRESS_KEY_MENU_ID);
        });

    addItem(MTFH::MenuItemPtr(playerTwoBrake));
    addItem(MTFH::MenuItemPtr(playerTwoAccelerate));
    addItem(MTFH::MenuItemPtr(playerTwoTurnRight));
    addItem(MTFH::MenuItemPtr(playerTwoTurnLeft));
}

KeyConfigMenu::~KeyConfigMenu()
{
    delete m_pressKeyMenu;
}
