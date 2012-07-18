// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "targetnode.hpp"
#include "tracktile.hpp"

#include <QGraphicsLineItem>
#include <QPainter>
#include <QPen>

#include <cassert>

static const int LW = 15;
static const int R  = TrackTile::TILE_W;

TargetNode::TargetNode()
: m_size(QSize(R, R))
, m_routeLine(nullptr)
{
}

void TargetNode::setRouteLine(QGraphicsLineItem * routeLine)
{
    m_routeLine = routeLine;
    m_routeLine->setPen(QPen(QBrush(QColor(0, 0, 255, 64)), LW, Qt::DashDotDotLine, Qt::RoundCap));
}

QGraphicsLineItem * TargetNode::routeLine() const
{
    return m_routeLine;
}

QRectF TargetNode::boundingRect () const
{
    return QRectF(
        -m_size.width() / 2 - LW, -m_size.height() / 2 - LW,
         m_size.width()     + LW,  m_size.height()     + LW);
}

void TargetNode::paint(QPainter * painter,
    const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->save();

    if (index() == 0)
    {
        painter->setPen(QPen(QBrush(QColor(255, 255, 255, 64)), LW));
    }
    else
    {
        painter->setPen(QPen(QBrush(QColor(0, 0, 255, 64)), LW));
    }

    painter->drawRect(
        boundingRect().x()     + LW,     boundingRect().y()      + LW,
        boundingRect().width() - LW * 2, boundingRect().height() - LW * 2);
    painter->drawEllipse(
        QPointF(boundingRect().x() + R / 2 + LW, boundingRect().y() + R / 2 + LW),
        LW, LW);
    painter->restore();
}

void TargetNode::setLocation(QPointF newLocation)
{
    TargetNodeBase::setLocation(newLocation);
    setPos(newLocation);

    updateRouteLine();

    if (TargetNode * pPrev = dynamic_cast<TargetNode *>(prev()))
    {
        pPrev->updateRouteLine();
    }
}

void TargetNode::updateRouteLine()
{
    if (m_routeLine && next())
    {
        m_routeLine->setLine(
            location().x(),
            location().y(),
            next()->location().x(),
            next()->location().y());
    }
}


