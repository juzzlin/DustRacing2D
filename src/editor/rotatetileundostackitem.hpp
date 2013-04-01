// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Markus Rinne <markus.ka.rinne@gmail.com>
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

#ifndef ROTATETILEUNDOSTACKITEM_HPP
#define ROTATETILEUNDOSTACKITEM_HPP

#include "undostackitembase.hpp"

#include <QPoint>

//! An operation that rotates a tile.
class RotateTileUndoStackItem : public UndoStackItemBase
{
public:

    //! Constructor
    RotateTileUndoStackItem(const QPoint & position, qreal oldRotation, qreal newRotation);

    virtual void executeUndo(TrackData * track, const ObjectModelLoader & loader);

    virtual void executeRedo(TrackData * track, const ObjectModelLoader & loader);

private:

    Q_DISABLE_COPY(RotateTileUndoStackItem)

    void setRotation(TrackData * track, qreal rotation);

    QPoint m_position;
    qreal m_oldRotation;
    qreal m_newRotation;
};

#endif
