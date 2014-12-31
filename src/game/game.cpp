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

#include "../common/config.hpp"

#include "game.hpp"

#include "audioworker.hpp"
#include "graphicsfactory.hpp"
#include "eventhandler.hpp"
#include "inputhandler.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "statemachine.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "trackloader.hpp"
#include "trackselectionmenu.hpp"

#include <MCAssetManager>
#include <MCCamera>
#include <MCException>
#include <MCLogger>
#include <MCObjectFactory>

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QTime>
#include <QSurfaceFormat>

#include <cassert>

static const unsigned int MAX_PLAYERS = 2;

Game * Game::m_instance = nullptr;

Game::Game(bool forceNoVSync)
: m_inputHandler(new InputHandler(MAX_PLAYERS))
, m_eventHandler(new EventHandler(*m_inputHandler))
, m_stateMachine(new StateMachine(*m_inputHandler))
, m_renderer(nullptr)
, m_scene(nullptr)
, m_assetManager(new MCAssetManager(
    Config::Common::dataPath.toStdString(),
    (Config::Common::dataPath + QDir::separator().toLatin1() + "textures.conf").toStdString(),
    "",
    (Config::Common::dataPath + QDir::separator().toLatin1() + "meshes.conf").toStdString()))
, m_objectFactory(new MCObjectFactory(*m_assetManager))
, m_trackLoader(new TrackLoader(*m_objectFactory))
, m_updateFps(60)
, m_updateDelay(1000 / m_updateFps)
, m_timeStep(1.0 / m_updateFps)
, m_lapCount(m_settings.loadValue(Settings::lapCountKey(), 5))
, m_paused(false)
, m_renderElapsed(0)
, m_mode(OnePlayerRace)
, m_splitType(Vertical)
, m_audioWorker(new AudioWorker(
      Scene::NUM_CARS, Settings::instance().loadValue(Settings::soundsKey(), true)))
{
    assert(!Game::m_instance);
    Game::m_instance = this;

    createRenderer(forceNoVSync);

    connect(m_eventHandler, SIGNAL(pauseToggled()), this, SLOT(togglePause()));
    connect(m_eventHandler, SIGNAL(gameExited()), this, SLOT(exitGame()));
    connect(m_eventHandler, SIGNAL(cursorRevealed()), this, SLOT(showCursor()));
    connect(m_eventHandler, SIGNAL(cursorHid()), this, SLOT(hideCursor()));
    connect(m_eventHandler, SIGNAL(soundRequested(QString)), m_audioWorker, SLOT(playSound(QString)));

    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(updateFrame()));
    m_updateTimer.setInterval(1000 / 60);

    connect(m_stateMachine, SIGNAL(exitGameRequested()), this, SLOT(exitGame()));

    // Add race track search paths
    m_trackLoader->addTrackSearchPath(QString(Config::Common::dataPath) +
        QDir::separator() + "levels");
    m_trackLoader->addTrackSearchPath(QDir::homePath() + QDir::separator() +
        Config::Common::TRACK_SEARCH_PATH);

    m_elapsed.start();
}

Game & Game::instance()
{
    assert(Game::m_instance);
    return *Game::m_instance;
}

void Game::createRenderer(bool forceNoVSync)
{
    // Create the main window / renderer
    int hRes, vRes;
    bool nativeResolution = true;
    bool fullScreen       = false;

    m_settings.loadResolution(hRes, vRes, nativeResolution, fullScreen);

    if (nativeResolution)
    {
        QDesktopWidget * desktop = QApplication::desktop();
        hRes = desktop->screenGeometry(desktop->primaryScreen()).width();
        vRes = desktop->screenGeometry(desktop->primaryScreen()).height();
    }

    adjustSceneSize(hRes, vRes);

    MCLogger().info()
        << "Resolution: " << hRes << " " << vRes << " " << nativeResolution << " " << fullScreen;

    MCLogger().info() << "Creating the renderer..";

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
    if (forceNoVSync)
    {
        format.setSwapInterval(0);
    }
    else
    {
        format.setSwapInterval(Settings::instance().loadValue(Settings::vsyncKey(), 0));
    }
#else
    Q_UNUSED(forceNoVSync);
#endif

    m_renderer = new Renderer(hRes, vRes, nativeResolution, fullScreen);
    m_renderer->setFormat(format);

    if (fullScreen)
    {
        m_renderer->showFullScreen();
    }
    else
    {
        m_renderer->show();
    }

    m_renderer->setEventHandler(*m_eventHandler);

    connect(m_renderer, SIGNAL(initialized()), this, SLOT(init()));

    connect(m_stateMachine, SIGNAL(renderingEnabled(bool)), m_renderer, SLOT(setEnabled(bool)));
}

