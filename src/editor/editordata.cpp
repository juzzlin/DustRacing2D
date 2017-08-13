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
#include <memory>

using std::dynamic_pointer_cast;

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
    std::vector<TargetNodeBasePtr> temp;
    m_trackData->route().getAll(temp);
    m_trackData->route().clear();
    for (auto tnode : temp)
    {
        pushTargetNodeToRoute(tnode);
    }
}

void EditorData::pushNewTargetNodeToRoute(QPointF pos)
{
    // Push location to the route
    TargetNodeBasePtr tnode(new TargetNode);
    tnode->setLocation(pos);
    pushTargetNodeToRoute(tnode);
}

void EditorData::pushTargetNodeToRoute(TargetNodeBasePtr tnode)
{
    Route & route = trackData()->route();

    if (route.numNodes())
    {
        auto prev = route.get(route.numNodes() - 1);

        prev->setNext(tnode);
        tnode->setPrev(prev);

        dynamic_pointer_cast<TargetNode>(prev)->updateRouteLine();
    }

    const bool loopClosed = route.push(tnode);

    auto node = dynamic_pointer_cast<TargetNode>(tnode);
    auto routeLine = new QGraphicsLineItem;
    node->setRouteLine(routeLine);

    m_mainWindow->editorScene().addItem(node.get()); // The scene wants a raw pointer
    m_mainWindow->editorScene().addItem(routeLine);

    const int routeLineZ = 10;
    node->setZValue(routeLineZ);
    routeLine->setZValue(routeLineZ);

    // Check if we might have a loop => end
    if (loopClosed)
    {
        setMode(EditorData::EM_NONE);
        m_mainWindow->endSetRoute();

        auto firstNode = route.get(0);
        route.get(route.numNodes() - 1)->setLocation(firstNode->location());

        tnode->setNext(firstNode);
        firstNode->setPrev(tnode);
    }
}

void EditorData::removeRouteFromScene()
{
    for (auto && tnode : m_trackData->route())
    {
        auto node = dynamic_pointer_cast<TargetNode>(tnode);
        assert(node);

        m_mainWindow->editorScene().removeItem(node.get()); // The scene wants a raw pointer
        m_mainWindow->editorScene().removeItem(node->routeLine());

        delete node->routeLine();
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
            auto tile = dynamic_pointer_cast<TrackTile>(m_trackData->map().getTile(i, j));
            assert(tile);

            if (!tile->added())
            {
                m_mainWindow->editorScene().addItem(tile.get()); // The scene wants a raw pointer

                tile->setAdded(true);
            }
        }
    }

    auto tile = dynamic_pointer_cast<TrackTile>(m_trackData->map().getTile(0, 0));
    assert(tile);

    tile->setActive(true);
}

void EditorData::addObjectsToScene()
{
    assert(m_trackData);

    for (unsigned int i = 0; i < m_trackData->objects().count(); i++)
    {
        auto object = dynamic_pointer_cast<Object>(m_trackData->objects().object(i));
        assert(object);

        m_mainWindow->editorScene().addItem(object.get()); // The scene wants a raw pointer

        object->setZValue(10);
    }
}

void EditorData::removeTileFromScene(TrackTileBasePtr trackTile)
{
    TrackTile::setActiveTile(nullptr);

    auto tile = dynamic_pointer_cast<TrackTile>(trackTile);
    assert(tile);

    m_mainWindow->editorScene().removeItem(tile.get()); // The scene wants a raw pointer
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
                auto tile = dynamic_pointer_cast<TrackTile>(m_trackData->map().getTile(i, j));
                assert(tile);

                m_mainWindow->editorScene().removeItem(tile.get()); // The scene wants a raw pointer
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
            auto object = dynamic_pointer_cast<Object>(m_trackData->objects().object(i));
            assert(object);

            m_mainWindow->editorScene().removeItem(object.get()); // The scene wants a raw pointer
        }
    }

    m_selectedObject = nullptr;
    m_dragAndDropObject = nullptr;
}

void EditorData::removeTargetNodesFromScene()
{
    if (m_trackData)
    {
        for (auto tnode : m_trackData->route())
        {
            auto node = dynamic_pointer_cast<TargetNode>(tnode);
            assert(node);

            m_mainWindow->editorScene().removeItem(node.get()); // The scene wants a raw pointer
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
