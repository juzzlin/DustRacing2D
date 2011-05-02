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
#include "trackdata.h"
#include "tiletypedialog.h"
#include "tileanimator.h"
#include "mainwindow.h"

#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QAction>

TrackTile * TrackTile::m_activeTile = NULL;

TrackTile::TrackTile(TrackData * trackData, QPointF location, QPoint matrixLocation,
                     const QString & type):
    m_trackData(trackData),
    m_size(QSizeF(TILE_W, TILE_H)),
    m_tileType(type),
    m_active(false),
    m_animator(new TileAnimator(this)),
    m_routeIndex(-1),
    m_matrixLocation(matrixLocation),
    m_routeDirection(TrackTile::RD_NONE)
{
    setPos(location);
}

void TrackTile::setRouteIndex(int index)
{
    m_routeIndex = index;
    update();
}

int TrackTile::routeIndex() const
{
    return m_routeIndex;
}

void TrackTile::setRouteDirection(TrackTile::RouteDirection direction)
{
    m_routeDirection = direction;
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

    // Render the tile image
    if (m_tileType == "grass")
    {
        painter->drawPixmap(-m_size.width() / 2, -m_size.height() / 2,
                            m_size.width(), m_size.height(),
                            QPixmap(":/grass.png"));
    }
    else if (m_tileType == "straight")
    {
        painter->drawPixmap(-m_size.width() / 2, -m_size.height() / 2,
                            m_size.width(), m_size.height(),
                            QPixmap(":/straight.png"));
    }
    else if (m_tileType == "corner")
    {
        painter->drawPixmap(-m_size.width() / 2, -m_size.height() / 2,
                            m_size.width(), m_size.height(),
                            QPixmap(":/corner.png"));
    }
    else if (m_tileType == "finish")
    {
        painter->drawPixmap(-m_size.width() / 2, -m_size.height() / 2,
                            m_size.width(), m_size.height(),
                            QPixmap(":/finish.png"));
    }
    else if (m_tileType == "clear")
    {
        painter->drawPixmap(-m_size.width() / 2, -m_size.height() / 2,
                            m_size.width(), m_size.height(),
                            QPixmap(":/clear.png"));

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

    // Render route index and arrow
    if (m_routeIndex >= 0)
    {
        pen.setColor(QColor(255, 255, 255, 127));
        painter->setPen(pen);

        // Cancel possible rotation so that the text is not
        // rotated.
        QTransform transform;
        transform.rotate(-rotation());
        painter->setTransform(transform, true);

        switch (m_routeDirection)
        {
        case RD_LEFT:
            {
                QPainterPath path;
                QPolygon triangle;
                triangle << QPoint(-m_size.width() / 2,  0)
                         << QPoint(-m_size.width() / 3,  m_size.height() / 3)
                         << QPoint(-m_size.width() / 3, -m_size.height() / 3);
                path.addPolygon(triangle);
                painter->fillPath(path, QBrush(QColor(255, 255, 255, 127)));
            }
            break;

        case RD_RIGHT:
            {
                QPainterPath path;
                QPolygon triangle;
                triangle << QPoint(m_size.width() / 2,  0)
                         << QPoint(m_size.width() / 3, -m_size.height() / 3)
                         << QPoint(m_size.width() / 3,  m_size.height() / 3);
                path.addPolygon(triangle);
                painter->fillPath(path, QBrush(QColor(255, 255, 255, 127)));
            }
            break;

        case RD_UP:
            {
                QPainterPath path;
                QPolygon triangle;
                triangle << QPoint(0, -m_size.height() / 2)
                         << QPoint( m_size.width() / 3, -m_size.height() / 3)
                         << QPoint(-m_size.width() / 3, -m_size.height() / 3);
                path.addPolygon(triangle);
                painter->fillPath(path, QBrush(QColor(255, 255, 255, 127)));
            }
            break;

        case RD_DOWN:
            {
                QPainterPath path;
                QPolygon triangle;
                triangle << QPoint(0, m_size.height() / 2)
                         << QPoint( m_size.width() / 3, m_size.height() / 3)
                         << QPoint(-m_size.width() / 3, m_size.height() / 3);
                path.addPolygon(triangle);
                painter->fillPath(path, QBrush(QColor(255, 255, 255, 127)));
            }
            break;

        default:
        case RD_NONE:
            break;
        }

        if (m_routeIndex == 0)
        {
            QFont font;
            font.setPixelSize(m_size.height() / 2);
            painter->setFont(font);
            painter->drawText(boundingRect(), Qt::AlignCenter, "S");
        }
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

void TrackTile::setTileType(const QString & type)
{
    if (type != m_tileType)
    {
        m_tileType = type;

        update();
    }
}

const QString & TrackTile::tileType() const
{
    return m_tileType;
}

QPoint TrackTile::matrixLocation() const
{
    return m_matrixLocation;
}

void TrackTile::rotate90CW()
{
    m_animator->rotate90CW();
}

void TrackTile::rotate90CCW()
{
    m_animator->rotate90CCW();
}

TrackTile::~TrackTile()
{
    delete m_animator;
}
