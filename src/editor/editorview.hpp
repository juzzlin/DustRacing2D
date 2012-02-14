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

#ifndef EDITORVIEW_HPP
#define EDITORVIEW_HPP

#include <QGraphicsView>
#include <QMenu>

class QAction;
class QMouseEvent;
class QPaintEvent;
class EditorData;
class ObjectLoader;
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

private slots:

    void doRotate90CW();
    void doRotate90CCW();
    void doSetAsFlat();
    void doSetAsHill();
    void doSetAsGorge();
    void doClearComputerHint();
    void doSetComputerHintFirstBeforeCorner();
    void doSetComputerHintSecondBeforeCorner();

private:

    void createContextMenu();
    void handleLeftButtonClickOnTile(TrackTile * tile);
    void handleRightButtonClickOnTile(TrackTile * tile);

    QMenu     m_menu;
    QPoint    m_clickedPos;
    QPointF   m_clickedScenePos;
    QAction * m_setAsFlat;
    QAction * m_setAsHill;
    QAction * m_setAsGorge;
    QAction * m_clearComputerHint;
    QAction * m_setComputerHintFirstBeforeCorner;
    QAction * m_setComputerHintSecondBeforeCorner;
};

#endif // EDITORVIEW_HPP
