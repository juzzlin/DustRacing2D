// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#include <QApplication>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QStatusBar>
#include <QString>

#include "editordata.hpp"
#include "editorview.hpp"
#include "mainwindow.hpp"
#include "object.hpp"
#include "objectfactory.hpp"
#include "objectmodelloader.hpp"
#include "rotatedialog.hpp"
#include "targetnode.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"

EditorView::EditorView(QWidget * parent)
: QGraphicsView(parent)
, m_clearComputerHint(nullptr)
, m_setComputerHintBrakeHard(nullptr)
, m_setComputerHintBrake(nullptr)
{
    createTileContextMenu();
    createObjectContextMenu();
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
            object->setLocation(mappedPos);
        }
        // Target node drag'n'drop active?
        else if (TargetNode * tnode = editorData.dragAndDropTargetNode())
        {
            tnode->setLocation(mappedPos);
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

void EditorView::createTileContextMenu()
{
    const QChar degreeSign(176);
    const QString dummy1(QString(QWidget::tr("Rotate 90")) +
        degreeSign + QWidget::tr(" CW.."));

    QAction * rotate90CW = new QAction(dummy1, &m_tileContextMenu);
    QObject::connect(rotate90CW, SIGNAL(triggered()), this, SLOT(doRotateTile90CW()));

    const QString dummy2(QString(QWidget::tr("Rotate 90")) +
        degreeSign + QWidget::tr(" CCW.."));

    QAction * rotate90CCW = new QAction(dummy2, &m_tileContextMenu);
    QObject::connect(rotate90CCW, SIGNAL(triggered()), this, SLOT(doRotateTile90CCW()));

    m_clearComputerHint = new QAction(QWidget::tr("Clear computer hint"), &m_tileContextMenu);
    QObject::connect(m_clearComputerHint, SIGNAL(triggered()), this, SLOT(doClearComputerHint()));

    m_setComputerHintBrakeHard = new QAction(
        QWidget::tr("Set computer hint 'brake hard'.."), &m_tileContextMenu);
    QObject::connect(m_setComputerHintBrakeHard, SIGNAL(triggered()), this,
        SLOT(doSetComputerHintBrakeHard()));

    m_setComputerHintBrake = new QAction(
        QWidget::tr("Set computer hint 'brake'.."), &m_tileContextMenu);
    QObject::connect(m_setComputerHintBrake, SIGNAL(triggered()), this,
        SLOT(doSetComputerHintBrake()));

    // Populate the menu
    m_tileContextMenu.addAction(rotate90CW);
    m_tileContextMenu.addAction(rotate90CCW);
    m_tileContextMenu.addSeparator();
    m_tileContextMenu.addAction(m_clearComputerHint);
    m_tileContextMenu.addAction(m_setComputerHintBrakeHard);
    m_tileContextMenu.addAction(m_setComputerHintBrake);
}

void EditorView::createObjectContextMenu()
{
    const QString dummy1(QString(QWidget::tr("Rotate..")));
    QAction * rotate = new QAction(dummy1, &m_tileContextMenu);
    QObject::connect(rotate, SIGNAL(triggered()), this, SLOT(doRotateObject()));

    // Populate the menu
    m_objectContextMenu.addAction(rotate);
}

void EditorView::mousePressEvent(QMouseEvent * event)
{
    if (scene())
    {
        EditorData & editorData = MainWindow::instance()->editorData();

        m_clickedPos      = event->pos();
        m_clickedScenePos = mapToScene(m_clickedPos);

        // Fetch all items at the location
        QList<QGraphicsItem *> items = scene()->items(
            m_clickedScenePos, Qt::IntersectsItemShape, Qt::DescendingOrder);

        // User is erasing an object
        if (editorData.mode() == EditorData::EM_ERASE_OBJECT)
        {
            // We need to find the first object to be erased, because
            // there might be also overlapping TargetNodes.
            for (QGraphicsItem * item : items)
            {
                if (Object * object = dynamic_cast<Object *>(item))
                {
                    // Remove from track data.
                    editorData.trackData()->objects().remove(*object);

                    // Remove from scene.
                    scene()->removeItem(object);

                    break;
                }
            }
        }
        // User is adding an object
        else if (editorData.mode() == EditorData::EM_ADD_OBJECT)
        {
            if (QAction * action = MainWindow::instance()->currentToolBarAction())
            {
                if (scene())
                {
                    // Create the object
                    Object & object = ObjectFactory::createObject(action->data().toString());
                    object.setLocation(m_clickedScenePos);

                    // Add to scene
                    scene()->addItem(&object);

                    // Add to track data
                    editorData.trackData()->objects().add(object);
                }
            }
        }
        // Default actions
        else
        {
            if (items.size())
            {
                QGraphicsItem * item = *items.begin();
                if (Object * object = dynamic_cast<Object *>(item))
                {
                    // Handle right button click
                    if (event->button() == Qt::RightButton)
                    {
                        handleRightButtonClickOnObject(*object);
                    }
                    // Handle left button click
                    else if (event->button() == Qt::LeftButton)
                    {
                        handleLeftButtonClickOnObject(*object);
                    }

                    QWidget::mousePressEvent(event);
                }
                else if (TargetNode * tnode = dynamic_cast<TargetNode *>(item))
                {
                    // Handle right button click
                    if (event->button() == Qt::RightButton)
                    {
                        handleRightButtonClickOnTargetNode(*tnode);
                    }
                    // Handle left button click
                    else if (event->button() == Qt::LeftButton)
                    {
                        handleLeftButtonClickOnTargetNode(*tnode);
                    }

                    QWidget::mousePressEvent(event);
                }
                else if (TrackTile * tile = dynamic_cast<TrackTile *>(item))
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
    }
}

void EditorView::handleLeftButtonClickOnObject(Object & object)
{
    EditorData & editorData = MainWindow::instance()->editorData();

    // User is initiating a drag'n'drop
    if (editorData.mode() == EditorData::EM_NONE)
    {
        object.setZValue(object.zValue() + 1);
        editorData.setDragAndDropObject(&object);
        editorData.setSelectedObject(&object);

        // Change cursor to the closed hand cursor.
        QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
    }
}

void EditorView::handleLeftButtonClickOnTargetNode(TargetNode & tnode)
{
    EditorData & editorData = MainWindow::instance()->editorData();

    // User is initiating a drag'n'drop
    if (editorData.mode() == EditorData::EM_NONE)
    {
        tnode.setZValue(tnode.zValue() + 1);
        editorData.setDragAndDropTargetNode(&tnode);

        // Change cursor to the closed hand cursor.
        QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
    }
    // It's not possible to make nodes overlap if not handled also here.
    else if (editorData.mode() == EditorData::EM_SET_ROUTE)
    {
        editorData.pushNewTargetNodeToRoute(m_clickedScenePos);
    }
}

void EditorView::handleLeftButtonClickOnTile(TrackTile & tile)
{
    EditorData & editorData = MainWindow::instance()->editorData();

    // User is defining the route
    if (editorData.mode() == EditorData::EM_SET_ROUTE)
    {
        editorData.pushNewTargetNodeToRoute(m_clickedScenePos);
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
    m_setComputerHintBrakeHard->setEnabled(true);
    m_setComputerHintBrake->setEnabled(true);

    switch (tile.computerHint())
    {
    case TrackTileBase::CH_NONE:
        m_clearComputerHint->setEnabled(false);
        break;
    case TrackTileBase::CH_BRAKE_HARD:
        m_setComputerHintBrakeHard->setEnabled(false);
        break;
    case TrackTileBase::CH_BRAKE:
        m_setComputerHintBrake->setEnabled(false);
        break;
    default:
        break;
    }

    // Show the context menu
    QPoint globalPos = mapToGlobal(m_clickedPos);
    m_tileContextMenu.exec(globalPos);
}

void EditorView::handleRightButtonClickOnObject(Object & object)
{
    EditorData & editorData = MainWindow::instance()->editorData();

    // Show the context menu
    QPoint globalPos = mapToGlobal(m_clickedPos);
    m_objectContextMenu.exec(globalPos);
    editorData.setSelectedObject(&object);
}

void EditorView::handleRightButtonClickOnTargetNode(TargetNode &)
{
    // Do nothing currently
}

void EditorView::mouseReleaseEvent(QMouseEvent * event)
{
    handleTileDragRelease(event);
    handleObjectDragRelease(event);
    handleTargetNodeDragRelease(event);
}

void EditorView::keyPressEvent(QKeyEvent * event)
{
    const EditorData & editorData = MainWindow::instance()->editorData();
    if (Object * object = editorData.selectedObject())
    {
        if (!event->isAutoRepeat())
        {
            switch (event->key())
            {
            case Qt::Key_Left:
                object->setLocation(QPointF(object->location().x() - 1, object->location().y()));
                break;
            case Qt::Key_Right:
                object->setLocation(QPointF(object->location().x() + 1, object->location().y()));
                break;
            case Qt::Key_Up:
                object->setLocation(QPointF(object->location().x(), object->location().y() - 1));
                break;
            case Qt::Key_Down:
                object->setLocation(QPointF(object->location().x(), object->location().y() + 1));
                break;
            default:
                break;
            }
        }
    }
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
            destTile->setZValue(sourceTile->zValue());

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

        // Object drag'n'drop active?
        if (Object * object = editorData.dragAndDropObject())
        {
            // Set the new position position
            object->setLocation(mapToScene(event->pos()));
            object->setZValue(object->zValue() - 1);

            update();

            editorData.setDragAndDropObject(nullptr);

            // Restore the cursor.
            QApplication::restoreOverrideCursor();
        }
    }
}

void EditorView::handleTargetNodeDragRelease(QMouseEvent * event)
{
    if (scene())
    {
        EditorData & editorData = MainWindow::instance()->editorData();

        // Target node drag'n'drop active?
        if (TargetNode * tnode = editorData.dragAndDropTargetNode())
        {
            // Set the new position position
            tnode->setLocation(mapToScene(event->pos()));
            tnode->setZValue(tnode->zValue() - 1);

            update();

            editorData.setDragAndDropTargetNode(nullptr);

            // Restore the cursor.
            QApplication::restoreOverrideCursor();
        }
    }
}

void EditorView::doRotateTile90CW()
{
    if (TrackTile * tile =
        dynamic_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos))))
    {
        tile->rotate90CW();
    }
}

void EditorView::doRotateTile90CCW()
{
    if (TrackTile * tile =
        dynamic_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos))))
    {
        tile->rotate90CCW();
    }
}

void EditorView::doRotateObject()
{
    RotateDialog dialog;
    if (dialog.exec() == QDialog::Accepted)
    {
        if (Object * object =
            dynamic_cast<Object *>(scene()->itemAt(mapToScene(m_clickedPos))))
        {
            object->setRotation(static_cast<int>(dialog.angle() + object->rotation()) % 360);
        }
    }
}

void EditorView::doClearComputerHint()
{
    doSetComputerHint(TrackTileBase::CH_NONE);
}

void EditorView::doSetComputerHintBrakeHard()
{
    doSetComputerHint(TrackTileBase::CH_BRAKE_HARD);
}

void EditorView::doSetComputerHintBrake()
{
    doSetComputerHint(TrackTileBase::CH_BRAKE);
}

void EditorView::doSetComputerHint(TrackTileBase::ComputerHint hint)
{
    if (TrackTile * tile =
        dynamic_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos))))
    {
        tile->setComputerHint(hint);
    }
}
