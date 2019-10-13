// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "targetnode.hpp"
#include "tracktile.hpp"

#include <QGraphicsLineItem>
#include <QPainter>
#include <QPen>

#include <cassert>
#include <memory>

static const int LINE_WIDTH = 15;
static const QColor ROUTE_LINE_COLOR(0, 0, 255, 128);
static const QColor FIRST_NODE_COLOR(255, 255, 255, 128);
static const QColor NODE_COLOR(ROUTE_LINE_COLOR);

TargetNode::TargetNode()
  : m_routeLine(nullptr)
{
}

TargetNode::TargetNode(const TargetNode & other)
  : QGraphicsItem()
  , TargetNodeBase()
  , m_routeLine(nullptr)
{
    setIndex(other.index());

    setLocation(other.location());

    setSize(other.size());
}

void TargetNode::setRouteLine(QGraphicsLineItem * routeLine)
{
    m_routeLine = routeLine;
    m_routeLine->setPen(
      QPen(QBrush(ROUTE_LINE_COLOR), LINE_WIDTH, Qt::DashDotDotLine, Qt::RoundCap));
}

QGraphicsLineItem * TargetNode::routeLine() const
{
    return m_routeLine;
}

QRectF TargetNode::boundingRect() const
{
    return QRectF(
      -size().width() / 2 - LINE_WIDTH, -size().height() / 2 - LINE_WIDTH,
      size().width() + LINE_WIDTH, size().height() + LINE_WIDTH);
}

void TargetNode::paint(QPainter * painter,
                       const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->save();

    if (index() == 0)
    {
        painter->setPen(QPen(QBrush(FIRST_NODE_COLOR), LINE_WIDTH));
    }
    else
    {
        painter->setPen(QPen(QBrush(NODE_COLOR), LINE_WIDTH));
    }

    // Outline
    painter->drawRect(
      boundingRect().x() + LINE_WIDTH, boundingRect().y() + LINE_WIDTH,
      boundingRect().width() - LINE_WIDTH * 2, boundingRect().height() - LINE_WIDTH * 2);

    // Circle in the middle
    painter->drawEllipse(
      QPointF(
        boundingRect().x() + boundingRect().width() / 2 + LINE_WIDTH / 2,
        boundingRect().y() + boundingRect().height() / 2 + LINE_WIDTH / 2),
      LINE_WIDTH,
      LINE_WIDTH);

    painter->restore();
}

void TargetNode::setLocation(QPointF newLocation)
{
    TargetNodeBase::setLocation(newLocation);
    setPos(newLocation);

    updateRouteLine();

    if (auto prevPtr = std::dynamic_pointer_cast<TargetNode>(prev()))
    {
        prevPtr->updateRouteLine();
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
