// This file is part of Dust Racing (DustRAC).
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
#include <QGraphicsSceneMouseEvent>

MWSelector::MWSelector(QSizeF size, MWWidget * parent) :
    MWWidget(size, parent),
    m_nextPressing(false),
    m_prevPressing(false)
{}

void MWSelector::paint(QPainter * painter,
                       const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->save();

    paintBackground(painter);
    paintArrows(painter);

    painter->restore();
}

void MWSelector::paintBackground(QPainter * painter)
{
    // Paint background not covered by the arrows
    QRectF bgRect(-size().width()  / 2 + size().height(),
                  -size().height() / 2,
                   size().width() - size().height() * 2,
                   size().height());

    if (clickable() && pressing())
    {
        painter->fillRect(bgRect, getHighlightColor(bgColor()));
    }
    else
    {
        painter->fillRect(bgRect, bgColor());
    }
}

void MWSelector::paintArrows(QPainter * painter)
{
    QRectF rectPrev(-size().width()  / 2, -size().height() / 2,
                     size().height(),      size().height());

    QRectF rectNext(size().width()  / 2 - size().height(), -size().height() / 2,
                    size().height(),                        size().height());

    if (m_prevPressing)
    {
        painter->fillRect(rectPrev, QBrush(getHighlightColor(bgColor())));
        painter->fillRect(rectNext, QBrush(bgColor()));
    }
    else if (m_nextPressing)
    {
        painter->fillRect(rectPrev, QBrush(bgColor()));
        painter->fillRect(rectNext, QBrush(getHighlightColor(bgColor())));
    }
    else
    {
        painter->fillRect(rectPrev, QBrush(bgColor()));
        painter->fillRect(rectNext, QBrush(bgColor()));
    }

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

void MWSelector::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    if (event->pos().x() > -size().width() / 2 &&
        event->pos().x() < -size().width() / 2 + size().height())
    {
        m_prevPressing = true;
        update();
    }
    else if (event->pos().x() < size().width() / 2 &&
             event->pos().x() > size().width() / 2 - size().height())
    {
        m_nextPressing = true;
        update();
    }
    else if (clickable())
    {
        setPressing(true);
        update();
    }
}

void MWSelector::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    Q_UNUSED(event)

    if (m_prevPressing)
    {
        m_prevPressing = false;
        emit prevClicked();
        update();
    }
    else if (m_nextPressing)
    {
        m_nextPressing = false;
        emit nextClicked();
        update();
    }
    else if (pressing())
    {
        setPressing(false);
        emit clicked();
        update();
    }
}
