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

#include "mediator.hpp"

#include "draganddropstore.hpp"
#include "editordata.hpp"
#include "editorview.hpp"
#include "floodfill.hpp"
#include "mainwindow.hpp"
#include "newtrackdialog.hpp"
#include "object.hpp"
#include "objectfactory.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"
#include "trackpropertiesdialog.hpp"

#include <cassert>

#include <QApplication>
#include <QGraphicsScene>
#include <QLayout>
#include <QSizePolicy>
#include <QStatusBar>

Mediator::Mediator(MainWindow & mainWindow)
    : m_editorData(new EditorData(*this))
    , m_editorScene(new QGraphicsScene)
    , m_editorView(new EditorView(*this))
    , m_mainWindow(mainWindow)
{
    m_editorView->setParent(&mainWindow);
}

void Mediator::addCurrentToolBarObjectToScene(QPointF clickedScenePos)
{
    if (QAction * action = MainWindow::instance()->currentToolBarAction())
    {
        saveUndoPoint();

        Object & object = ObjectFactory::createObject(action->data().toString());
        object.setLocation(clickedScenePos);

        if (MainWindow::instance()->randomlyRotateObjects())
        {
            object.setRotation(std::rand() % 360);
        }

        addObject(object);
        setSelectedObject(&object);
    }
}

void Mediator::addItem(QGraphicsItem * item)
{
    m_editorScene->addItem(item);
}

void Mediator::addObject(Object & object)
{
    m_editorScene->addItem(&object);
    m_editorData->trackData()->objects().add(ObjectBasePtr(&object));
}

void Mediator::addViewToLayout(QLayout * layout)
{
    layout->addWidget(m_editorView);
}

bool Mediator::beginSetRoute()
{
    QApplication::restoreOverrideCursor();

    assert(m_editorData);

    m_editorData->saveUndoPoint();

    if (m_editorData->canRouteBeSet())
    {
        m_editorData->beginSetRoute();

        return true;
    }

    return false;
}

void Mediator::console(QString message)
{
    m_mainWindow.console(message);
}

QString Mediator::currentFileName() const
{
    return m_editorData->trackData()->fileName();
}

QAction * Mediator::currentToolBarAction() const
{
    return m_mainWindow.currentToolBarAction();
}

DragAndDropStore & Mediator::dadStore()
{
    return m_editorData->dadStore();
}

void Mediator::deleteColumn()
{
    auto deleted = m_editorData->trackData()->deleteColumn(m_editorData->activeColumn());
    for (auto && tile : deleted) {
        m_editorData->removeTileFromScene(tile);
    }
}

void Mediator::deleteRow()
{
    auto deleted = m_editorData->trackData()->deleteRow(m_editorData->activeRow());
    for (auto && tile : deleted) {
        m_editorData->removeTileFromScene(tile);
    }
}

void Mediator::floodFill(TrackTile & tile, QAction * action, const QString & typeToFill)
{
    saveUndoPoint();

    FloodFill::floodFill(tile, action, typeToFill, m_editorData->trackData()->map());
}

void Mediator::endSetRoute()
{
    setMode(EditorMode::None);
}

void Mediator::enableUndo(bool enable)
{
    m_mainWindow.enableUndo(enable);
}

int Mediator::fitScale()
{
    m_editorView->centerOn(m_editorView->sceneRect().center());
    return m_editorView->viewport()->height() * 100 / m_editorView->sceneRect().height();
}

bool Mediator::initializeNewTrack(QString & name, int & cols, int & rows)
{
    assert(m_editorData);

    // Show a dialog asking some questions about the track
    NewTrackDialog dialog(&m_mainWindow);
    if (dialog.exec() == QDialog::Accepted)
    {
        cols = dialog.cols();
        rows = dialog.rows();
        name = dialog.name();

        m_editorData->setTrackData(TrackDataPtr(new TrackData(name, dialog.isUserTrack(), cols, rows)));

        delete m_editorScene;
        m_editorScene = new QGraphicsScene;

        m_editorView->setScene(m_editorScene);
        m_editorView->updateSceneRect();

        m_editorData->addTilesToScene();
        m_editorData->addObjectsToScene();

        return true;
    }

    return false;
}

void Mediator::insertColumnAfter()
{
    m_editorData->trackData()->insertColumn(m_editorData->activeColumn(), MapBase::InsertDirection::After);
    m_editorData->addTilesToScene();
}

void Mediator::insertColumnBefore()
{
    m_editorData->trackData()->insertColumn(m_editorData->activeColumn(), MapBase::InsertDirection::Before);
    m_editorData->addTilesToScene();
}

void Mediator::insertRowAfter()
{
    m_editorData->trackData()->insertRow(m_editorData->activeRow(), MapBase::InsertDirection::After);
    m_editorData->addTilesToScene();
}

void Mediator::insertRowBefore()
{
    m_editorData->trackData()->insertRow(m_editorData->activeRow(), MapBase::InsertDirection::Before);
    m_editorData->addTilesToScene();
}

void Mediator::initScene()
{
    m_editorView->setScene(m_editorScene);
    m_editorView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_editorView->setMouseTracking(true);
}

bool Mediator::isUndoable() const
{
    return m_editorData->isUndoable();
}

void Mediator::clearEditMode()
{
    QApplication::restoreOverrideCursor();
    setMode(EditorMode::None);
}

void Mediator::clearRoute()
{
    assert(m_editorData);

    m_editorData->saveUndoPoint();
    m_editorData->clearRoute();
}

