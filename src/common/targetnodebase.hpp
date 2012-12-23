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

#ifndef TARGETNODEBASE_HPP
#define TARGETNODEBASE_HPP

#include <QSizeF>
#include <QString>
#include <QPointF>

//! Base class for freely placeable target nodes in the editor.
class TargetNodeBase
{
public:

    //! Constructor.
    TargetNodeBase();

    //! Destructor.
    virtual ~TargetNodeBase() {};

    //! Get location in world / scene.
    virtual QPointF location() const;

    //! Set coordinates in the world.
    virtual void setLocation(QPointF newLocation);

    virtual int index() const;

    virtual void setIndex(int index);

    virtual void setNext(TargetNodeBase * tnode);

    virtual TargetNodeBase * next() const;

    virtual void setPrev(TargetNodeBase * tnode);

    virtual TargetNodeBase * prev() const;

    virtual void setSize(QSizeF size);

    virtual QSizeF size() const;

private:

    //! Coordinates in the world.
    QPointF m_location;

    //! Original size in pixels.
    QSizeF m_size;

    //! Index in the route.
    int m_index;

    TargetNodeBase * m_next;

    TargetNodeBase * m_prev;
};

#endif // TargetNodeBase_HPP
