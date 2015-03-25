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

#include "rotatetileundostackitem.hpp"

#include "trackdata.hpp"
#include "tracktile.hpp"

RotateTileUndoStackItem::RotateTileUndoStackItem(const QPoint & position, qreal oldRotation, qreal newRotation)
: UndoStackItemBase()
, m_position(position)
, m_oldRotation(oldRotation)
, m_newRotation(newRotation)
{
}

void RotateTileUndoStackItem::executeUndo(TrackData * track, const ObjectModelLoader &)
{
    setRotation(track, m_oldRotation);
}

void RotateTileUndoStackItem::executeRedo(TrackData * track, const ObjectModelLoader &)
{
    setRotation(track, m_newRotation);
}

void RotateTileUndoStackItem::setRotation(TrackData * track, qreal rotation)
{
    int x = m_position.x();
    int y = m_position.y();

    if (TrackTile * tile = dynamic_cast<TrackTile *>(track->map().getTile(x, y).get()))
    {
        tile->setRotation(rotation);
    }
}
