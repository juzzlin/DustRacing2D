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
#include <QPainter>

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

    if (active)
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

void TrackTile::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    setActive(true);

    QGraphicsItem::mousePressEvent(event);
}

