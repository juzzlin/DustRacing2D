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

#include "tiletypeundostackitem.hpp"

#include "objectmodelloader.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"

TileTypeUndoStackItem::TileTypeUndoStackItem(const std::vector<QPoint> & positions, const QString & oldType, const QString & newType)
: UndoStackItemBase()
, m_positions(positions)
, m_oldType(oldType)
, m_newType(newType)
{
}

void TileTypeUndoStackItem::executeUndo(TrackData * track, const ObjectModelLoader & loader)
{
    setTiles(track, loader, m_oldType);
}
void TileTypeUndoStackItem::executeRedo(TrackData * track, const ObjectModelLoader & loader)
{
    setTiles(track, loader, m_newType);
}

void TileTypeUndoStackItem::setTiles(TrackData * track, const ObjectModelLoader & loader, QString type)
{
    for (auto iter = m_positions.begin(); iter != m_positions.end(); ++iter)
    {
        TrackTileBase * base = track->map().getTile(iter->x(), iter->y());
        TrackTile * tile = dynamic_cast<TrackTile *>(base);

        tile->setTileType(type);
        tile->setPixmap(loader.getPixmapByRole(type));
    }
}
