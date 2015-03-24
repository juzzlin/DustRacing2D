// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#include "editordata.hpp"
#include "editorscene.hpp"
#include "editorview.hpp"
#include "mainwindow.hpp"
#include "object.hpp"
#include "targetnode.hpp"
#include "tracktile.hpp"
#include "trackio.hpp"

#include <cassert>

EditorData::EditorData(MainWindow * mainWindow)
: m_mode(EM_NONE)
, m_dragAndDropSourceTile(nullptr)
, m_dragAndDropObject(nullptr)
, m_selectedObject(nullptr)
, m_selectedTargetNode(nullptr)
, m_dragAndDropTargetNode(nullptr)
, m_dragAndDropSourcePos()
, m_mainWindow(mainWindow)
, m_activeColumn(0)
, m_activeRow(0)
{}

void EditorData::clearScene()
{
    removeTilesFromScene();
    removeObjectsFromScene();
    removeTargetNodesFromScene();
}

bool EditorData::loadTrackData(QString fileName)
{
    clearScene();

    m_trackData = TrackIO::open(fileName);
    return static_cast<bool>(m_trackData);
}

bool EditorData::saveTrackData()
{
    assert(m_trackData);
    return TrackIO::save(m_trackData, m_trackData->fileName());
}

bool EditorData::saveTrackDataAs(QString fileName)
{
    assert(m_trackData);
    if (TrackIO::save(m_trackData, fileName))
    {
        m_trackData->setFileName(fileName);
        return true;
    }
    return false;
}

void EditorData::setTrackData(TrackDataPtr trackData)
{
    clearScene();

    m_trackData = trackData;
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
    std::vector<TargetNodePtr> temp;
    m_trackData->route().getAll(temp);
    m_trackData->route().clear();
    for (TargetNodePtr tnode : temp)
    {
        pushTargetNodeToRoute(tnode);
    }
}

void EditorData::pushNewTargetNodeToRoute(QPointF pos)
{
    // Push location to the route
    TargetNodePtr tnode(new TargetNode);
    tnode->setLocation(pos);
    pushTargetNodeToRoute(tnode);
}

void EditorData::pushTargetNodeToRoute(TargetNodePtr tnode)
{
    Route & route = trackData()->route();

    if (route.numNodes())
    {
        TargetNodePtr prev = route.get(route.numNodes() - 1);

        prev->setNext(tnode);
        tnode->setPrev(prev);

        dynamic_cast<TargetNode *>(prev.get())->updateRouteLine();
    }

    const bool loopClosed = route.push(tnode);

    QGraphicsLineItem * routeLine = new QGraphicsLineItem;
    TargetNode * ptr = dynamic_cast<TargetNode *>(tnode.get());
    ptr->setRouteLine(routeLine);

    m_mainWindow->editorScene().addItem(ptr);
    m_mainWindow->editorScene().addItem(routeLine);

    const int routeLineZ = 10;
    ptr->setZValue(routeLineZ);
    routeLine->setZValue(routeLineZ);

    // Check if we might have a loop => end
    if (loopClosed)
    {
        setMode(EditorData::EM_NONE);
        m_mainWindow->endSetRoute();

        TargetNodePtr firstNode = route.get(0);
        route.get(route.numNodes() - 1)->setLocation(firstNode->location());

        tnode->setNext(firstNode);
        firstNode->setPrev(tnode);
    }
}

void EditorData::removeRouteFromScene()
{
    for (auto tnodePtr : m_trackData->route())
    {
        TargetNode * tnode = dynamic_cast<TargetNode *>(tnodePtr.get());
        assert(tnode);

        m_mainWindow->editorScene().removeItem(tnode);
        m_mainWindow->editorScene().removeItem(tnode->routeLine());

        delete tnode->routeLine();
    }

    m_mainWindow->editorView().update();
}

TrackDataPtr EditorData::trackData()
{
    return m_trackData;
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

void EditorData::setSelectedObject(Object * object)
{
    m_selectedObject = object;
}

Object * EditorData::selectedObject() const
{
    return m_selectedObject;
}

void EditorData::setSelectedTargetNode(TargetNode * tnode)
{
    m_selectedTargetNode = tnode;
}

TargetNode * EditorData::selectedTargetNode() const
{
    return m_selectedTargetNode;
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
            TrackTile * tile = dynamic_cast<TrackTile *>(m_trackData->map().getTile(i, j).get());
            assert(tile);

            if (!tile->added())
            {
                m_mainWindow->editorScene().addItem(tile);
                tile->setAdded(true);
            }
        }
    }

    TrackTile * tile = dynamic_cast<TrackTile *>(m_trackData->map().getTile(0, 0).get());
    assert(tile);

    tile->setActive(true);
}

void EditorData::addObjectsToScene()
{
    assert(m_trackData);

    for (unsigned int i = 0; i < m_trackData->objects().count(); i++)
    {
        Object * object = dynamic_cast<Object *>(m_trackData->objects().object(i).get());
        assert(object);

        m_mainWindow->editorScene().addItem(object);
        object->setZValue(10);
    }
}

void EditorData::removeTileFromScene(TrackTilePtr trackTile)
{
    TrackTile::setActiveTile(nullptr);

    TrackTile * tile = dynamic_cast<TrackTile *>(trackTile.get());
    assert(tile);

    m_mainWindow->editorScene().removeItem(tile);
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
                TrackTile * tile = dynamic_cast<TrackTile *>(m_trackData->map().getTile(i, j).get());
                assert(tile);

                m_mainWindow->editorScene().removeItem(tile);
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
            Object * object = dynamic_cast<Object *>(m_trackData->objects().object(i).get());
            assert(object);

            m_mainWindow->editorScene().removeItem(object);
        }
    }

    m_selectedObject    = nullptr;
    m_dragAndDropObject = nullptr;
}

void EditorData::removeTargetNodesFromScene()
{
    if (m_trackData)
    {
        for (auto tnodePtr : m_trackData->route())
        {
            TargetNode * tnode = dynamic_cast<TargetNode *>(tnodePtr.get());
            assert(tnode);

            m_mainWindow->editorScene().removeItem(tnode);
        }
    }
}

void EditorData::clearRoute()
{
    assert(m_trackData);

    removeRouteFromScene();
    m_trackData->route().clear();

    m_mainWindow->console(QString(QObject::tr("Route cleared.")));
}

bool EditorData::undo(const ObjectModelLoader & loader)
{
    return m_trackData ? m_trackData->undo(loader) : false;
}

bool EditorData::redo(const ObjectModelLoader & loader)
{
    return m_trackData ? m_trackData->redo(loader) : false;
}

void EditorData::setActiveColumn(unsigned int column)
{
    m_activeColumn = column;
}

unsigned int EditorData::activeColumn() const
{
    return m_activeColumn;
}

void EditorData::setActiveRow(unsigned int row)
{
    m_activeRow = row;
}

unsigned int EditorData::activeRow() const
{
    return m_activeRow;
}
