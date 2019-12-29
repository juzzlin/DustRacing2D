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

#include "object.hpp"
#include <QPainter>

Object::Object(QString category, QString role, QSizeF size, QPixmap pixmap)
  : ObjectBase(category, role)
  , m_size(size)
  , m_pixmap(pixmap)
{
}

Object::Object(const Object & other)
  : QGraphicsItem()
  , ObjectBase(other.category(), other.role())
  , m_size(other.m_size)
  , m_pixmap(other.m_pixmap)
{
    setLocation(other.location());
    setRotation(other.rotation());
    setForceStationary(other.forceStationary());
}

Object::Object(std::unique_ptr<Object> other)
  : Object(*other)
{
}

QRectF Object::boundingRect() const
{
    return QRectF(-m_size.width() / 2, -m_size.height() / 2, m_size.width(), m_size.height());
}

void Object::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget)
    Q_UNUSED(option)

    painter->save();
    painter->drawPixmap(boundingRect().x(), boundingRect().y(), boundingRect().width(), boundingRect().height(), m_pixmap);
    painter->restore();
}

void Object::setLocation(QPointF newLocation)
{
    ObjectBase::setLocation(newLocation);
    setPos(newLocation);
}
