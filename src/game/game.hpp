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

#ifndef GAME_HPP
#define GAME_HPP

#include <QObject>
#include <QTimer>

class EventHandler;
class InputHandler;
class Renderer;
class Scene;
class Speedometer;
class Startlights;
class StartlightsOverlay;
class StateMachine;
class TimingOverlay;
class TrackLoader;
class MCObjectFactory;
class MCSurfaceManager;
class MCTextureFontManager;

//! The main game class.
class Game : public QObject
{
    Q_OBJECT

public:

    //! Constructor
    Game();

    //! Destructor
    virtual ~Game();

    //! Return the game instance.
    static Game & instance();

    //! Set target value for frames per second. Default is 60.
    void setTargetUpdateFps(unsigned int fps);

    void setRenderer(Renderer * renderer);

    Renderer * renderer() const;

    //! Init the game.
    //! \return True on success.
    bool init();

    //! Start the game.
    void start();

    //! Stop scene.
    void stop();

public slots:

    void togglePause();
    void exitGame();

private slots:

    void updateFrame();
    void updateAnimations();
    void countRenderFps();
    void finish();

private:

    void loadSurfaces();
    void loadFonts();
    bool loadTracks();
    void initScene();

    StateMachine         * m_stateMachine;
    Renderer             * m_renderer;
    Scene                * m_scene;
    MCSurfaceManager     * m_textureManager;
    MCTextureFontManager * m_textureFontManager;
    MCObjectFactory      * m_objectFactory;
    TrackLoader          * m_trackLoader;
    InputHandler         * m_inputHandler;
    EventHandler         * m_eventHandler;
    unsigned int           m_updateFps;
    unsigned int           m_updateDelay;
    float                  m_timeStep;
    unsigned int           m_renderCount;
    int                    m_availableRenderTime;
    bool                   m_paused;
    QTimer                 m_updateTimer;

    static Game * m_instance;
};

#endif // GAME_HPP
