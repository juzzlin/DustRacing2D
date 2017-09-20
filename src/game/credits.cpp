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

// Note!!: MSVC requires that this file is saved in "UTF-8 with BOM" format
// in order to make all the wide chars work correctly. On Linux/Unix it doesn't
// matter whether there's a BOM or not.

Credits::Credits(std::string id, int width, int height)
    : SurfaceMenu("creditsBack", id, width, height, Menu::Style::VerticalList, true)
{
}

void Credits::render()
{
    static MCTextureText CREDITS_TEXT(
        (QObject::tr("PROGRAMMING BY").toStdWString() +
        L"\n\n" +
        L"* Jussi Lind\n\n" +
        QObject::tr("GRAPHICS BY").toStdWString() +
        L"\n\n" +
        L"* Jussi Lind\n\n" +
        QObject::tr("RACE TRACKS BY").toStdWString() +
        L"\n\n" +
        L"* Jussi Lind\n\n" +
        QObject::tr("CONTRIBUTORS").toStdWString() +
        L"\n\n" +
        L"* Markus Rinne\n\n" +
        L"* Paolo Straffi\n\n" +
        L"* Pavel Fric\n\n" +
        L"* Rémi Verschelde\n\n" +
        QObject::tr("SPECIAL THANKS").toStdWString() +
        L"\n\n" +
        L"* Tommi Martela\n\n"
        L"* Alex Rietveld\n\n"
        L"* Matthias Mailänder\n")
    );

    SurfaceMenu::render();

    CREDITS_TEXT.setGlyphSize(20, 20 * height() / 640);
    CREDITS_TEXT.render(
        width() / 2 - CREDITS_TEXT.width(font()) / 2,
        height() / 2 + CREDITS_TEXT.height(font()) / 2, nullptr, font());
}
