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
#include "game.hpp"
#include "inputhandler.hpp"
#include "textmenuitemview.hpp"

#include <MenuItem>
#include <MenuItemAction>
#include <MenuItemView>
#include <MenuManager>

#include <MCAssetManager>
#include <MCTextureFont>
#include <MCTextureText>

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
  , m_text(QObject::tr("Press a key..").toUpper().toStdWString())
{
}

void PressKeyMenu::render()
{
    SurfaceMenu::render();

    MCTextureText text(m_text);

    const int shadowY = -2;
    const int shadowX = 2;

    text.setColor(MCGLColor(0.25, 0.75, 1.0, 1.0));
    text.setGlyphSize(20, 20);
    text.setShadowOffset(shadowX, shadowY);

    auto && font = MCAssetManager::textureFontManager().font(Game::instance().fontName());
    text.render(width() / 2 - text.width(font) / 2 + 20, height() / 2 + 60, nullptr, font);
}

static const char * PRESS_KEY_MENU_ID = "pressKeyMenu";

KeyConfigMenu::KeyConfigMenu(std::string id, int width, int height)
  : SurfaceMenu("settingsBack", id, width, height, Menu::Style::VerticalList)
{
    MTFH::MenuManager::instance().addMenu(std::make_shared<PressKeyMenu>(PRESS_KEY_MENU_ID, width, height));

    addPlayerTwoConfig(width, height);
    addPlayerOneConfig(width, height);
}

void KeyConfigMenu::addPlayerOneConfig(int width, int height)
{
    const int itemHeight = height / 10;

    using MTFH::MenuItem;
    using MTFH::MenuManager;

    const auto playerOneAccelerate =
      std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Player One Accelerate").toUpper().toStdWString());
    playerOneAccelerate->setView(std::make_shared<TextMenuItemView>(20, *playerOneAccelerate));
    playerOneAccelerate->setAction(
      []() {
          Game::instance().eventHandler().enableCaptureMode(InputHandler::Action::Up, 0);
          MenuManager::instance().pushMenu(PRESS_KEY_MENU_ID);
      });

    const auto playerOneBrake =
      std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Player One Brake").toUpper().toStdWString());
    playerOneBrake->setView(std::make_shared<TextMenuItemView>(20, *playerOneBrake));
    playerOneBrake->setAction(
      []() {
          Game::instance().eventHandler().enableCaptureMode(InputHandler::Action::Down, 0);
          MenuManager::instance().pushMenu(PRESS_KEY_MENU_ID);
      });

    const auto playerOneTurnLeft =
      std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Player One Turn Left").toUpper().toStdWString());
    playerOneTurnLeft->setView(std::make_shared<TextMenuItemView>(20, *playerOneTurnLeft));
    playerOneTurnLeft->setAction(
      []() {
          Game::instance().eventHandler().enableCaptureMode(InputHandler::Action::Left, 0);
          MenuManager::instance().pushMenu(PRESS_KEY_MENU_ID);
      });

    const auto playerOneTurnRight =
      std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Player One Turn Right").toUpper().toStdWString());
    playerOneTurnRight->setView(std::make_shared<TextMenuItemView>(20, *playerOneTurnRight));
    playerOneTurnRight->setAction(
      []() {
          Game::instance().eventHandler().enableCaptureMode(InputHandler::Action::Right, 0);
          MenuManager::instance().pushMenu(PRESS_KEY_MENU_ID);
      });

    addItem(playerOneBrake);
    addItem(playerOneAccelerate);
    addItem(playerOneTurnRight);
    addItem(playerOneTurnLeft);
}

void KeyConfigMenu::addPlayerTwoConfig(int width, int height)
{
    const int itemHeight = height / 10;

    using MTFH::MenuItem;
    using MTFH::MenuManager;

    const auto playerTwoAccelerate =
      std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Player Two Accelerate").toUpper().toStdWString());
    playerTwoAccelerate->setView(std::make_shared<TextMenuItemView>(20, *playerTwoAccelerate));
    playerTwoAccelerate->setAction(
      []() {
          Game::instance().eventHandler().enableCaptureMode(InputHandler::Action::Up, 1);
          MenuManager::instance().pushMenu(PRESS_KEY_MENU_ID);
      });

    const auto playerTwoBrake =
      std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Player Two Brake").toUpper().toStdWString());
    playerTwoBrake->setView(std::make_shared<TextMenuItemView>(20, *playerTwoBrake));
    playerTwoBrake->setAction(
      []() {
          Game::instance().eventHandler().enableCaptureMode(InputHandler::Action::Down, 1);
          MenuManager::instance().pushMenu(PRESS_KEY_MENU_ID);
      });

    const auto playerTwoTurnLeft =
      std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Player Two Turn Left").toUpper().toStdWString());
    playerTwoTurnLeft->setView(std::make_shared<TextMenuItemView>(20, *playerTwoTurnLeft));
    playerTwoTurnLeft->setAction(
      []() {
          Game::instance().eventHandler().enableCaptureMode(InputHandler::Action::Left, 1);
          MenuManager::instance().pushMenu(PRESS_KEY_MENU_ID);
      });

    const auto playerTwoTurnRight =
      std::make_shared<MenuItem>(width, itemHeight, QObject::tr("Player Two Turn Right").toUpper().toStdWString());
    playerTwoTurnRight->setView(std::make_shared<TextMenuItemView>(20, *playerTwoTurnRight));
    playerTwoTurnRight->setAction(
      []() {
          Game::instance().eventHandler().enableCaptureMode(InputHandler::Action::Right, 1);
          MenuManager::instance().pushMenu(PRESS_KEY_MENU_ID);
      });

    addItem(playerTwoBrake);
    addItem(playerTwoAccelerate);
    addItem(playerTwoTurnRight);
    addItem(playerTwoTurnLeft);
}

KeyConfigMenu::~KeyConfigMenu() = default;
