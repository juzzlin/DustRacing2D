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

#include "graphicsfactory.hpp"
#include "game.hpp"

#include <MCAssetManager>
#include <MCSurface>
#include <MCSurfaceMetaData>
#include <MCSurfaceManager>
#include <MCTextureGlyph>

// These are needed to generate the number plate surface on top of the car.
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QPixmap>

#include <cassert>

MCSurface & GraphicsFactory::generateNumberSurface(int index)
{
    static std::vector<std::string> numberPlates(
        {"1", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "x"});
    assert(index < static_cast<int>(numberPlates.size()));

    const int pixmapWidth  = 32;
    const int pixmapHeight = 32;

    QPixmap numberPixmap(pixmapWidth, pixmapHeight);
    numberPixmap.fill(Qt::white);

    QPainter painter;
    painter.begin(&numberPixmap);

    QFont font;
    font.setPixelSize(pixmapHeight);
    font.setBold(true);

    painter.setFont(font);
    painter.setPen(QColor(0, 0, 0));
    const QFontMetrics fm = painter.fontMetrics();
    const QString text = numberPlates.at(index).c_str();

    painter.drawText(
        pixmapWidth / 2 - fm.width(text) / 2,
        pixmapHeight / 2 + fm.height() / 2 - fm.descent(),
        text);
    painter.end();

    // Note, that the size of the pixmap doesn't affect the size of the actual
    // surface / texture rendering that pixmap.
    MCSurfaceMetaData surfaceData;
    surfaceData.height = std::pair<int, bool>(9, true);
    surfaceData.width  = std::pair<int, bool>(9, true);

    return
        MCAssetManager::surfaceManager().createSurfaceFromImage(surfaceData, numberPixmap.toImage());
}

MCTextureFontData GraphicsFactory::generateFont()
{
    static std::vector<QString> glyphs(
        {"A", "B", "C", "D", "E", "F", "G", "H",
         "I", "J", "K", "L", "M", "N", "O", "P",
         "Q", "R", "S", "T", "U", "V", "W", "X",
         "Y", "Z", "Å", "Ä", "Ö", "Ü", "",  "",
         "a", "b", "c", "d", "e", "f", "g", "h",
         "i", "j", "k", "l", "m", "n", "o", "p",
         "q", "r", "s", "t", "u", "v", "w", "x",
         "y", "z", "å", "ä", "ö", "ü", "",  "",
         "0", "1", "2", "3", "4", "5", "6", "7",
         "8", "9", "!", "\"","#", "$", "%", "&",
         "'", "(", ")", "*", "+", ",", "-", ".",
         "/", ":", ";", "<", "=", ">", "?", " "});

    const int cols        = 8;
    const int rows        = glyphs.size() / cols;
    const int slotWidth   = 36;
    const int slotHeight  = 64;
    const int glyphHeight = 52;
    const int textureW    = cols * slotWidth;
    const int textureH    = rows * slotHeight;

    QPixmap fontPixmap(textureW, textureH);
    fontPixmap.fill(Qt::transparent);

    MCTextureFontData fontData;

    for (int j = 0; j < rows; j++)
    {
        for (int i = 0; i < cols; i++)
        {
            const int glyphIndex = j * cols + i;
            const QString text = glyphs.at(glyphIndex);

            if (text.length())
            {
                QPainter painter;
                painter.begin(&fontPixmap);

                QFont font;
                font.setPixelSize(glyphHeight);
                font.setBold(true);
                font.setFamily("Monospace");

                painter.setFont(font);
                painter.setPen(QColor(255, 255, 255));
                const QFontMetrics fm = painter.fontMetrics();

                painter.drawText(
                    i * slotWidth  + slotWidth  / 2 - fm.width(text) / 2,
                    j * slotHeight + slotHeight / 2 + fm.height()    / 2 - fm.descent(),
                    text);
                painter.end();

                MCTextureFontData::Glyph glyph;
                glyph.name = text.toStdString();
                glyph.x0   = i * textureW / cols;
                glyph.y0   = (rows - j) * textureH / rows;
                glyph.x1   = (i + 1) * textureW / cols;
                glyph.y1   = (rows - j - 1) * textureH / rows;

                fontData.glyphs.push_back(glyph);
            }
        }
    }

    // Note, that the size of the pixmap doesn't affect the size of the actual
    // surface / texture rendering that pixmap.
    MCSurfaceMetaData surfaceData;
    surfaceData.height    = std::pair<int, bool>(textureH, true);
    surfaceData.width     = std::pair<int, bool>(textureW, true);
    surfaceData.minFilter = std::pair<GLint, bool>(GL_LINEAR, true);
    surfaceData.magFilter = std::pair<GLint, bool>(GL_LINEAR, true);
    surfaceData.handle    = "generated";

    MCAssetManager::surfaceManager().createSurfaceFromImage(surfaceData, fontPixmap.toImage());
    fontData.name = Game::instance().fontName();
    fontData.surface = surfaceData.handle;

    return fontData;
}
