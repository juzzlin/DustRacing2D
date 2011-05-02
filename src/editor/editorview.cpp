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

#include <QMouseEvent>
#include <QGraphicsItem>

#include "editor.h"
#include "editorview.h"
#include "trackdata.h"
#include "tracktile.h"
#include "mainwindow.h"

EditorView::EditorView(Editor * editor, QWidget *parent) :
    QGraphicsView(parent),
    m_editor(editor)
{
    createContextMenu();
}

void EditorView::mouseMoveEvent(QMouseEvent * event)
{
    if (scene())
    {
        if (TrackTile * tile =
            static_cast<TrackTile *>(scene()->itemAt(mapToScene(event->pos()))))
        {
            tile->setActive(true);
        }
    }
}

void EditorView::createContextMenu()
{
    QChar degreeSign(176);

    QString dummy1(QString(QWidget::tr("Rotate 90")) +
                   degreeSign + QWidget::tr(" CW.."));

    QAction * rotate90CW = new QAction(dummy1, &m_menu);
    QObject::connect(rotate90CW, SIGNAL(triggered()), this, SLOT(doRotate90CW()));

    QString dummy2(QString(QWidget::tr("Rotate 90")) +
                   degreeSign + QWidget::tr(" CCW.."));

    QAction * rotate90CCW = new QAction(dummy2, &m_menu);
    QObject::connect(rotate90CCW, SIGNAL(triggered()), this, SLOT(doRotate90CCW()));

    m_menu.addActions(QList<QAction *>() << rotate90CW << rotate90CCW);
}

void EditorView::mousePressEvent(QMouseEvent * event)
{
    if (scene())
    {
        m_clickedPos = event->pos();

        if (TrackTile * tile =
            static_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos))))
        {
            tile->setActive(true);

            // Handle right button click
            if (event->button() == Qt::RightButton)
            {
                // Show the context menu
                QPoint globalPos = mapToGlobal(m_clickedPos);
                m_menu.exec(globalPos);
            }
            // Handle left button click
            else if (event->button() == Qt::LeftButton)
            {
                // User is defining the route
                if (m_editor->mode() == Editor::EM_SETROUTE)
                {
                    // Push tile to the route
                    tile->setRouteIndex(m_editor->trackData()->route().push(tile));

                    // Check if we might have a loop => end
                    if (!tile->routeIndex() && m_editor->trackData()->route().length() > 1)
                    {
                        m_editor->setMode(Editor::EM_NONE);
                        MainWindow::instance()->endSetRoute();
                    }
                }
                // User is setting the tile type
                else if (m_editor->mode() == Editor::EM_SETTILETYPE)
                {
                    if (QAction * action = MainWindow::instance()->currentToolBarAction())
                        tile->setTileType(action->data().toString());
                }
            }

            QWidget::mousePressEvent(event);
        }
    }
}

void EditorView::doRotate90CW()
{
    if (TrackTile * tile =
        static_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos))))
        tile->rotate90CW();
}

void EditorView::doRotate90CCW()
{
    if (TrackTile * tile =
        static_cast<TrackTile *>(scene()->itemAt(mapToScene(m_clickedPos))))
        tile->rotate90CCW();
}
