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

TrackTile::TrackTile(QSizeF size, QPointF location, TileType type):
    m_size(size),
    m_active(false)
{
    setPos(location);
}

QRectF TrackTile::boundingRect () const
{
    return QRectF(QPointF(-m_size.width() / 2, -m_size.height() / 2), m_size);
}

void TrackTile::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);

    painter->drawRect(-m_size.width() / 2, -m_size.height() / 2,
                      m_size.width(), m_size.height());
}

void TrackTile::setActive(bool active)
{
    m_active = active;
}

