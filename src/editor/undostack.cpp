// This file is part of Dust Racing 2D.
// Copyright (C) 2017 Jussi Lind <jussi.lind@iki.fi>
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

#include "undostack.hpp"

UndoStack::UndoStack(unsigned int maxHistorySize)
  : m_maxHistorySize(maxHistorySize)
{
}

void UndoStack::pushUndoPoint(TrackDataPtr trackData)
{
    auto copyData = new TrackData(*trackData.get());
    m_undoStack.push_back(TrackDataPtr(copyData));

    if (m_undoStack.size() > m_maxHistorySize)
    {
        m_undoStack.pop_front();
    }
}

void UndoStack::pushRedoPoint(TrackDataPtr trackData)
{
    auto copyData = new TrackData(*trackData.get());
    m_redoStack.push_back(TrackDataPtr(copyData));

    if (m_redoStack.size() > m_maxHistorySize)
    {
        m_redoStack.pop_front();
    }
}

void UndoStack::clear()
{
    m_undoStack.clear();
    m_redoStack.clear();
}

bool UndoStack::isUndoable() const
{
    return m_undoStack.size() > 0;
}

TrackDataPtr UndoStack::undo()
{
    if (isUndoable())
    {
        auto head = m_undoStack.back();
        m_undoStack.pop_back();
        return head;
    }

    return TrackDataPtr();
}

bool UndoStack::isRedoable() const
{
    return m_redoStack.size() > 0;
}

TrackDataPtr UndoStack::redo()
{
    if (isRedoable())
    {
        auto head = m_redoStack.back();
        m_redoStack.pop_back();
        return head;
    }

    return TrackDataPtr();
}
