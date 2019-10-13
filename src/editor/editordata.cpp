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
#include "editorview.hpp"
#include "mainwindow.hpp"
#include "mediator.hpp"
#include "object.hpp"
#include "objectmodelloader.hpp"
#include "targetnode.hpp"
#include "trackio.hpp"
#include "tracktile.hpp"

#include <cassert>
#include <memory>

using std::dynamic_pointer_cast;

EditorData::EditorData(Mediator & mediator)
  : m_mediator(mediator)
{
}

void EditorData::clearScene()
{
    removeTilesFromScene();
    removeObjectsFromScene();
    removeTargetNodesFromScene();
}

DragAndDropStore & EditorData::dadStore()
{
    return m_dadStore;
}

bool EditorData::loadTrackData(QString fileName)
{
    m_undoStack.clear();

    clearScene();

    m_trackData = m_trackIO.open(fileName);
    return static_cast<bool>(m_trackData);
}

bool EditorData::isUndoable() const
{
    return m_undoStack.isUndoable();
}

void EditorData::undo()
{
    if (m_undoStack.isUndoable())
    {
        m_dadStore.clear();

        m_selectedObject = nullptr;

        m_selectedTargetNode = nullptr;

        saveRedoPoint();

        clearScene();

        m_trackData = m_undoStack.undo();
    }
}

bool EditorData::isRedoable() const
{
    return m_undoStack.isRedoable();
}

void EditorData::redo()
{
    if (m_undoStack.isRedoable())
    {
        m_dadStore.clear();

        m_selectedObject = nullptr;

        m_selectedTargetNode = nullptr;

        saveUndoPoint();

        clearScene();

        m_trackData = m_undoStack.redo();
    }
}

bool EditorData::saveTrackData()
{
    assert(m_trackData);
    return m_trackIO.save(m_trackData, m_trackData->fileName());
}

void EditorData::saveUndoPoint()
{
    assert(m_trackData);
    m_undoStack.pushUndoPoint(m_trackData);

    m_mediator.enableUndo(m_undoStack.isUndoable());
}

void EditorData::saveRedoPoint()
{
    assert(m_trackData);
    m_undoStack.pushRedoPoint(m_trackData);
}

bool EditorData::saveTrackDataAs(QString fileName)
{
    assert(m_trackData);
    if (m_trackIO.save(m_trackData, fileName))
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
    m_mediator.setMode(EditorMode::SetRoute);
    removeRouteFromScene();
    m_trackData->route().clear();
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

    m_mediator.addItem(node.get()); // The scene wants a raw pointer
    m_mediator.addItem(routeLine);

    const int routeLineZ = 10;
    node->setZValue(routeLineZ);
    routeLine->setZValue(routeLineZ);

    // Check if we might have a loop => end
    if (loopClosed)
    {
        m_mediator.endSetRoute();

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

        m_mediator.removeItem(node.get()); // The scene wants a raw pointer
        m_mediator.removeItem(node->routeLine());

        delete node->routeLine();
    }

    m_mediator.updateView();
}

TrackDataPtr EditorData::trackData()
{
    return m_trackData;
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

void EditorData::addTilesToScene()
{
    assert(m_trackData);

    for (unsigned int i = 0; i < m_trackData->map().cols(); i++)
    {
        for (unsigned int j = 0; j < m_trackData->map().rows(); j++)
        {
            auto tile = dynamic_pointer_cast<TrackTile>(m_trackData->map().getTile(i, j));
            assert(tile);

            tile->setPixmap(MainWindow::instance()->objectModelLoader().getPixmapByRole(tile->tileType()));

            if (!tile->added())
            {
                m_mediator.addItem(tile.get()); // The scene wants a raw pointer

                tile->setAdded(true);
            }
        }
    }

    auto tile = dynamic_pointer_cast<TrackTile>(m_trackData->map().getTile(0, 0));
    assert(tile);
}

void EditorData::addObjectsToScene()
{
    assert(m_trackData);

    for (unsigned int i = 0; i < m_trackData->objects().count(); i++)
    {
        auto object = dynamic_pointer_cast<Object>(m_trackData->objects().object(i));
        assert(object);

        m_mediator.addItem(object.get()); // The scene wants a raw pointer

        object->setZValue(10);
    }
}

void EditorData::removeTileFromScene(TrackTileBasePtr trackTile)
{
    TrackTile::setActiveTile(nullptr);

    auto tile = dynamic_pointer_cast<TrackTile>(trackTile);
    assert(tile);

    m_mediator.removeItem(tile.get()); // The scene wants a raw pointer
}

void EditorData::removeTilesFromScene()
{
    if (m_trackData)
    {
        TrackTile::setActiveTile(nullptr);

        for (unsigned int i = 0; i < m_trackData->map().cols(); i++)
        {
            for (unsigned int j = 0; j < m_trackData->map().rows(); j++)
            {
                auto tile = dynamic_pointer_cast<TrackTile>(m_trackData->map().getTile(i, j));
                assert(tile);

                m_mediator.removeItem(tile.get()); // The scene wants a raw pointer
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

            m_mediator.removeItem(object.get()); // The scene wants a raw pointer
        }
    }

    m_selectedObject = nullptr;

    m_dadStore.clear();
}

void EditorData::removeTargetNodesFromScene()
{
    if (m_trackData)
    {
        for (auto tnode : m_trackData->route())
        {
            auto node = dynamic_pointer_cast<TargetNode>(tnode);
            assert(node);

            m_mediator.removeItem(node.get()); // The scene wants a raw pointer
        }
    }
}

int EditorData::currentScale() const
{
    return m_currentScale;
}

void EditorData::setCurrentScale(int currentScale)
{
    m_currentScale = currentScale;
}

void EditorData::clearRoute()
{
    assert(m_trackData);

    removeRouteFromScene();
    m_trackData->route().clear();

    m_mediator.console(QString(QObject::tr("Route cleared.")));
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
