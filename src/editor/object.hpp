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

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <QGraphicsItem>
#include <QPixmap>

#include "../common/objectbase.hpp"

//! Freely placeable object.
class Object : public QGraphicsItem, public ObjectBase
{
public:
    //! Constructor.
    Object(QString category, QString role, QSizeF size, QPixmap pixmap);

    //! Copy constructor.
    Object(const Object & other);

    Object(std::unique_ptr<Object> other);

    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = nullptr) override;

    //! Sets the Object and QGraphicsItem locations.
    virtual void setLocation(QPointF newLocation) override;

private:
    //! Original size in pixels.
    QSizeF m_size;

    //! Image of the object.
    QPixmap m_pixmap;
};

using ObjectPtr = std::shared_ptr<Object>;

#endif // OBJECT_HPP
