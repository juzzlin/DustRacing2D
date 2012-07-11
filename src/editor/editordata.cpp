// This file is part of Dust Racing (DustRAC).
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

#include "editordata.hpp"
#include "editorscene.hpp"
#include "editorview.hpp"
#include "mainwindow.hpp"
#include "object.hpp"
#include "targetnode.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"
#include "trackio.hpp"

#include <cassert>

EditorData::EditorData(MainWindow * mainWindow)
: m_mode(EM_NONE)
, m_dragAndDropSourceTile(nullptr)
, m_dragAndDropObject(nullptr)
, m_dragAndDropTargetNode(nullptr)
, m_dragAndDropSourcePos()
, m_mainWindow(mainWindow)
{}

bool EditorData::loadTrackData(QString fileName)
{
    removeTilesFromScene();
    removeObjectsFromScene();

    m_trackData.reset(TrackIO::open(fileName));
    return static_cast<bool>(m_trackData.get());
}

bool EditorData::saveTrackData()
{
    assert(m_trackData);
    return TrackIO::save(m_trackData.get(), m_trackData->fileName());
}

bool EditorData::saveTrackDataAs(QString fileName)
{
    assert(m_trackData);
    if (TrackIO::save(m_trackData.get(), fileName))
    {
        m_trackData->setFileName(fileName);
        return true;
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
    assert(m_trackData);
    setMode(EditorData::EM_SET_ROUTE);
    removeRouteFromScene();
    m_trackData->route().clear();
}

void EditorData::endSetRoute()
{
    // Reset the editing mode.
    setMode(EditorData::EM_NONE);
}

void EditorData::addExistingRouteToScene()
{
    // Re-push existing (loaded) target nodes so that route lines
    // and links will be correctly created. A bit stupid,
    // but is enough for now, because the vectors are not long.
    std::vector<TargetNodeBase *> temp;
    m_trackData->route().getAll(temp);
    m_trackData->route().clear();
    for (TargetNodeBase * tnode : temp)
    {
        pushTargetNodeToRoute(*tnode);
    }
}

void EditorData::pushNewTargetNodeToRoute(QPointF pos)
{
    // Push location to the route
    TargetNodeBase * tnode = new TargetNode;
    tnode->setLocation(pos);
    pushTargetNodeToRoute(*tnode);
}

void EditorData::pushTargetNodeToRoute(TargetNodeBase & tnode)
{
    Route & route = trackData()->route();

    if (route.numNodes())
    {
        TargetNode & prev = static_cast<TargetNode &>(route.get(route.numNodes() - 1));
        prev.setNext(&tnode);
        tnode.setPrev(&prev);
        prev.updateRouteLine();
    }

    const bool loopClosed = route.push(tnode);

    QGraphicsLineItem * routeLine = new QGraphicsLineItem;
    static_cast<TargetNode &>(tnode).setRouteLine(routeLine);

    m_mainWindow->editorScene().addItem(&static_cast<TargetNode &>(tnode));
    m_mainWindow->editorScene().addItem(routeLine);
    static_cast<TargetNode &>(tnode).setZValue(10);
    routeLine->setZValue(10);

    // Check if we might have a loop => end
    if (loopClosed)
    {
        setMode(EditorData::EM_NONE);
        m_mainWindow->endSetRoute();

        static_cast<TargetNode &>(route.get(route.numNodes() - 1)).setLocation(
            static_cast<TargetNode &>(route.get(0)).location());

        tnode.setNext(&route.get(0));
        route.get(0).setPrev(&tnode);
    }
}

void EditorData::removeRouteFromScene()
{
    for (unsigned int i = 0; i < m_trackData->route().numNodes(); i++)
    {
        TargetNodeBase * tnode = &m_trackData->route().get(i);
        m_mainWindow->editorScene().removeItem(static_cast<TargetNode *>(tnode));
        m_mainWindow->editorScene().removeItem(static_cast<TargetNode *>(tnode)->routeLine());
        delete static_cast<TargetNode *>(tnode)->routeLine();
        delete tnode;
    }

    m_mainWindow->editorView().update();
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

void EditorData::setDragAndDropObject(Object * object)
{
    m_dragAndDropObject = object;
}

Object * EditorData::dragAndDropObject() const
{
    return m_dragAndDropObject;
}

void EditorData::setDragAndDropTargetNode(TargetNode * tnode)
{
    m_dragAndDropTargetNode = tnode;
}

TargetNode * EditorData::dragAndDropTargetNode() const
{
    return m_dragAndDropTargetNode;
}

void EditorData::setDragAndDropSourcePos(QPointF pos)
{
    m_dragAndDropSourcePos = pos;
}

QPointF EditorData::dragAndDropSourcePos() const
{
    return m_dragAndDropSourcePos;
}

void EditorData::addTilesToScene()
{
    assert(m_trackData);

    const unsigned int cols = m_trackData->map().cols();
    const unsigned int rows = m_trackData->map().rows();

    for (unsigned int i = 0; i < cols; i++)
    {
        for (unsigned int j = 0; j < rows; j++)
        {
            if (TrackTile * tile = static_cast<TrackTile *>(m_trackData->map().getTile(i, j)))
            {
                if (!tile->added())
                {
                    m_mainWindow->editorScene().addItem(tile);
                    tile->setAdded(true);
                }
            }
        }
    }

    if (m_trackData->map().getTile(0, 0))
        static_cast<TrackTile *>(m_trackData->map().getTile(0, 0))->setActive(true);
}

void EditorData::addObjectsToScene()
{
    assert(m_trackData);

    for (unsigned int i = 0; i < m_trackData->objects().count(); i++)
    {
        if (Object * object = dynamic_cast<Object *>(&m_trackData->objects().object(i)))
        {
            m_mainWindow->editorScene().addItem(object);
            object->setZValue(10);
        }
    }
}

void EditorData::removeTilesFromScene()
{
    if (m_trackData)
    {
        TrackTile::setActiveTile(nullptr);

        const unsigned int cols = m_trackData->map().cols();
        const unsigned int rows = m_trackData->map().rows();

        for (unsigned int i = 0; i < cols; i++)
        {
            for (unsigned int j = 0; j < rows; j++)
            {
                if (TrackTile * tile = static_cast<TrackTile *>(m_trackData->map().getTile(i, j)))
                {
                    m_mainWindow->editorScene().removeItem(tile);
                    delete tile;
                }
            }
        }
    }
}

void EditorData::removeObjectsFromScene()
{
    if (m_trackData)
    {
        for (unsigned int i = 0; i < m_trackData->objects().count(); i++)
        {
            if (Object * object =
                    dynamic_cast<Object *>(&m_trackData->objects().object(i)))
            {
                m_mainWindow->editorScene().removeItem(object);
                delete object;
            }
        }
    }
}

void EditorData::clear()
{
    assert(m_trackData);

    const unsigned int cols = m_trackData->map().cols();
    const unsigned int rows = m_trackData->map().rows();

    for (unsigned int i = 0; i < cols; i++)
    {
        for (unsigned int j = 0; j < rows; j++)
        {
            if (TrackTile * pTile = static_cast<TrackTile *>(m_trackData->map().getTile(i, j)))
            {
                pTile->setTileType("clear");
            }
        }
    }

    m_mainWindow->console(QString(QObject::tr("Tiles cleared.")));

    clearRoute();
}

void EditorData::clearRoute()
{
    assert(m_trackData);

    removeRouteFromScene();
    m_trackData->route().clear();

    m_mainWindow->console(QString(QObject::tr("Route cleared.")));
}
