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

#include "../common/config.hpp"
#include "../common/userexception.hpp"

#include "game.hpp"

#include "audioworker.hpp"
#include "database.hpp"
#include "eventhandler.hpp"
#include "graphicsfactory.hpp"
#include "inputhandler.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "statemachine.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "trackloader.hpp"
#include "trackselectionmenu.hpp"

#include <MCCamera>
#include <MCWorldRenderer>

#include <QDir>
#include <QGuiApplication>
#include <QProcessEnvironment>
#include <QScreen>
#include <QSurfaceFormat>
#include <QThread>
#include <QTime>

#include "argengine.hpp"
#include "simple_logger.hpp"

#include <cassert>

static const unsigned int MAX_PLAYERS = 2;

Game * Game::m_instance = nullptr;

using juzzlin::L;

Game::Game(int & argc, char ** argv)
  : m_app(argc, argv)
  , m_database(std::make_unique<Database>())
  , m_forceNoVSync(false)
  , m_settings()
  , m_difficultyProfile(m_settings.loadDifficulty())
  , m_inputHandler(new InputHandler(MAX_PLAYERS))
  , m_eventHandler(new EventHandler(*m_inputHandler))
  , m_stateMachine(new StateMachine(*m_inputHandler))
  , m_renderer(nullptr)
  , m_scene(nullptr)
  , m_trackLoader(new TrackLoader)
  , m_screenIndex(m_settings.loadValue(m_settings.screenKey(), 0))
  , m_updateFps(60)
  , m_updateDelay(1000 / m_updateFps)
  , m_timeStep(1000 / m_updateFps)
  , m_lapCount(m_settings.loadValue(Settings::lapCountKey(), 5))
  , m_paused(false)
  , m_renderElapsed(0)
  , m_fps(m_settings.loadValue(Settings::fpsKey()) == 30 ? Fps::Fps30 : Fps::Fps60)
  , m_mode(Mode::OnePlayerRace)
  , m_splitType(SplitType::Vertical)
  , m_audioWorker(new AudioWorker(
      static_cast<int>(Scene::carCount()), Settings::instance().loadValue(Settings::soundsKey(), true)))
  , m_audioThread(new QThread)
  , m_world(new MCWorld)
{
    assert(!Game::m_instance);
    Game::m_instance = this;

    parseArgs(argc, argv);

    createRenderer();

    connect(&m_difficultyProfile, &DifficultyProfile::difficultyChanged, this, [this]() {
        m_trackLoader->updateLockedTracks(m_lapCount, m_difficultyProfile.difficulty());
    });

    connect(m_eventHandler, &EventHandler::pauseToggled, this, &Game::togglePause);
    connect(m_eventHandler, &EventHandler::gameExited, this, &Game::exitGame);

    connect(m_eventHandler, &EventHandler::cursorRevealed, this, [this]() {
        m_renderer->setCursor(Qt::ArrowCursor);
    });

    connect(m_eventHandler, &EventHandler::cursorHid, this, [this]() {
        m_renderer->setCursor(Qt::BlankCursor);
    });

    connect(m_eventHandler, &EventHandler::soundRequested, m_audioWorker, &AudioWorker::playSound);

    m_elapsedTimer.start();
    m_lastUpdateTime = m_elapsedTimer.elapsed();

    connect(&m_updateTimer, &QTimer::timeout, this, [this]() {
        const qint64 now = m_elapsedTimer.elapsed();
        const qint64 deltaMs = now - m_lastUpdateTime;
        m_lastUpdateTime = now;
        m_stateMachine->update();
        m_scene->updateFrame(*m_inputHandler, std::chrono::milliseconds { (deltaMs + m_lastDeltaMs) / 2 });
        m_lastDeltaMs = deltaMs;
        m_scene->updateOverlays();
        m_renderer->renderNow();
    });

    m_updateTimer.setInterval(m_updateDelay);

    connect(m_stateMachine, &StateMachine::exitGameRequested, this, &Game::exitGame);

    addTrackSearchPaths();
}

Game & Game::instance()
{
    assert(Game::m_instance);
    return *Game::m_instance;
}

