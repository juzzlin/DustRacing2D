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
#include "editorscene.h"
#include "mainwindow.h"
#include "tracktile.h"
#include "trackio.h"
#include "../common/trackdata.h"

EditorData::EditorData(MainWindow * mainWindow)
: m_mode(EM_NONE)
, m_dragAndDropSourceTile(NULL)
, m_dragAndDropSourcePos()
, m_mainWindow(mainWindow)
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
    {
        removeRouteLinesFromScene();
        m_trackData->route().clear();
    }
}

void EditorData::endSetRoute()
{
    // Reset the editing mode.
    setMode(EditorData::EM_NONE);
}

void EditorData::addRouteLinesToScene(bool closeLoop)
{
    // Add route lines to the scene.
    // Each tile contains a pointer to the corresponding line object so that
    // it can be removed later.
    if (m_trackData)
    {
        const unsigned int length = m_trackData->route().length();
        if (length > 1)
        {
            QPen pen(QBrush(QColor(0, 0, 255, 64)), 7, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);

            for (unsigned int i = 1; i < length; i++)
            {
                TrackTile * tile0 = m_trackData->route().get(i - 1);
                TrackTile * tile1 = m_trackData->route().get(i);

                if (!tile1->routeLine())
                {
                    tile1->setRouteLine(m_mainWindow->editorScene()->addLine(tile0->x(), tile0->y(),
                                                                             tile1->x(), tile1->y(), pen));
                }
            }

            if (closeLoop)
            {
                TrackTile * tile0 = m_trackData->route().get(length - 1);
                TrackTile * tile1 = m_trackData->route().get(0);

                if (!tile1->routeLine())
                {
                    tile1->setRouteLine(m_mainWindow->editorScene()->addLine(tile0->x(), tile0->y(),
                                                                             tile1->x(), tile1->y(), pen));
                }
            }
        }
    }
}

void EditorData::removeRouteLinesFromScene()
{
    if (m_trackData)
    {
        const unsigned int length = m_trackData->route().length();
        for (unsigned int i = 0; i < length; i++)
        {
            TrackTile * tile = m_trackData->route().get(i);
            if (tile->routeLine())
            {
                QGraphicsLineItem * line = tile->routeLine();
                m_mainWindow->editorScene()->removeItem(line);
                delete line;
                tile->setRouteLine(NULL);
                tile->update();
            }
        }
    }
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
