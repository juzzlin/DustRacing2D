// This file is part of Dust Racing 2D.
// Copyright (C) 2018 Jussi Lind <jussi.lind@iki.fi>
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

#include "draganddropstore.hpp"

DragAndDropStore::DragAndDropStore()
{
}

void DragAndDropStore::clear()
{
    m_dragAndDropSourceTile = nullptr;

    m_dragAndDropObject = nullptr;

    m_dragAndDropTargetNode = nullptr;
}

void DragAndDropStore::setDragAndDropSourceTile(TrackTile * tile)
{
    m_dragAndDropSourceTile = tile;
}

TrackTile * DragAndDropStore::dragAndDropSourceTile() const
{
    return m_dragAndDropSourceTile;
}

void DragAndDropStore::setDragAndDropObject(Object * object)
{
    m_dragAndDropObject = object;
}

Object * DragAndDropStore::dragAndDropObject() const
{
    return m_dragAndDropObject;
}

void DragAndDropStore::setDragAndDropTargetNode(TargetNode * tnode)
{
    m_dragAndDropTargetNode = tnode;
}

TargetNode * DragAndDropStore::dragAndDropTargetNode() const
{
    return m_dragAndDropTargetNode;
}

void DragAndDropStore::setDragAndDropSourcePos(QPointF pos)
{
    m_dragAndDropSourcePos = pos;
}

QPointF DragAndDropStore::dragAndDropSourcePos() const
{
    return m_dragAndDropSourcePos;
}
