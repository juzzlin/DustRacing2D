// This file is part of Dust Racing 2D.
// Copyright (C) 2014 Jussi Lind <jussi.lind@iki.fi>
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

#include "fontfactory.hpp"
#include "game.hpp"

#include <MCAssetManager>
#include <MCSurface>
#include <MCSurfaceManager>
#include <MCSurfaceMetaData>
#include <MCTextureGlyph>

#include "simple_logger.hpp"

#include <QFont>
#include <QPainter>
#include <QPixmap>

#include <cassert>

// Note!!: MSVC requires that this file is saved in "UTF-8 with BOM" format
// in order to make all the wide chars work correctly. On Linux/Unix it doesn't
// matter whether there's a BOM or not.

static const std::vector<wchar_t> glyphs(
  { // Russian letters in UTF-8
    L'А', L'Б', L'В', L'Г', L'Д', L'Е', L'Ё', L'Ж',
    L'З', L'И', L'Й', L'К', L'Л', L'М', L'Н', L'О',
    L'П', L'Р', L'С', L'Т', L'У', L'Ф', L'Х', L'Ц',
    L'Ч', L'Ш', L'Щ', L'Ъ', L'Ы', L'Ь', L'Э', L'Ю',
    L'Я',

    L'а', L'б', L'в', L'г', L'д', L'е', L'ё', L'ж',
    L'з', L'и', L'й', L'к', L'л', L'м', L'н', L'о',
    L'п', L'р', L'с', L'т', L'у', L'ф', L'х', L'ц',
    L'ч', L'ш', L'щ', L'ъ', L'ы', L'ь', L'э', L'ю',
    L'я', L'№',
    // End of russian letters in UTF-8

    L'A', L'B', L'C', L'D', L'E', L'F', L'G', L'H',
    L'I', L'J', L'K', L'L', L'M', L'N', L'O', L'P',
    L'Q', L'R', L'S', L'T', L'U', L'V', L'W', L'X',
    L'Y', L'Z', L' ', L'Ä', L'Ö', L'Ü', L'Å', L' ',
    L'a', L'b', L'c', L'd', L'e', L'f', L'g', L'h',
    L'i', L'j', L'k', L'l', L'm', L'n', L'o', L'p',
    L'q', L'r', L's', L't', L'u', L'v', L'w', L'x',
    L'y', L'z', L' ', L'ä', L'ö', L'ü', L'å', L'\"',
    L'0', L'1', L'2', L'3', L'4', L'5', L'6', L'7',
    L'8', L'9', L'!', L'\"', L'#', L'$', L'%', L'&',
    L'@', L'(', L')', L'*', L'+', L',', L'-', L'.',
    L'/', L':', L';', L'<', L'=', L'>', L'?', L'_',
    L'Â', L'À', L'É', L'È', L'Ê', L'Ë', L'Î', L'Ï',
    L'Ô', L'Û', L'Ç', L'â', L'à', L'é', L'è', L'ê',
    L'ë', L'î', L'ï', L'ô', L'û', L'ç', L'\'', L' ' });

static void addFallbacks(MCTextureFontData & fontData)
{
    fontData.fallback[L'Á'] = L'A';
    fontData.fallback[L'á'] = L'a';
    fontData.fallback[L'Č'] = L'C';
    fontData.fallback[L'č'] = L'c';
    fontData.fallback[L'Ď'] = L'D';
    fontData.fallback[L'ď'] = L'd';
    fontData.fallback[L'Ě'] = L'E';
    fontData.fallback[L'ě'] = L'e';
    fontData.fallback[L'Í'] = L'I';
    fontData.fallback[L'ì'] = L'i';
    fontData.fallback[L'í'] = L'i';
    fontData.fallback[L'Ň'] = L'N';
    fontData.fallback[L'ň'] = L'n';
    fontData.fallback[L'Ó'] = L'O';
    fontData.fallback[L'ò'] = L'o';
    fontData.fallback[L'ó'] = L'o';
    fontData.fallback[L'Ř'] = L'R';
    fontData.fallback[L'ř'] = L'r';
    fontData.fallback[L'Š'] = L'S';
    fontData.fallback[L'š'] = L's';
    fontData.fallback[L'Ť'] = L'T';
    fontData.fallback[L'ť'] = L't';
    fontData.fallback[L'Ú'] = L'U';
    fontData.fallback[L'ú'] = L'u';
    fontData.fallback[L'Ů'] = L'U';
    fontData.fallback[L'ů'] = L'u';
    fontData.fallback[L'ù'] = L'u';
    fontData.fallback[L'Ý'] = L'Y';
    fontData.fallback[L'ý'] = L'y';
    fontData.fallback[L'Ž'] = L'Z';
    fontData.fallback[L'ž'] = L'z';
}

