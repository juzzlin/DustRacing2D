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

#ifndef EDITORVIEW_HPP
#define EDITORVIEW_HPP

#include <QGraphicsView>
#include <QMenu>
#include "../common/tracktilebase.hpp"

class QAction;
class QMouseEvent;
class QPaintEvent;
class EditorData;
class Object;
class ObjectModelLoaderoader;
class TargetNode;
class TrackTile;

class EditorView : public QGraphicsView
{
    Q_OBJECT

public:

    explicit EditorView(QWidget * parent = 0);

protected:

    //! \reimp
    void mouseMoveEvent(QMouseEvent * event);

    //! \reimp
    void mousePressEvent(QMouseEvent * event);

    //! \reimp
    void mouseReleaseEvent(QMouseEvent * event);

    //! \reimp
    void keyPressEvent(QKeyEvent * event);

private slots:

    void doRotateTile90CW();
    void doRotateTile90CCW();
    void doRotateObject();
    void doClearComputerHint();
    void doSetComputerHintBrakeHard();
    void doSetComputerHintBrake();

private:

    void createTileContextMenu();
    void createObjectContextMenu();
    void handleLeftButtonClickOnTile(TrackTile & tile);
    void handleRightButtonClickOnTile(TrackTile & tile);
    void handleLeftButtonClickOnObject(Object & object);
    void handleRightButtonClickOnObject(Object & object);
    void handleLeftButtonClickOnTargetNode(TargetNode & tnode);
    void handleRightButtonClickOnTargetNode(TargetNode & tnode);
    void handleTileDragRelease(QMouseEvent * event);
    void handleObjectDragRelease(QMouseEvent * event);
    void handleTargetNodeDragRelease(QMouseEvent * event);
    void doSetComputerHint(TrackTileBase::ComputerHint hint);

    QMenu     m_tileContextMenu;
    QMenu     m_objectContextMenu;
    QPoint    m_clickedPos;
    QPointF   m_clickedScenePos;
    QAction * m_clearComputerHint;
    QAction * m_setComputerHintBrakeHard;
    QAction * m_setComputerHintBrake;
};

#endif // EDITORVIEW_HPP
