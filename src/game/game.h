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

#include <QObject>
#include <QTimer>

class Scene;
class Renderer;
class TrackLoader;
class MCTextureManager;

//! The main game class.
class Game : public QObject
{
    Q_OBJECT

public:

    //! Constructor
    Game();

    //! Destructor
    virtual ~Game();

    //! Set target value for frames per second.
    //! Default is 30. This is independent of the
    //! rendering FPS.
    void setTargetFps(unsigned int fps);

    void setRenderer(Renderer * renderer);

    Renderer * renderer() const;

    //! Start scene.
    void start();

    //! Stop scene.
    void stop();

private slots:

    void updateFrame();

private:

    Scene            * m_scene;
    Renderer         * m_renderer;
    MCTextureManager * m_textureManager;
    TrackLoader      * m_trackLoader;
    QTimer             m_timer;
    unsigned int       m_targetFps;
};

#endif // GAME_H
