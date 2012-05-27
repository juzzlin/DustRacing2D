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

#include <QApplication>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QStatusBar>
#include <QString>

#include "editordata.hpp"
#include "editorview.hpp"
#include "mainwindow.hpp"
#include "object.hpp"
#include "objectloader.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"

EditorView::EditorView(QWidget * parent)
: QGraphicsView(parent)
, m_clearComputerHint(nullptr)
, m_setComputerHintFirstBeforeCorner(nullptr)
, m_setComputerHintSecondBeforeCorner(nullptr)
, m_clearDrivingLineHint(nullptr)
, m_setDrivingLineHintLeft(nullptr)
, m_setDrivingLineHintRight(nullptr)
, m_setDrivingLineHintTop(nullptr)
, m_setDrivingLineHintBottom(nullptr)
{
    createContextMenu();
}

void EditorView::mouseMoveEvent(QMouseEvent * event)
{
    if (scene())
    {
        const QPointF mappedPos = mapToScene(event->pos());

        if (TrackTile * tile =
            dynamic_cast<TrackTile *>(scene()->itemAt(mappedPos)))
        {
            tile->setActive(true);
        }

        EditorData & editorData = MainWindow::instance()->editorData();

        // Tile drag'n'drop active?
        if (TrackTile * sourceTile = editorData.dragAndDropSourceTile())
        {
            sourceTile->setPos(mappedPos);
        }
        // Object drag'n'drop active?
        else if (Object * object = editorData.dragAndDropObject())
        {
            object->setPos(mappedPos);
        }

        // Show coordinates in status bar
        QString coordinates("X: %1 Y: %2 I: %3 J: %4");
        coordinates = coordinates.arg(
            mappedPos.x()).arg(
            mappedPos.y()).arg(
            static_cast<int>(mappedPos.x() / TrackTile::TILE_W)).arg(
            static_cast<int>(mappedPos.y() / TrackTile::TILE_H));
        MainWindow::instance()->statusBar()->showMessage(coordinates);
    }
}

void EditorView::createContextMenu()
{
    QChar degreeSign(176);

    QString dummy1(QString(QWidget::tr("Rotate 90")) +
                   degreeSign + QWidget::tr(" CW.."));

    QAction * rotate90CW = new QAction(dummy1, &m_menu);
    QObject::connect(rotate90CW, SIGNAL(triggered()), this,
                     SLOT(doRotate90CW()));

    QString dummy2(QString(QWidget::tr("Rotate 90")) +
                   degreeSign + QWidget::tr(" CCW.."));

    QAction * rotate90CCW = new QAction(dummy2, &m_menu);
    QObject::connect(rotate90CCW, SIGNAL(triggered()), this,
                     SLOT(doRotate90CCW()));

    m_clearComputerHint = new QAction(QWidget::tr("Clear computer hint"), &m_menu);
    QObject::connect(m_clearComputerHint, SIGNAL(triggered()), this,
                     SLOT(doClearComputerHint()));

    m_setComputerHintFirstBeforeCorner = new QAction(
        QWidget::tr("Set computer hint 'first before corner'.."), &m_menu);
    QObject::connect(m_setComputerHintFirstBeforeCorner, SIGNAL(triggered()), this,
                     SLOT(doSetComputerHintFirstBeforeCorner()));

    m_setComputerHintSecondBeforeCorner = new QAction(
        QWidget::tr("Set computer hint 'second before corner'.."), &m_menu);
    QObject::connect(m_setComputerHintSecondBeforeCorner, SIGNAL(triggered()), this,
                     SLOT(doSetComputerHintSecondBeforeCorner()));

    m_clearDrivingLineHint = new QAction(QWidget::tr("Clear driving line hint"), &m_menu);
    QObject::connect(m_clearDrivingLineHint, SIGNAL(triggered()), this,
                     SLOT(doClearDrivingLineHint()));

    m_setDrivingLineHintLeft = new QAction(
        QWidget::tr("Set driving line hint 'left'.."), &m_menu);
    QObject::connect(m_setDrivingLineHintLeft, SIGNAL(triggered()), this,
                     SLOT(doSetDrivingLineHintLeft()));

    m_setDrivingLineHintRight = new QAction(
        QWidget::tr("Set driving line hint 'right'.."), &m_menu);
    QObject::connect(m_setDrivingLineHintRight, SIGNAL(triggered()), this,
                     SLOT(doSetDrivingLineHintRight()));

    m_setDrivingLineHintTop = new QAction(
        QWidget::tr("Set driving line hint 'top'.."), &m_menu);
    QObject::connect(m_setDrivingLineHintTop, SIGNAL(triggered()), this,
                     SLOT(doSetDrivingLineHintTop()));

    m_setDrivingLineHintBottom = new QAction(
        QWidget::tr("Set driving line hint 'bottom'.."), &m_menu);
    QObject::connect(m_setDrivingLineHintBottom, SIGNAL(triggered()), this,
                     SLOT(doSetDrivingLineHintBottom()));

    // Populate the menu
    m_menu.addActions(QList<QAction *>()
        << rotate90CW
        << rotate90CCW
        << m_clearComputerHint
        << m_setComputerHintFirstBeforeCorner
        << m_setComputerHintSecondBeforeCorner
        << m_clearDrivingLineHint
        << m_setDrivingLineHintLeft
        << m_setDrivingLineHintRight
        << m_setDrivingLineHintTop
        << m_setDrivingLineHintBottom);
}

