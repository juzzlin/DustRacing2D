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

#ifndef EDITORVIEW_HPP
#define EDITORVIEW_HPP

#include "../common/tracktilebase.hpp"
#include <QGraphicsView>
#include <QMenu>

class Mediator;
class QAction;
class QMouseEvent;
class QPaintEvent;
class Object;
class ObjectModelLoaderoader;
class TargetNode;
class TrackTile;

class EditorView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit EditorView(Mediator & mediator);

    void updateSceneRect();

protected:
    void mouseMoveEvent(QMouseEvent * event) override;

    void mousePressEvent(QMouseEvent * event) override;

    void mouseReleaseEvent(QMouseEvent * event) override;

    void keyPressEvent(QKeyEvent * event) override;

    void wheelEvent(QWheelEvent * event) override;

private:
    void addCurrentToolBarObjectToScene();

    void changeTileType(TrackTile & tile, QAction * action);

    void createTileContextMenuActions();

    void createObjectContextMenuActions();

    void createTargetNodeContextMenuActions();

    void eraseObjectAtCurrentClickedPos();

    void handleMousePressEventOnTile(QMouseEvent & event, TrackTile & tile);

    void handleLeftButtonClickOnTile(TrackTile & tile);

    void handleRightButtonClickOnTile(TrackTile & tile);

    void handleMousePressEventOnObject(QMouseEvent & event, Object & object);

    void handleLeftButtonClickOnObject(Object & object);

    void handleRightButtonClickOnObject(Object & object);

    void handleMousePressEventOnTargetNode(QMouseEvent & event, TargetNode & tnode);

    void handleLeftButtonClickOnTargetNode(TargetNode & tnode);

    void handleRightButtonClickOnTargetNode(TargetNode & tnode);

    void handleTileDragRelease(QMouseEvent * event);

    void handleObjectDragRelease(QMouseEvent * event);

    void handleTargetNodeDragRelease(QMouseEvent * event);

    void openObjectContextMenu();

    void openTargetNodeContextMenu();

    void openTileContextMenu(TrackTile & tile);

    void setComputerHint(TrackTileBase::ComputerHint hint);

    static void setTileType(TrackTile & tile, QAction * action);

    QMenu m_tileContextMenu;

    QMenu m_objectContextMenu;

    QMenu m_targetNodeContextMenu;

    QPoint m_clickedPos;

    QPointF m_clickedScenePos;

    QAction * m_clearComputerHint = nullptr;

    QAction * m_setComputerHintBrakeHard = nullptr;

    QAction * m_setComputerHintBrake = nullptr;

    QAction * m_deleteRow = nullptr;

    QAction * m_deleteCol = nullptr;

    QAction * m_excludeFromMinimap = nullptr;

    QAction * m_forceStationaryAction = nullptr;

    Mediator & m_mediator;
};

#endif // EDITORVIEW_HPP
