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

#include <MCAssetManager>
#include <MCSurface>
#include <MCSurfaceManager>
#include <MCSurfaceMetaData>

// These are needed to generate the number plate surface on top of the car.
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QPixmap>

#include <cassert>

MCSurface & GraphicsFactory::generateNumberSurface(size_t index)
{
    static const std::vector<std::string> numberPlates(
      { "1", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "x" });
    assert(index < numberPlates.size());

    const int pixmapWidth = 32;
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
    painter.drawText(
      0,
      0,
      pixmapWidth,
      pixmapHeight,
      Qt::AlignCenter,
      numberPlates.at(index).c_str());

    painter.end();

    // Note, that the size of the pixmap doesn't affect the size of the actual
    // surface / texture rendering that pixmap.
    MCSurfaceMetaData surfaceData;
    surfaceData.height = { 9, true };
    surfaceData.width = { 9, true };
    surfaceData.minFilter = { GL_LINEAR, true };
    surfaceData.magFilter = { GL_LINEAR, true };
    surfaceData.handle = "Number" + std::to_string(index);

    return MCAssetManager::surfaceManager().createSurfaceFromImage(surfaceData, numberPixmap.toImage());
}

MCSurface & GraphicsFactory::generateMinimapMarker()
{
    const int pixmapWidth = 64;
    const int pixmapHeight = 64;

    QPixmap markerPixmap(pixmapWidth, pixmapHeight);
    markerPixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&markerPixmap);
    painter.setBrush(Qt::white);
    auto rect = markerPixmap.rect();
    rect.adjust(2, 2, -2, -2);
    painter.drawEllipse(rect);
    painter.end();

    // Note, that the size of the pixmap doesn't affect the size of the actual
    // surface / texture rendering that pixmap.
    MCSurfaceMetaData surfaceData;
    surfaceData.height = { 9, true };
    surfaceData.width = { 9, true };
    surfaceData.minFilter = { GL_LINEAR, true };
    surfaceData.magFilter = { GL_LINEAR, true };
    surfaceData.handle = "Minimap";

    return MCAssetManager::surfaceManager().createSurfaceFromImage(surfaceData, markerPixmap.toImage());
}