MCTextureFontData FontFactory::generateFontData(QString family)
{
    const int cols = 8;
    const int rows = glyphs.size() / cols;

    int slotWidth = 64;
    int slotHeight = 64;
    const int glyphHeight = 64;

    QFont font;
    font.setFamily(family);
    font.setStyleHint(QFont::Monospace);
    font.setPixelSize(glyphHeight);
    font.setBold(true);

    // Calculate the widest glyph and tune the slot size accordingly
    QFontMetrics fm(font);
    for (int j = 0; j < rows; j++)
    {
        for (int i = 0; i < cols; i++)
        {
            const int glyphIndex = j * cols + i;
            const QString text(glyphs.at(glyphIndex));
            if (fm.width(text) > slotWidth)
            {
                slotWidth = fm.width(text);
            }
        }
    }
    if (fm.height() > slotHeight)
    {
        slotHeight = fm.height();
    }

    // Add a little margin
    slotHeight += 4;

    juzzlin::L().debug() << "Font slot size: " << slotWidth << "x" << slotHeight;

    const int textureW = cols * slotWidth;
    const int textureH = rows * slotHeight;

    juzzlin::L().debug() << "Font texture size (initial): " << textureW << "x" << textureH;

    QPixmap fontPixmap(textureW, textureH);
    fontPixmap.fill(Qt::transparent);

    MCTextureFontData fontData;

    for (int j = 0; j < rows; j++)
    {
        for (int i = 0; i < cols; i++)
        {
            const int glyphIndex = j * cols + i;
            const QString text(glyphs.at(glyphIndex));

            if (text.length())
            {
                QPainter painter;
                painter.begin(&fontPixmap);

                painter.setFont(font);
                painter.setPen(QColor(255, 255, 255));

                painter.drawText(
                  i * slotWidth,
                  j * slotHeight,
                  slotWidth,
                  slotHeight,
                  Qt::AlignCenter,
                  text);

                painter.end();

                MCTextureFontData::Glyph glyph;
                glyph.name = glyphs.at(glyphIndex);
                glyph.x0 = i * textureW / cols;
                glyph.y0 = (rows - j) * textureH / rows;
                glyph.x1 = (i + 1) * textureW / cols;
                glyph.y1 = (rows - j - 1) * textureH / rows;

                fontData.glyphs.push_back(glyph);
            }
        }
    }

    // Note, that the size of the pixmap doesn't affect the size of the actual
    // surface / texture rendering that pixmap.
    MCSurfaceMetaData surfaceData;
    surfaceData.height = std::pair<int, bool>(textureH, true);
    surfaceData.width = std::pair<int, bool>(textureW, true);
    surfaceData.minFilter = std::pair<GLint, bool>(GL_LINEAR, true);
    surfaceData.magFilter = std::pair<GLint, bool>(GL_LINEAR, true);
    surfaceData.handle = family.toStdString();

    MCAssetManager::surfaceManager().createSurfaceFromImage(surfaceData, fontPixmap.toImage());
    fontData.name = Game::instance().fontName();
    fontData.surface = surfaceData.handle;

    // Compensate the "too big" texture rectangle per glyph
    fontData.xDensity = 0.75f;
    fontData.yDensity = 1.0f;

    addFallbacks(fontData);

    return fontData;
}
