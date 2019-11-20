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

#include "help.hpp"
#include "../common/config.hpp"
#include "renderer.hpp"

#include <MCTextureText>
#include <QObject> // For QObject::tr()

Help::Help(std::string id, int width, int height)
  : SurfaceMenu("helpBack", id, width, height, Menu::Style::VerticalList, true)
{
}

void Help::render()
{
    static MCTextureText HELP_TEXT(
      (QObject::tr("GAME GOAL\n\n"
                   "You are racing against eleven\ncomputer players.\n\n"
                   "Your best position will be\nthe next start position.\n\n"
                   "Finish in TOP-6 to unlock\na new race track!\n\n"
                   "CONTROLS FOR PLAYER 1\n\n"
                   "Turn left  : Left\n"
                   "Turn right : Right\n"
                   "Accelerate : Up / RIGHT SHIFT\n"
                   "Brake      : Down / RIGHT CTRL\n\n"
                   "CONTROLS FOR PLAYER 2\n\n"
                   "Turn left  : A\n"
                   "Turn right : D\n"
                   "Accelerate : W / LEFT SHIFT\n"
                   "Brake      : S / LEFT CTRL\n\n"
                   "Quit       : ESC/Q\n"
                   "Pause      : P")
       + "\n\n" + Config::General::WEB_SITE_URL)
        .toStdWString());

    SurfaceMenu::render();

    HELP_TEXT.setGlyphSize(20, 20 * height() / 640);
    HELP_TEXT.render(
      x() + width() / 2 - HELP_TEXT.width(font()) / 2,
      y() + height() / 2 + HELP_TEXT.height(font()) / 2, nullptr, font());
}
