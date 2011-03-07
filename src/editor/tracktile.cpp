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

#include "tracktile.h"
#include "tiletypedialog.h"
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QAction>

// Width of the highlight shown when the tile is active
const int HIGHLIGHT_WIDTH = 11;

TrackTile * TrackTile::m_activeTile = NULL;

TrackTile::TrackTile(QSizeF size, QPointF location, TileType type):
    m_size(size),
    m_type(type),
    m_active(false)
{
    setPos(location);
}

QRectF TrackTile::boundingRect () const
{
    return QRectF(-m_size.width() / 2, -m_size.height() / 2,
                   m_size.width(), m_size.height());
}

void TrackTile::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->save();

    QPen pen;
    pen.setColor(QColor(0, 0, 0));
    pen.setJoinStyle(Qt::MiterJoin);
    painter->setPen(pen);
    painter->drawRect(-m_size.width() / 2, -m_size.height() / 2,
                       m_size.width(),      m_size.height());

    if (m_active)
    {
        pen.setColor(QColor(0, 0, 255, 64));
        pen.setWidth(HIGHLIGHT_WIDTH);

        painter->setPen(pen);
        painter->drawRect(-m_size.width()  / 2 + HIGHLIGHT_WIDTH / 2,
                          -m_size.height() / 2 + HIGHLIGHT_WIDTH / 2,
                           m_size.width()      - HIGHLIGHT_WIDTH,
                           m_size.height()     - HIGHLIGHT_WIDTH);
    }

    painter->restore();
}

void TrackTile::setActive(bool active)
{
    m_active = active;

    if (active && TrackTile::m_activeTile != this)
    {
        if (TrackTile::m_activeTile)
        {
            TrackTile::m_activeTile->setActive(false);
        }

        TrackTile::m_activeTile = this;
    }

    update();
}

TrackTile * TrackTile::activeTile()
{
    return TrackTile::m_activeTile;
}

void TrackTile::doContextMenu(QPoint pos)
{
    QAction setType(QWidget::tr("Set type.."), NULL);
    QChar degreeSign(176);

    QString dummy1(QString(QWidget::tr("Rotate 90")) +
                   degreeSign + QWidget::tr(" CW.."));
    QAction rotate90CW(dummy1, NULL);

    QString dummy2(QString(QWidget::tr("Rotate 90")) +
                   degreeSign + QWidget::tr(" CCW.."));
    QAction rotate90CCW(dummy2, NULL);

    QMenu menu;
    menu.addActions(QList<QAction *>()
                    << &setType << &rotate90CW << &rotate90CCW);

    menu.exec(pos);
}

void TrackTile::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    setActive(true);

    if (event->button() == Qt::RightButton)
    {
        if (scene() && !scene()->views().isEmpty())
        {
            // This seems to be a bit tricky to get right due to the scaling transformation
            // applied to the view.
            QPoint p = scene()->views()[0]->mapFromScene(pos() + event->pos().toPoint()) +
                       QPoint(m_size.width(), m_size.height() / 2);

            doContextMenu(p);
        }
    }

    QGraphicsItem::mousePressEvent(event);
}

void TrackTile::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

