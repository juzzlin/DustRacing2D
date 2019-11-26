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

std::wstring margin(int width)
{
    std::wstring m = L"";
    for (int i = 0; i < width; i++)
    {
        m += L" ";
    }
    return m;
}

Credits::Credits(std::string id, int width, int height)
  : SurfaceMenu("creditsBack", id, width, height, Menu::Style::VerticalList, true)
{
    m_texts = {
        MCTextureText(margin(8) + QObject::tr("PROGRAMMING").toUpper().toStdWString() + L"\n\n" + L"Jussi Lind <jussi.lind@iki.fi>\n\n"),

        MCTextureText(margin(13) + QObject::tr("GRAPHICS").toUpper().toStdWString() + L"\n\n" + L"Jussi Lind\n\n" + L"Ville Mäkiranta (original graphics)\n\n"),

        MCTextureText(margin(5) + QObject::tr("LEVEL DESIGN").toUpper().toStdWString() + L"\n\n" + L"Jussi Lind\n\n" + L"Wuzzy <almikes@aol.com>\n\n"),

        MCTextureText(margin(14) + QObject::tr("TRANSLATIONS").toUpper().toStdWString() + L"\n\n" + L"Pavel Fric <pavelfric@seznam.cz> (cs)\n\n" + L"Wuzzy <almikes@aol.com> (de)\n\n" + L"Paolo Straffi <p_straffi@hotmail.com> (it)\n\n" + L"Jussi Lind (fi)\n\n" + L"Rémi Verschelde <remi@verschelde.fr> (fr)\n\n"),

        MCTextureText(margin(14) + QObject::tr("PATCHES").toUpper().toStdWString() + L"\n\n" + L"Markus Rinne\n\n" + L"Paolo Straffi\n\n" + L"Pavel Fric\n\n" + L"Rémi Verschelde\n\n" + L"Pino Toscano <toscano.pino@tiscali.it>\n\n" + L"David Carlier <devnexen@gmail.com>\n\n" + L"Dimitriy Marakasov <amdmi3@amdmi3.ru>\n\n" + L"jarssoft <jari.m.saari@gmail.com>\n\n"),

        MCTextureText(margin(2) + QObject::tr("SPECIAL THANKS").toUpper().toStdWString() + L"\n\n" + L"Tommi Martela\n\n"
                                                                                                     L"Alex Rietveld\n\n"
                                                                                                     L"Matthias Mailänder\n")
    };
}

void Credits::render()
{
    SurfaceMenu::render();

    m_time++;
    if (m_time > 120)
    {
        m_time = 0;
        m_textIndex++;
        m_textIndex = m_textIndex % m_texts.size();
    }

    auto && text = m_texts.at(m_textIndex);
    text.setGlyphSize(20, 20 * height() / 640);
    text.render(
      x() + width() / 2 - text.width(font()) / 2,
      y() + height() / 2 + text.height(font()) / 2, nullptr, font());
}