int Mediator::cols()
{
    if (m_editorData->trackData())
    {
        return m_editorData->trackData()->map().cols();
    }
    return 0;
}

bool Mediator::openTrack(QString fileName)
{
    assert(m_editorData);

    if (m_editorData->loadTrackData(fileName))
    {
        delete m_editorScene;
        m_editorScene = new QGraphicsScene;

        m_editorView->setScene(m_editorScene);
        m_editorView->updateSceneRect();

        m_editorData->addTilesToScene();
        m_editorData->addObjectsToScene();
        m_editorData->addExistingRouteToScene();

        return true;
    }

    return false;
}

bool Mediator::isRedoable() const
{
    return m_editorData->isRedoable();
}

void Mediator::mouseWheelZoom(int delta)
{
    const int sensitivity = 10;
    const int currentScale = m_editorData->currentScale();
    const int maxScale = 200;

    int newScale = currentScale;

    if (delta < 0)
    {
        if (currentScale > sensitivity)
        {
            newScale = currentScale - sensitivity;
        }
        else
        {
            newScale = 0;
        }
    }
    else if (delta > 0)
    {
        if (currentScale + sensitivity < maxScale)
        {
            newScale = currentScale + sensitivity;
        }
        else
        {
            newScale = maxScale;
        }
    }

    if (newScale != currentScale)
    {
        setScale(newScale);
        m_mainWindow.updateScaleSlider(newScale);
    }
}

void Mediator::pushNewTargetNodeToRoute(QPointF pos)
{
    m_editorData->pushNewTargetNodeToRoute(pos);
}

void Mediator::redo()
{
    m_editorData->redo();
}

void Mediator::removeItem(QGraphicsItem * item)
{
    m_editorScene->removeItem(item);
}

void Mediator::removeObject(Object & object)
{
    m_editorScene->removeItem(&object);
    m_editorData->trackData()->objects().remove(object);
}

bool Mediator::routeHasNodes() const
{
    return m_editorData->trackData()->route().numNodes();
}

void Mediator::saveUndoPoint()
{
    m_editorData->saveUndoPoint();
}

bool Mediator::saveTrackData()
{
    return m_editorData->saveTrackData();
}

bool Mediator::saveTrackDataAs(QString fileName)
{
    return m_editorData->saveTrackDataAs(fileName);
}

void Mediator::setMode(EditorMode mode)
{
    m_mode = mode;
}

void Mediator::setSelectedObject(Object * object)
{
    m_editorData->setSelectedObject(object);
}

Object * Mediator::selectedObject() const
{
    return m_editorData->selectedObject();
}

void Mediator::setSelectedTargetNode(TargetNode * tnode)
{
    m_editorData->setSelectedTargetNode(tnode);
}

TargetNode * Mediator::selectedTargetNode() const
{
    return m_editorData->selectedTargetNode();
}

void Mediator::setScale(int value)
{
    const qreal scale = static_cast<qreal>(value) / 100;
    QTransform transform;
    transform.scale(scale, scale);
    m_editorView->setTransform(transform);
    m_editorData->setCurrentScale(value);
}

EditorMode Mediator::mode() const
{
    return m_mode;
}

int Mediator::rows()
{
    if (m_editorData->trackData())
    {
        return m_editorData->trackData()->map().rows();
    }
    return 0;
}

bool Mediator::setTrackProperties()
{
    assert(m_editorData);

    // Show a dialog to set some properties e.g. lap count.
    TrackPropertiesDialog dialog(
        m_editorData->trackData()->name(), m_editorData->trackData()->index(), m_editorData->trackData()->isUserTrack(), &m_mainWindow);
    if (dialog.exec() == QDialog::Accepted)
    {
        m_editorData->trackData()->setName(dialog.name());
        m_editorData->trackData()->setIndex(dialog.index());
        m_editorData->trackData()->setUserTrack(dialog.isUserTrack());
        return true;
    }

    return false;
}

void Mediator::showStatusBarMessage(QString message)
{
    m_mainWindow.statusBar()->showMessage(message);
}

void Mediator::setupTrackAfterUndoOrRedo()
{
    delete m_editorScene;
    m_editorScene = new QGraphicsScene;

    m_editorView->setScene(m_editorScene);
    m_editorView->updateSceneRect();

    m_editorData->addTilesToScene();
    m_editorData->addObjectsToScene();
    m_editorData->addExistingRouteToScene();
}

void Mediator::undo()
{
    m_editorData->undo();
}

void Mediator::updateCoordinates(QPointF mappedPos)
{
    if (m_editorData->trackData())
    {
        const int maxCols = static_cast<int>(m_editorData->trackData()->map().cols());
        int column = mappedPos.x() / TrackTile::TILE_W;
        column = column >= maxCols ? maxCols - 1 : column;
        column = column < 0 ? 0 : column;

        const int maxRows = static_cast<int>(m_editorData->trackData()->map().rows());
        int row = mappedPos.y() / TrackTile::TILE_H;
        row = row >= maxRows ? maxRows - 1 : row;
        row = row < 0 ? 0 : row;

        m_editorData->setActiveRow(static_cast<unsigned int>(row));
        m_editorData->setActiveColumn(static_cast<unsigned int>(column));

        QString coordinates("X: %1 Y: %2 I: %3 J: %4");
        coordinates = coordinates.arg(mappedPos.x()).arg(mappedPos.y()).arg(column).arg(row);

        showStatusBarMessage(coordinates);
    }
}

void Mediator::updateView()
{
    m_editorView->update();
}

Mediator::~Mediator()
{
    delete m_editorData;
}

