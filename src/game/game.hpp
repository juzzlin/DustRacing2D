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

#ifndef GAME_HPP
#define GAME_HPP

#include <QElapsedTimer>
#include <QObject>
#include <QTime>
#include <QTimer>
#include <QTranslator>

#include <MCWorld>

#include <chrono>

#include "application.hpp"
#include "settings.hpp"

class AudioWorker;
class Database;
class EventHandler;
class InputHandler;
class Renderer;
class Scene;
class Startlights;
class StartlightsOverlay;
class StateMachine;
class TimingOverlay;
class TrackLoader;
class QScreen;

//! The main game class.
class Game : public QObject
{
    Q_OBJECT

public:
    enum class Mode
    {
        OnePlayerRace,
        TwoPlayerRace,
        TimeTrial,
        Duel
    };

    enum class SplitType
    {
        Horizontal,
        Vertical
    };

    enum class Fps
    {
        Fps30,
        Fps60
    };

    Game(int & argc, char ** argv);
    virtual ~Game() override;

    //! Return the game instance.
    static Game & instance();

    //! \return The renderer.
    Renderer & renderer() const;

    int run();

    void setMode(Mode mode);
    Mode mode() const;

    void setSplitType(SplitType splitType);
    SplitType splitType() const;

    void setFps(Fps fps);
    Fps fps() const;

    void setLapCount(int lapCount);
    int lapCount() const;

    bool hasTwoHumanPlayers() const;
    bool hasComputerPlayers() const;

    EventHandler & eventHandler();
    AudioWorker & audioWorker();

    DifficultyProfile & difficultyProfile();

    const std::string & fontName() const;

    QScreen * screen() const;

public slots:

    void exitGame();

private slots:

    void init();

    void togglePause();

private:
    void addTrackSearchPaths();

    void adjustSceneSize(int hRes, int vRes);
    void createRenderer();

    void initScene();
    bool loadTracks();

    void parseArgs(int argc, char ** argv);

    void start();
    void stop();

    Application m_app;

    QTranslator m_appTranslator;

    std::unique_ptr<Database> m_database;

    bool m_forceNoVSync;

    Settings m_settings;

    DifficultyProfile m_difficultyProfile;

    InputHandler * m_inputHandler;
    EventHandler * m_eventHandler;

    StateMachine * m_stateMachine;

    Renderer * m_renderer;
    Scene * m_scene;

    TrackLoader * m_trackLoader;

    QScreen * m_screen = nullptr;

    int m_screenIndex = 0;

    int m_updateFps;
    int m_updateDelay;

    float m_timeStep;

    int m_lapCount;

    bool m_paused;

    QTimer m_updateTimer;
    int m_renderElapsed;

    Fps m_fps;

    Mode m_mode;

    SplitType m_splitType;

    AudioWorker * m_audioWorker;
    QThread * m_audioThread;

    MCWorld * m_world;

    QElapsedTimer m_elapsedTimer;
    qint64 m_lastUpdateTime = 0;
    qint64 m_lastDeltaMs = 0;

    static Game * m_instance;
};

#endif // GAME_HPP
