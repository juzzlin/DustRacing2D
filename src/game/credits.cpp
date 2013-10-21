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

#include "credits.hpp"
#include "renderer.hpp"

#include <MCTextureText>
#include <QObject> // For QObject::tr()

Credits::Credits(std::string id, int width, int height)
: SurfaceMenu("creditsBack", id, width, height, Menu::MS_VERTICAL_LIST, true)
{
}

void Credits::render()
{
    static MCTextureText CREDITS_TEXT(
        (QObject::tr("PROGRAMMING BY") + "\n\n" +
        "* Jussi Lind\n\n" +
        QObject::tr("GRAPHICS BY") + "\n\n" +
        "* Jussi Lind\n\n" +
        QObject::tr("RACE TRACKS BY") + "\n\n" +
        "* Jussi Lind\n\n" +
        QObject::tr("FONT") + "\n\n" +
        "* Ubuntu Monospace\n\n" +
        QObject::tr("PATCHES") + "\n\n" +
        "* Markus Rinne\n\n" +
        QObject::tr("SPECIAL THANKS") + "\n\n" +
        "* Tommi Martela\n\n"
        "* Alex Rietveld\n").toStdString()
    );

    SurfaceMenu::render();

    CREDITS_TEXT.setGlyphSize(20, 20);
    CREDITS_TEXT.render(
        width()  / 2 - CREDITS_TEXT.width()  / 2,
        height() / 2 + CREDITS_TEXT.height() / 2, nullptr, font());
}
