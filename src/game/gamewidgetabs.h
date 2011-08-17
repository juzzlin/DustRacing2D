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

#ifndef GAMEWIDGETABS_H
#define GAMEWIDGETABS_H

#include <QGLWidget>

class QTimer;
class QKeyEvent;

class GameWidgetAbs : public QGLWidget
{
    Q_OBJECT

public:

    explicit GameWidgetAbs(QWidget * parent = 0);
    virtual ~GameWidgetAbs();

protected:

    virtual void initializeGL() = 0;
    virtual void resizeGL(int width, int height) = 0;
    virtual void paintGL() = 0;
    virtual void keyPressEvent(QKeyEvent * e);

protected slots:

private:

    QTimer * m_timer;
};

#endif // GAMEWIDGETABS_H