static void initTranslations(QTranslator & appTranslator, QGuiApplication & app, QString lang = "")
{
    if (lang == "")
    {
        lang = QLocale::system().name();
    }

    if (appTranslator.load(QString(DATA_PATH) + "/translations/dustrac-game_" + lang))
    {
        app.installTranslator(&appTranslator);
        L().info() << "Loaded translations for " << lang.toStdString();
    }
    else
    {
        L().warning() << "Failed to load translations for " << lang.toStdString();
    }
}

void Game::addTrackSearchPaths()
{
    m_trackLoader->addTrackSearchPath(QString(Config::General::dataPath) + QDir::separator() + "levels");
    m_trackLoader->addTrackSearchPath(QDir::homePath() + QDir::separator() + Config::General::TRACK_SEARCH_PATH);

#ifdef __unix__
    // See: https://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html,
    //      https://github.com/juzzlin/DustRacing2D/issues/49
    m_trackLoader->addTrackSearchPath(Config::General::Unix::getXdgTrackSearchPath());
#endif
}

void Game::parseArgs(int argc, char ** argv)
{
    QString lang = "";

    juzzlin::Argengine ae(argc, argv);
    ae.addOption(
      { "--screen" }, [=](std::string value) {
          m_screenIndex = std::stoi(value);
          m_settings.saveValue(m_settings.screenKey(), m_screenIndex);
      },
      false, "Force a certain screen on multi-display setups.");

    ae.addOption(
      { "--lang" }, [&](std::string value) {
          lang = value.c_str();
      },
      false, "Force language: fi, fr, it, cs, nl, tr.");

    ae.addOption(
      { "--no-vsync" }, [=]() {
          m_forceNoVSync = true;
      },
      false, "Force vsync off.");

    ae.addOption(
      { "--debug" }, [=]() {
          L::setLoggingLevel(L::Level::Debug);
      },
      false, "Set log level to debug.");

    ae.addOption(
      { "--trace" }, [=]() {
          L::setLoggingLevel(L::Level::Trace);
      },
      false, "Set log level to trace.");

    ae.setHelpText("\nUsage: " + std::string(argv[0]) + " [OPTIONS]");

    ae.parse();

    initTranslations(m_appTranslator, m_app, lang);
}

void Game::createRenderer()
{
    QSurfaceFormat format;

#ifdef __MC_GL30__
    format.setVersion(3, 0);
    format.setProfile(QSurfaceFormat::CoreProfile);
#elif defined(__MC_GLES__)
    format.setVersion(1, 0);
#else
    format.setVersion(2, 1);
#endif
    format.setSamples(0);

// Supported only in Qt 5.3+
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
    if (m_forceNoVSync)
    {
        format.setSwapInterval(0);
    }
    else
    {
        format.setSwapInterval(Settings::instance().loadVSync());
    }
#endif

    // Create the main window / renderer
    int hRes, vRes;
    bool fullScreen = false;

    m_settings.loadResolution(hRes, vRes, fullScreen);

    const auto screens = QGuiApplication::screens();

    L().info() << "Screen: " << m_screenIndex << "/" << screens.size();

    m_screen = m_screenIndex < screens.size() ? screens.at(m_screenIndex) : screens.at(0);

    if (!hRes || !vRes)
    {
        hRes = m_screen->geometry().width();
        vRes = m_screen->geometry().height();
    }

    adjustSceneSize(m_screen->geometry().width(), m_screen->geometry().height());

    L().info() << "Screen resolution: " << m_screen->geometry().width() << "x" << m_screen->geometry().height();
    L().info() << "Virtual resolution: " << hRes << "x" << vRes << ", fullScreen: " << fullScreen;

    L().debug() << "Creating the renderer..";
    
    m_renderer = new Renderer(hRes,
                              vRes,
                              m_screen->geometry().width(),
                              m_screen->geometry().height(),
                              fullScreen,
                              m_world->renderer().glScene());
    m_renderer->setFormat(format);
    m_renderer->setCursor(Qt::BlankCursor);

    if (fullScreen)
    {
        m_renderer->setGeometry(m_screen->geometry());
        m_renderer->showFullScreen();
    }
    else
    {
        m_renderer->show();
    }

    m_renderer->setEventHandler(*m_eventHandler);

    connect(m_renderer, &Renderer::initialized, this, &Game::init);

    connect(m_stateMachine, &StateMachine::renderingEnabled, m_renderer, &Renderer::setEnabled);
}

