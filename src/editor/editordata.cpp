// This file is part of Dust Rallycross (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#include "editordata.h"
#include "../common/trackdata.h"
#include "../common/tracktile.h"
#include "../common/trackio.h"

EditorData::EditorData()
: m_mode(EM_NONE)
, m_dragAndDropSourceTile(NULL)
, m_dragAndDropSourcePos()
{}

bool EditorData::loadTrackData(QString fileName)
{
    m_trackData.reset(TrackIO::open(fileName));
    return static_cast<bool>(m_trackData.get());
}

bool EditorData::saveTrackData()
{
    if (m_trackData)
        return TrackIO::save(m_trackData.get(), m_trackData->fileName());

    return false;
}

bool EditorData::saveTrackDataAs(QString fileName)
{
    if (m_trackData)
    {
        if (TrackIO::save(m_trackData.get(), fileName))
        {
            m_trackData->setFileName(fileName);
            return true;
        }
    }

    return false;
}

void EditorData::setTrackData(TrackData * newTrackData)
{
    m_trackData.reset(newTrackData);
}

bool EditorData::canRouteBeSet() const
{
    return m_trackData && m_trackData->map().cols() > 1 && m_trackData->map().rows() > 1;
}

void EditorData::beginSetRoute()
{
    setMode(EditorData::EM_SETROUTE);

    if (m_trackData)
        m_trackData->route().clear();
}

void EditorData::endSetRoute()
{
    setMode(EditorData::EM_NONE);
}

TrackData * EditorData::trackData()
{
    return m_trackData.get();
}

EditorData::EditorMode EditorData::mode() const
{
    return m_mode;
}

void EditorData::setMode(EditorMode newMode)
{
    m_mode = newMode;
}

void EditorData::setDragAndDropSourceTile(TrackTile * tile)
{
    m_dragAndDropSourceTile = tile;
}

TrackTile * EditorData::dragAndDropSourceTile() const
{
    return m_dragAndDropSourceTile;
}

void EditorData::setDragAndDropSourcePos(QPointF pos)
{
    m_dragAndDropSourcePos = pos;
}

QPointF EditorData::dragAndDropSourcePos() const
{
    return m_dragAndDropSourcePos;
}
