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

#include "tracktile.h"
#include "tiletypedialog.h"
#include "tileanimator.h"
#include "mainwindow.h"
#include "config.h"
#include "../common/trackdata.h"

#include <QAction>
#include <QGraphicsLineItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPainter>

TrackTile * TrackTile::m_activeTile = NULL;

TrackTile::TrackTile(TrackData * trackData, QPointF location, QPoint matrixLocation,
                     const QString & type)
: TrackTileBase(trackData, location, matrixLocation, type)
, m_size(QSizeF(TILE_W, TILE_H))
, m_active(false)
, m_animator(new TileAnimator(this))
, m_routeLine(NULL)
{
    setPos(location);
}

void TrackTile::setRouteIndex(int index)
{
    TrackTileBase::setRouteIndex(index);
    update();
}

void TrackTile::setRouteLine(QGraphicsLineItem * routeLine)
{
    m_routeLine = routeLine;
}

QGraphicsLineItem * TrackTile::routeLine() const
{
    return m_routeLine;
}

void TrackTile::setRouteDirection(TrackTileBase::RouteDirection direction)
{
    TrackTileBase::setRouteDirection(direction);
    update();
}

QRectF TrackTile::boundingRect () const
{
    return QRectF(-m_size.width() / 2, -m_size.height() / 2,
                   m_size.width(), m_size.height());
}

void TrackTile::paint(QPainter * painter,
                      const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->save();

    QPen pen;
    pen.setJoinStyle(Qt::MiterJoin);

    // Render the tile pixmap if tile is not cleared.
    if (tileType() != "clear")
    {
        painter->drawPixmap(-m_size.width() / 2, -m_size.height() / 2,
                            m_size.width(), m_size.height(),
                            m_pixmap);
    }
    else
    {
        painter->drawPixmap(-m_size.width() / 2, -m_size.height() / 2,
                            m_size.width(), m_size.height(),
                            QPixmap(Config::CLEAR_PATH));

        pen.setColor(QColor(0, 0, 0));
        painter->setPen(pen);
        painter->drawRect(-m_size.width() / 2, -m_size.height() / 2,
                           m_size.width(),      m_size.height());
    }

    // Render highlight
    if (m_active)
    {
        painter->fillRect(boundingRect(), QBrush(QColor(0, 0, 0, 64)));
    }

    // Render white (hill) or black (gorge) X
    switch (profile())
    {
        case TrackTileBase::TP_HILL:
        {
            pen.setColor(QColor(255, 255, 255));
            painter->setPen(pen);
            const int w = m_size.width();
            const int h = m_size.height();
            painter->drawLine(-w / 2, -h / 2, w / 2,  h / 2);
            painter->drawLine(-w / 2,  h / 2, w / 2, -h / 2);
        }
        break;
        case TrackTileBase::TP_GORGE:
        {
            pen.setColor(QColor(0, 0, 0));
            painter->setPen(pen);
            const int w = m_size.width();
            const int h = m_size.height();
            painter->drawLine(-w / 2, -h / 2, w / 2,  h / 2);
            painter->drawLine(-w / 2,  h / 2, w / 2, -h / 2);
        }
        break;
    default:
        break;
    }

    // Render route arrow arrow
    if (routeIndex() == 0)
    {
        // Cancel possible rotation so that the text is not
        // rotated and rotate for the arrow head.
        QTransform transform;
        switch (routeDirection())
        {
        case RD_LEFT:
            transform.rotate(180 - rotation());
            break;
        case RD_RIGHT:
            transform.rotate(0 - rotation());
            break;
        case RD_UP:
            transform.rotate(270 - rotation());
            break;
        case RD_DOWN:
            transform.rotate(90 - rotation());
            break;
        default:
        case RD_NONE:
            break;
        }
        painter->setTransform(transform, true);

        // Draw an arrow head
        QPainterPath path;
        QPolygon triangle;
        triangle << QPoint( m_size.width() / 3,  0)
                 << QPoint(                  0, -m_size.height() / 4)
                 << QPoint(                  0,  m_size.height() / 4)
                 << QPoint( m_size.width() / 3,  0);
        path.addPolygon(triangle);
        painter->strokePath(path,
                            QPen(QBrush(QColor(0, 0, 255, 64)), 15,
                                 Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin));
    }

    painter->restore();
}

void TrackTile::setActive(bool active)
{
    m_active = active;

    if (active && TrackTile::m_activeTile != this)
    {
        if (TrackTile::m_activeTile)
            TrackTile::m_activeTile->setActive(false);

        TrackTile::m_activeTile = this;
    }

    update();
}

void TrackTile::setActiveTile(TrackTile * tile)
{
    if (tile)
    {
        tile->setActive(true);
    }
    else
    {
        if (activeTile())
            activeTile()->setActive(false);

        TrackTile::m_activeTile = NULL;
    }
}

TrackTile * TrackTile::activeTile()
{
    return TrackTile::m_activeTile;
}

void TrackTile::rotate90CW()
{
    m_animator->rotate90CW();
}

void TrackTile::rotate90CCW()
{
    m_animator->rotate90CCW();
}

void TrackTile::setTileType(const QString & type)
{
    TrackTileBase::setTileType(type);
    update();
}

QPixmap TrackTile::pixmap() const
{
    return m_pixmap;
}

void TrackTile::setPixmap(const QPixmap & pixmap)
{
    m_pixmap = pixmap;
    update();
}

TrackTile::~TrackTile()
{
    delete m_animator;
}