void EditorView::mousePressEvent(QMouseEvent * event)
{
    if (scene())
    {
        m_clickedPos      = event->pos();
        m_clickedScenePos = mapToScene(m_clickedPos);

        if (Object * object =
            dynamic_cast<Object *>(scene()->itemAt(m_clickedScenePos)))
        {
            // Handle right button click
            if (event->button() == Qt::RightButton)
            {
                // Nothing to do currently
            }
            // Handle left button click
            else if (event->button() == Qt::LeftButton)
            {
                handleLeftButtonClickOnObject(*object);
            }

            QWidget::mousePressEvent(event);
        }
        else if (TrackTile * tile =
            dynamic_cast<TrackTile *>(scene()->itemAt(m_clickedScenePos)))
        {
            tile->setActive(true);

            // Handle right button click
            if (event->button() == Qt::RightButton)
            {
                handleRightButtonClickOnTile(*tile);
            }
            // Handle left button click
            else if (event->button() == Qt::LeftButton)
            {
                handleLeftButtonClickOnTile(*tile);
            }

            QWidget::mousePressEvent(event);
        }
    }
}

void EditorView::handleLeftButtonClickOnObject(Object & object)
{
    EditorData & editorData = MainWindow::instance()->editorData();

    // User is erasing an object
    if (editorData.mode() == EditorData::EM_ERASE_OBJECT)
    {
        if (scene())
        {
            // Remove from track data.
            editorData.trackData()->objects().remove(object);

            // Remove from scene.
            scene()->removeItem(&object);
        }
    }
    // User is initiating a drag'n'drop
    else if (editorData.mode() == EditorData::EM_NONE)
    {
        object.setZValue(object.zValue() + 1);
        editorData.setDragAndDropObject(&object);

        // Change cursor to the closed hand cursor.
        QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
    }
}

void EditorView::handleLeftButtonClickOnTile(TrackTile & tile)
{
    EditorData   & editorData   = MainWindow::instance()->editorData();
    ObjectLoader & objectLoader = MainWindow::instance()->objectLoader();

    // User is defining the route
    if (editorData.mode() == EditorData::EM_SET_ROUTE)
    {
        // Push tile to the route
        tile.setRouteIndex(editorData.trackData()->route().push(&tile));

        // Check if we might have a loop => end
        if (!tile.routeIndex() && editorData.trackData()->route().length() > 1)
        {
            editorData.setMode(EditorData::EM_NONE);
            MainWindow::instance()->endSetRoute();

            // Update route lines and close the loop
            editorData.addRouteLinesToScene(true);
        }
        else
        {
            // Update route lines but don't close the loop
            editorData.addRouteLinesToScene(false);
        }
    }
    // User is setting the tile type
    else if (editorData.mode() == EditorData::EM_SET_TILE_TYPE)
    {
        if (QAction * action = MainWindow::instance()->currentToolBarAction())
        {
            tile.setTileType(action->data().toString());
            tile.setPixmap(action->icon().pixmap(
                TrackTile::TILE_W, TrackTile::TILE_H));
        }
    }
    // User is adding an object
    else if (editorData.mode() == EditorData::EM_ADD_OBJECT)
    {
        if (QAction * action = MainWindow::instance()->currentToolBarAction())
        {
            if (scene())
            {
                ObjectData objectData(
                    objectLoader.getObjectByRole(action->data().toString()));

                unsigned int w = objectData.width;
                w = w > 0 ? w : objectData.pixmap.width();

                unsigned int h = objectData.height;
                h = h > 0 ? h : objectData.pixmap.height();

                // Create the object
                Object * newObject = new Object(
                    objectData.category, objectData.role,
                    QSizeF(w, h), objectData.pixmap);
                newObject->setLocation(m_clickedScenePos);

                // Add to scene
                scene()->addItem(newObject);

                // Add to track data
                editorData.trackData()->objects().add(*newObject);
            }
        }
    }
    // User is initiating a drag'n'drop
    else if (editorData.mode() == EditorData::EM_NONE)
    {
        tile.setZValue(tile.zValue() + 1);
        editorData.setDragAndDropSourceTile(&tile);
        editorData.setDragAndDropSourcePos(tile.pos());

        // Change cursor to the closed hand cursor.
        QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
    }
}

