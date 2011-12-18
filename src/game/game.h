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

#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QTimer>

class InputHandler;
class Scene;
class Renderer;
class TrackLoader;
class MCCamera;
class MCObjectFactory;
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
    void setTargetUpdateFps(unsigned int fps);

    void setTargetRenderFps(unsigned int fps);

    void setRenderer(Renderer * renderer);

    Renderer * renderer() const;

    //! Init the game.
    //! \return True on success.
    bool init();

    //! Start the game.
    void start();

    //! Stop scene.
    void stop();

private slots:

    void updateFrame();
    void countRenderFps();
    void finish();

private:

    void renderFrame();

    Renderer         * m_pRenderer;
    Scene            * m_pScene;
    MCTextureManager * m_pTextureManager;
    MCObjectFactory  * m_pObjectFactory;
    TrackLoader      * m_pTrackLoader;
    MCCamera         * m_pCamera;
    InputHandler     * m_pInputHandler;
    QTimer             m_updateTimer;
    QTimer             m_renderTimer;
    unsigned int       m_updateFps;
    unsigned int       m_renderFps;
    float              m_timeStep;
    unsigned int       m_renderCount;
    int                m_availableRenderTime;
};

#endif // GAME_H