void Game::adjustSceneSize(int hRes, int vRes)
{
    // Adjust scene height so that view aspect ratio is taken into account.
    const int newSceneHeight = Scene::width() * vRes / hRes;
    Scene::setSize(Scene::width(), newSceneHeight);
}

void Game::showCursor()
{
    m_renderer->setCursor(Qt::ArrowCursor);
}

void Game::hideCursor()
{
    m_renderer->setCursor(Qt::BlankCursor);
}

void Game::setMode(GameMode gameMode)
{
    m_mode = gameMode;
}

Game::GameMode Game::mode() const
{
    return m_mode;
}

void Game::setSplitType(SplitType splitType)
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
    m_trackLoader->updateLockedTracks(lapCount);
}

int Game::lapCount() const
{
    return m_lapCount;
}

bool Game::hasTwoHumanPlayers() const
{
    return m_mode == TwoPlayerRace || m_mode == Duel;
}

bool Game::hasComputerPlayers() const
{
    return m_mode == TwoPlayerRace || m_mode == OnePlayerRace;
}

EventHandler & Game::eventHandler() const
{
    assert(m_eventHandler);
    return *m_eventHandler;
}

AudioWorker & Game::audioWorker()
{
    assert(m_audioWorker);
    return *m_audioWorker;
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

bool Game::loadTracks()
{
    // Load track data
    if (int numLoaded = m_trackLoader->loadTracks(m_lapCount))
    {
        MCLogger().info() << "A total of " << numLoaded << " race track(s) loaded.";
    }
    else
    {
        throw MCException("No valid race tracks found.");
    }

    return true;
}

void Game::initScene()
{
    assert(m_stateMachine);
    assert(m_renderer);

    // Create the scene
    m_scene = new Scene(*this, *m_stateMachine, *m_renderer);

    // Add tracks to the menu.
    for (unsigned int i = 0; i < m_trackLoader->tracks(); i++)
    {
        m_scene->trackSelectionMenu().addTrack(*m_trackLoader->track(i));
    }

    // Set the current game scene. Renderer calls render()
    // for all objects in the scene.
    m_renderer->setScene(*m_scene);
}

void Game::init()
{
    m_audioThread.start();
    m_audioWorker->moveToThread(&m_audioThread);
    QMetaObject::invokeMethod(m_audioWorker, "init");
    QMetaObject::invokeMethod(m_audioWorker, "loadSounds");

    m_assetManager->load();

    if (loadTracks())
    {
        initScene();
    }
    else
    {
        throw MCException("Couldn't load tracks.");
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

void Game::togglePause()
{
    if (m_paused)
    {
        start();
        MCLogger().info() << "Game continued.";
    }
    else
    {
        stop();
        MCLogger().info() << "Game paused.";
    }
}

void Game::exitGame()
{
    stop();
    m_renderer->close();

    m_audioThread.quit();
    m_audioThread.wait();

    QApplication::quit();
}

void Game::updateFrame()
{
    m_stateMachine->update();
    m_scene->updateFrame(*m_inputHandler, m_timeStep);
    m_scene->updateAnimations();
    m_scene->updateOverlays();
    m_renderer->renderNow();
}

Game::~Game()
{
    delete m_renderer;
    delete m_stateMachine;
    delete m_scene;
    delete m_assetManager;
    delete m_trackLoader;
    delete m_objectFactory;
    delete m_eventHandler;
    delete m_inputHandler;
    delete m_audioWorker;
}
