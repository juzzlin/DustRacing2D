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

#include "help.hpp"
#include "renderer.hpp"

#include <MCTextureText>

static MCTextureText HELP_TEXT(
    "GAME GOAL\n\n"
    "You are racing against nine\ncomputer players.\n\n"
    "Finish in TOP-5 to unlock\na new race track!\n\n"
    "CONTROLS FOR PLAYER 1\n\n"
    "Turn left  : Left\n"
    "Turn right : Right\n"
    "Accelerate : Up\n"
    "Brake      : Down/B\n\n"
    "CONTROLS FOR PLAYER 2\n\n"
    "Turn left  : A\n"
    "Turn right : D\n"
    "Accelerate : W\n"
    "Brake      : S\n\n"
    "Quit       : ESC/Q\n"
    "Pause      : P"
);

Help::Help(std::string id, int width, int height)
: SurfaceMenu("helpBack", id, width, height, Menu::MS_VERTICAL_LIST)
{
}

void Help::render()
{
    SurfaceMenu::render();

    HELP_TEXT.setGlyphSize(20, 20);
    HELP_TEXT.render(
        width()  / 2 - HELP_TEXT.width()  / 2,
        height() / 2 + HELP_TEXT.height() / 2, nullptr, font());
}
