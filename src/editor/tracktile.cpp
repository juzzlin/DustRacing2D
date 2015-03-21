// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#include "tracktile.hpp"
#include "trackdata.hpp"
#include "tileanimator.hpp"
#include "mainwindow.hpp"

#include "../common/config.hpp"

#include <QAction>
#include <QGraphicsLineItem>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPainter>

TrackTile * TrackTile::m_activeTile = nullptr;

TrackTile::TrackTile(
    TrackData & trackData, QPointF location, QPoint matrixLocation, const QString & type)
: TrackTileBase(trackData, location, matrixLocation, type)
, m_size(QSizeF(TILE_W, TILE_H))
, m_active(false)
, m_animator(new TileAnimator(this))
, m_added(false)
{
    setPos(location);
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
        painter->drawPixmap(boundingRect().x(), boundingRect().y(),
            boundingRect().width(), boundingRect().height(),
            m_pixmap);

        // Mark the tile if it has computer hints set
        if (computerHint() == TrackTile::CH_BRAKE_HARD)
        {
            painter->fillRect(boundingRect(), QBrush(QColor(255, 0, 0, 128)));
        }
        else if (computerHint() == TrackTile::CH_BRAKE)
        {
            painter->fillRect(boundingRect(), QBrush(QColor(128, 0, 0, 128)));
        }
    }
    else
    {
        painter->drawPixmap(boundingRect().x(), boundingRect().y(),
            boundingRect().width(), boundingRect().height(),
            QPixmap(Config::Editor::CLEAR_ICON_PATH));

        pen.setColor(QColor(0, 0, 0));
        painter->setPen(pen);
        painter->drawRect(boundingRect());
    }

    // Render highlight
    if (m_active)
    {
        painter->fillRect(boundingRect(), QBrush(QColor(0, 0, 0, 64)));
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

        TrackTile::m_activeTile = nullptr;
    }
}

TrackTile * TrackTile::activeTile()
{
    return TrackTile::m_activeTile;
}

bool TrackTile::rotate90CW(qreal * newRotation)
{
    return m_animator->rotate90CW(newRotation);
}

bool TrackTile::rotate90CCW(qreal * newRotation)
{
    return m_animator->rotate90CCW(newRotation);
}

void TrackTile::setTileType(const QString & type)
{
    TrackTileBase::setTileType(type);
    update();
}

void TrackTile::setComputerHint(ComputerHint hint)
{
    TrackTileBase::setComputerHint(hint);
    update();
}

void TrackTile::setLocation(QPointF location)
{
    TrackTileBase::setLocation(location);

    setPos(location);
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

void TrackTile::swap(TrackTile & other)
{
    // Swap tile types
    const QString sourceType = tileType();
    setTileType(other.tileType());
    other.setTileType(sourceType);

    // Swap tile pixmaps
    const QPixmap sourcePixmap = pixmap();
    setPixmap(other.pixmap());
    other.setPixmap(sourcePixmap);

    // Swap tile rotations
    const int sourceAngle = rotation();
    setRotation(other.rotation());
    other.setRotation(sourceAngle);

    // Swap computer hints
    const TrackTileBase::ComputerHint sourceHint = computerHint();
    setComputerHint(other.computerHint());
    other.setComputerHint(sourceHint);
}

void TrackTile::setAdded(bool state)
{
    m_added = state;
}

bool TrackTile::added() const
{
    return m_added;
}

TrackTile::~TrackTile()
{
    delete m_animator;
}