void EditorView::handleRightButtonClickOnTile(TrackTile & tile)
{
    // Enable all hints by default
    m_clearComputerHint->setEnabled(true);
    m_setComputerHintFirstBeforeCorner->setEnabled(true);
    m_setComputerHintSecondBeforeCorner->setEnabled(true);

    switch (tile.computerHint())
    {
    case TrackTileBase::CH_NONE:
        m_clearComputerHint->setEnabled(false);
        break;
    case TrackTileBase::CH_FIRST_BEFORE_CORNER:
        m_setComputerHintFirstBeforeCorner->setEnabled(false);
        break;
    case TrackTileBase::CH_SECOND_BEFORE_CORNER:
        m_setComputerHintSecondBeforeCorner->setEnabled(false);
        break;
    default:
        break;
    }

    // Enable all driving line hints by default
    m_clearDrivingLineHint->setEnabled(true);
    m_setDrivingLineHintLeft->setEnabled(true);
    m_setDrivingLineHintRight->setEnabled(true);
    m_setDrivingLineHintTop->setEnabled(true);
    m_setDrivingLineHintBottom->setEnabled(true);

    switch (tile.drivingLineHint())
    {
    case TrackTileBase::DLH_NONE:
        m_clearDrivingLineHint->setEnabled(false);
        break;
    case TrackTileBase::DLH_LEFT:
        m_setDrivingLineHintLeft->setEnabled(false);
        break;
    case TrackTileBase::DLH_RIGHT:
        m_setDrivingLineHintRight->setEnabled(false);
        break;
    case TrackTileBase::DLH_TOP:
        m_setDrivingLineHintTop->setEnabled(false);
        break;
    case TrackTileBase::DLH_BOTTOM:
        m_setDrivingLineHintBottom->setEnabled(false);
        break;
    default:
        break;
    }

    // Show the context menu
    QPoint globalPos = mapToGlobal(m_clickedPos);
    m_menu.exec(globalPos);
}

void EditorView::mouseReleaseEvent(QMouseEvent * event)
{
    handleTileDragRelease(event);
    handleObjectDragRelease(event);
}

void EditorView::handleTileDragRelease(QMouseEvent * event)
{
    if (scene())
    {
        EditorData & editorData = MainWindow::instance()->editorData();

        // Tile drag'n'drop active?
        if (TrackTile * sourceTile = editorData.dragAndDropSourceTile())
        {
            // Determine the dest tile
            TrackTile * destTile = sourceTile;
            QList<QGraphicsItem *> items = scene()->items(mapToScene(event->pos()));
            for (QGraphicsItem * item : items)
            {
                TrackTile * testTile = dynamic_cast<TrackTile *>(item);
                if (testTile && testTile != sourceTile)
                {
                    destTile = testTile;
                    break;
                }
            }

            // Swap tiles
            sourceTile->swap(*destTile);

            // Restore position
            sourceTile->setPos(editorData.dragAndDropSourcePos());
            sourceTile->setZValue(sourceTile->zValue() - 1);

            update();

            editorData.setDragAndDropSourceTile(nullptr);

            // Restore the cursor.
            QApplication::restoreOverrideCursor();
        }
    }
}

void EditorView::handleObjectDragRelease(QMouseEvent * event)
{
    if (scene())
    {
        EditorData & editorData = MainWindow::instance()->editorData();

        // Tile drag'n'drop active?
        if (Object * object = editorData.dragAndDropObject())
        {
            // Restore position
            object->setPos(mapToScene(event->pos()));
            object->setZValue(object->zValue() - 1);

            update();

            editorData.setDragAndDropObject(nullptr);

            // Restore the cursor.
            QApplication::restoreOverrideCursor();
        }
    }
}

void EditorView::doRotate90CW()
{
    if (TrackTile * tile =
        dynamic_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos))))
    {
        tile->rotate90CW();
    }
}

void EditorView::doRotate90CCW()
{
    if (TrackTile * tile =
        dynamic_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos))))
    {
        tile->rotate90CCW();
    }
}

void EditorView::doClearComputerHint()
{
    doSetComputerHint(TrackTileBase::CH_NONE);
}

void EditorView::doSetComputerHintFirstBeforeCorner()
{
    doSetComputerHint(TrackTileBase::CH_FIRST_BEFORE_CORNER);
}

void EditorView::doSetComputerHintSecondBeforeCorner()
{
    doSetComputerHint(TrackTileBase::CH_SECOND_BEFORE_CORNER);
}

void EditorView::doSetComputerHint(TrackTileBase::ComputerHint hint)
{
    if (TrackTile * tile =
        dynamic_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos))))
    {
        tile->setComputerHint(hint);
    }
}

void EditorView::doClearDrivingLineHint()
{
    doSetDrivingLineHint(TrackTileBase::DLH_NONE);
}

void EditorView::doSetDrivingLineHintLeft()
{
    doSetDrivingLineHint(TrackTileBase::DLH_LEFT);
}

void EditorView::doSetDrivingLineHintRight()
{
    doSetDrivingLineHint(TrackTileBase::DLH_RIGHT);
}

void EditorView::doSetDrivingLineHintTop()
{
    doSetDrivingLineHint(TrackTileBase::DLH_TOP);
}

void EditorView::doSetDrivingLineHintBottom()
{
    doSetDrivingLineHint(TrackTileBase::DLH_BOTTOM);
}

void EditorView::doSetDrivingLineHint(TrackTileBase::DrivingLineHint hint)
{
    if (TrackTile * tile =
        dynamic_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos))))
    {
        tile->setDrivingLineHint(hint);
    }
}
