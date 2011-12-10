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

#include "editordata.h"
#include "editorview.h"
#include "mainwindow.h"
#include "object.h"
#include "objectloader.h"
#include "tracktile.h"
#include "../common/trackdata.h"

EditorView::EditorView(QWidget * parent)
: QGraphicsView(parent)
, m_setAsFlat(nullptr)
, m_setAsHill(nullptr)
, m_setAsGorge(nullptr)
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

        // Drag'n'drop active?
        EditorData & editorData = MainWindow::instance()->editorData();
        TrackTile  * sourceTile = editorData.dragAndDropSourceTile();
        if (sourceTile)
        {
            sourceTile->setPos(mappedPos);
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

    m_setAsFlat = new QAction(QWidget::tr("Set as flat.."), &m_menu);
    QObject::connect(m_setAsFlat, SIGNAL(triggered()), this,
                     SLOT(doSetAsFlat()));

    m_setAsHill = new QAction(QWidget::tr("Set as hill.."), &m_menu);
    QObject::connect(m_setAsHill, SIGNAL(triggered()), this,
                     SLOT(doSetAsHill()));

    m_setAsGorge = new QAction(QWidget::tr("Set as gorge.."), &m_menu);
    QObject::connect(m_setAsGorge, SIGNAL(triggered()), this,
                     SLOT(doSetAsGorge()));

    m_menu.addActions(QList<QAction *>()
                      << rotate90CW << rotate90CCW
                      << m_setAsFlat << m_setAsHill << m_setAsGorge);
}

void EditorView::mousePressEvent(QMouseEvent * event)
{
    if (scene())
    {
        m_clickedPos      = event->pos();
        m_clickedScenePos = mapToScene(m_clickedPos);

        if (TrackTile * tile =
            dynamic_cast<TrackTile *>(scene()->itemAt(m_clickedScenePos)))
        {
            tile->setActive(true);

            // Handle right button click
            if (event->button() == Qt::RightButton)
            {
                handleRightButtonClickOnTile(tile);
            }
            // Handle left button click
            else if (event->button() == Qt::LeftButton)
            {
                handleLeftButtonClickOnTile(tile);
            }

            QWidget::mousePressEvent(event);
        }
    }
}

void EditorView::handleLeftButtonClickOnTile(TrackTile * tile)
{
    EditorData   & editorData   = MainWindow::instance()->editorData();
    ObjectLoader & objectLoader = MainWindow::instance()->objectLoader();

    // User is defining the route
    if (editorData.mode() == EditorData::EM_SET_ROUTE)
    {
        // Push tile to the route
        tile->setRouteIndex(editorData.trackData()->route().push(tile));

        // Check if we might have a loop => end
        if (!tile->routeIndex() && editorData.trackData()->route().length() > 1)
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
            tile->setTileType(action->data().toString());
            tile->setPixmap(action->icon().pixmap(
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
        tile->setZValue(tile->zValue() + 1);
        editorData.setDragAndDropSourceTile(tile);
        editorData.setDragAndDropSourcePos(tile->pos());

        // Change cursor to the closed hand cursor.
        QApplication::setOverrideCursor(QCursor(Qt::ClosedHandCursor));
    }
}

void EditorView::handleRightButtonClickOnTile(TrackTile * tile)
{
    // Enable all profiles by default
    m_setAsFlat->setEnabled(true);
    m_setAsHill->setEnabled(true);
    m_setAsGorge->setEnabled(true);

    // Hide settings that are already applied
    switch (tile->profile())
    {
    case TrackTileBase::TP_FLAT:
        m_setAsFlat->setEnabled(false);
        break;
    case TrackTileBase::TP_HILL:
        m_setAsHill->setEnabled(false);
        break;
    case TrackTileBase::TP_GORGE:
        m_setAsGorge->setEnabled(false);
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
    if (scene())
    {
        // Drag'n'drop active?
        EditorData & editorData = MainWindow::instance()->editorData();
        TrackTile  * sourceTile = editorData.dragAndDropSourceTile();
        if (sourceTile)
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

            // Swap tile types
            QString sourceType = sourceTile->tileType();
            QString destType = destTile->tileType();
            sourceTile->setTileType(destType);
            destTile->setTileType(sourceType);

            // Swap tile pixmaps
            QPixmap sourcePixmap = sourceTile->pixmap();
            QPixmap destPixmap = destTile->pixmap();
            sourceTile->setPixmap(destPixmap);
            destTile->setPixmap(sourcePixmap);

            // Swap tile rotations
            int sourceAngle = sourceTile->rotation();
            int destAngle = destTile->rotation();
            sourceTile->setRotation(destAngle);
            destTile->setRotation(sourceAngle);

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

void EditorView::doSetAsFlat()
{
    if (TrackTile * tile =
        dynamic_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos))))
    {
        tile->setProfile(TrackTileBase::TP_FLAT);
    }
}

void EditorView::doSetAsHill()
{
    if (TrackTile * tile =
        dynamic_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos))))
    {
        tile->setProfile(TrackTileBase::TP_HILL);
    }
}

void EditorView::doSetAsGorge()
{
    if (TrackTile * tile =
        dynamic_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos))))
    {
        tile->setProfile(TrackTileBase::TP_GORGE);
    }
}
