// This file is part of Dust Rallycross (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#include "mwselector.h"
#include <QPainter>

MWSelector::MWSelector(QSizeF size, MWWidget * parent) :
    MWWidget(size, parent)
{}

void MWSelector::paint(QPainter * painter,
                       const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->save();

    MWWidget::paintBackground(painter);
    paintArrows(painter);

    painter->restore();
}

void MWSelector::paintArrows(QPainter * painter)
{
    {
        QPainterPath path;
        QPolygon triangle;
        triangle << QPoint(size().width() / 2, 0)
                 << QPoint(size().width() / 2 - size().height(), -size().height() / 2)
                 << QPoint(size().width() / 2 - size().height(),  size().height() / 2);
        path.addPolygon(triangle);
        painter->fillPath(path, QBrush(fgColor()));
    }

    {
        QPainterPath path;
        QPolygon triangle;
        triangle << QPoint(-size().width() / 2, 0)
                 << QPoint(-size().width() / 2 + size().height(), -size().height() / 2)
                 << QPoint(-size().width() / 2 + size().height(),  size().height() / 2);
        path.addPolygon(triangle);
        painter->fillPath(path, QBrush(fgColor()));
    }
}
