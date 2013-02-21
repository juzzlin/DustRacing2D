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
#include <MCSurfaceMetaData>
#include <MCSurfaceManager>

// These are needed to generate the number plate surface on top of the car.
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QPixmap>

MCSurface & GraphicsFactory::generateNumberSurface(int index)
{
    static std::vector<std::string> numberPlates(
        {"1", "11", "10", "9", "8", "7", "6", "5", "4", "3", "2", "x"});

    const int w = 32;
    const int h = 32;

    QPixmap numberPixmap(w, h);
    numberPixmap.fill(Qt::white);

    QPainter painter;
    painter.begin(&numberPixmap);
    QFont font;
    font.setPixelSize(32);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(QColor(0, 0, 0));
    const QFontMetrics fm = painter.fontMetrics();
    const QString text = numberPlates.at(index).c_str();
    painter.drawText(w / 2 - fm.width(text) / 2, h / 2 + fm.height() / 2 - fm.descent(), text);
    painter.end();

    MCSurfaceMetaData surfaceData;
    surfaceData.height    = 11;
    surfaceData.heightSet = true;
    surfaceData.width     = 11;
    surfaceData.widthSet  = true;

    return
        MCAssetManager::surfaceManager().createSurfaceFromImage(surfaceData, numberPixmap.toImage());
}
