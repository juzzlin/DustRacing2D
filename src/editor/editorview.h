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

#ifndef EDITORVIEW_H
#define EDITORVIEW_H

#include <QGraphicsView>
#include <QMenu>

class QMouseEvent;
class QPaintEvent;
class EditorData;

class EditorView : public QGraphicsView
{
    Q_OBJECT

public:

    explicit EditorView(EditorData * editor, QWidget * parent = 0);

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

private:

    void createContextMenu();

    EditorData * m_editorData;
    QMenu        m_menu;
    QPoint       m_clickedPos;
};

#endif // EDITORVIEW_H
