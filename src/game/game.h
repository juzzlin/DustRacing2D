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

#ifndef GAME_H
#define GAME_H

#include <QGLWidget>
#include <QTimer>

//! The game widget.
class Game : public QGLWidget
{
    Q_OBJECT

public:

    //! Constructor.
    Game(QWidget * parent = NULL);

    //! Destructor.
    virtual ~Game();

    //! Set target value for frames per second.
    //! Default is 30.
    void setTargetFps(unsigned int fps);

    //! Start rendering.
    void start();

    //! Stop rendering.
    void stop();

protected:

    //! \reimp
    virtual void initializeGL();

    //! \reimp
    virtual void resizeGL(int width, int height);

    //! \reimp
    virtual void paintGL();

private slots:

    void updateFrame();

private:

    //! Update timer.
    QTimer m_timer;

    //! Target FPS.
    unsigned int m_targetFps;
};

#endif // GAME_H