void Game::adjustSceneSize(int hRes, int vRes)
{
    // Adjust scene height so that view aspect ratio is taken into account.
    Scene::setSize(Scene::width(), Scene::width() * vRes / hRes);
}

void Game::setMode(Game::Mode mode)
{
    m_mode = mode;
}

Game::Mode Game::mode() const
{
    return m_mode;
}

void Game::setSplitType(Game::SplitType splitType)
{
    m_splitType = splitType;
}

Game::SplitType Game::splitType() const
{
    return m_splitType;
}

void Game::setLapCount(int lapCount)
{
    m_lapCount = lapCount;
    m_trackLoader->updateLockedTracks(lapCount, m_difficultyProfile.difficulty());
}

int Game::lapCount() const
{
    return m_lapCount;
}

bool Game::hasTwoHumanPlayers() const
{
    return m_mode == Mode::TwoPlayerRace || m_mode == Mode::Duel;
}

bool Game::hasComputerPlayers() const
{
    return m_mode == Mode::TwoPlayerRace || m_mode == Mode::OnePlayerRace;
}

EventHandler & Game::eventHandler()
{
    assert(m_eventHandler);
    return *m_eventHandler;
}

AudioWorker & Game::audioWorker()
{
    assert(m_audioWorker);
    return *m_audioWorker;
}

DifficultyProfile & Game::difficultyProfile()
{
    return m_difficultyProfile;
}

const std::string & Game::fontName() const
{
    static const std::string fontName("generated");
    return fontName;
}

Renderer & Game::renderer() const
{
    assert(m_renderer);
    return *m_renderer;
}

int Game::run()
{
    return m_app.exec();
}

QScreen * Game::screen() const
{
    return m_screen;
}

bool Game::loadTracks()
{
    // Load track data
    if (int numLoaded = m_trackLoader->loadTracks(m_lapCount, m_difficultyProfile.difficulty()))
    {
        L().info() << "A total of " << numLoaded << " race track(s) loaded.";
    }
    else
    {
        throw std::runtime_error("No valid race tracks found.");
    }

    return true;
}

void Game::initScene()
{
    assert(m_stateMachine);
    assert(m_renderer);

    // Create the scene
    m_scene = new Scene(*this, *m_stateMachine, *m_renderer, *m_world);

    auto trackSelectionMenu = std::dynamic_pointer_cast<TrackSelectionMenu>(m_scene->trackSelectionMenu());
    assert(trackSelectionMenu);

    // Add tracks to the menu.
    for (unsigned int i = 0; i < m_trackLoader->tracks(); i++)
    {
        trackSelectionMenu->addTrack(m_trackLoader->track(i));
    }

    // Set the current game scene. Renderer calls render()
    // for all objects in the scene.
    m_renderer->setScene(*m_scene);
}

void Game::init()
{
    m_audioThread->start();
    m_audioWorker->moveToThread(m_audioThread);
    QMetaObject::invokeMethod(m_audioWorker, "init");
    QMetaObject::invokeMethod(m_audioWorker, "loadSounds");

    m_trackLoader->loadAssets();

    if (loadTracks())
    {
        initScene();
    }
    else
    {
        throw std::runtime_error("Couldn't load tracks.");
    }

    start();
}

void Game::start()
{
    m_paused = false;
    m_updateTimer.start();
}

void Game::stop()
{
    m_paused = true;
    m_updateTimer.stop();
}

Game::Fps Game::fps() const
{
    return m_fps;
}

void Game::setFps(Game::Fps fps)
{
    m_fps = fps;
}

void Game::togglePause()
{
    if (m_paused)
    {
        start();
        L().info() << "Game continued.";
    }
    else
    {
        stop();
        L().info() << "Game paused.";
    }
}

void Game::exitGame()
{
    stop();

    m_renderer->close();

    m_audioThread->quit();
    m_audioThread->wait();

    m_app.quit();
}

Game::~Game()
{
    delete m_stateMachine;
    m_stateMachine = nullptr;

    delete m_scene;
    m_scene = nullptr;

    delete m_trackLoader;
    m_trackLoader = nullptr;

    delete m_eventHandler;
    m_eventHandler = nullptr;

    delete m_inputHandler;
    m_inputHandler = nullptr;

    delete m_audioWorker;
    m_audioWorker = nullptr;

    delete m_world;
    m_world = nullptr;

    delete m_renderer;
    m_renderer = nullptr;
}
