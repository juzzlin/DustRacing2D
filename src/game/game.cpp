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

#include <Device>
#include <SoundManager>
#include <Sound>

#include <QDir>
#include <QTime>
#include <QApplication>

#include <SDL/SDL.h>

#include <cassert>

static const unsigned int MAX_PLAYERS = 2;

Game * Game::m_instance = nullptr;

Game::Game()
: m_stateMachine(new StateMachine)
, m_renderer(nullptr)
, m_scene(nullptr)
, m_assetManager(new MCAssetManager(
    Config::Common::dataPath,
    std::string(Config::Common::dataPath) + QDir::separator().toAscii() + "textures.conf",
    std::string(Config::Common::dataPath) + QDir::separator().toAscii() + "fonts.conf",
    std::string(Config::Common::dataPath) + QDir::separator().toAscii() + "meshes.conf"))
, m_objectFactory(new MCObjectFactory(*m_assetManager))
, m_trackLoader(new TrackLoader(*m_objectFactory))
, m_inputHandler(new InputHandler(MAX_PLAYERS))
, m_eventHandler(new EventHandler(*m_inputHandler))
, m_soundDevice(new SFX::Device)
, m_soundManager(new SFX::SoundManager)
, m_updateFps(60)
, m_updateDelay(1000 / m_updateFps)
, m_timeStep(1.0 / m_updateFps)
, m_renderFps(60)
, m_renderDelay(1000 / m_renderFps)
, m_renderCount(0)
, m_availableRenderTime(0)
, m_paused(false)
, m_mode(OnePlayerRace)
{
    assert(!Game::m_instance);
    Game::m_instance = this;

    // Connect pause toggling
    connect(m_eventHandler, SIGNAL(pauseToggled()), this, SLOT(togglePause()));

    // Connect signal that exits the whole game
    connect(m_eventHandler, SIGNAL(gameExited()), this, SLOT(exitGame()));

    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(updateFrame()));
    m_updateTimer.setInterval(m_updateDelay);

    connect(&m_renderTimer, SIGNAL(timeout()), this, SLOT(renderFrame()));
    m_renderTimer.setInterval(m_renderDelay);

    // Add race track search paths
    m_trackLoader->addTrackSearchPath(QString(Config::Common::dataPath) +
        QDir::separator() + "levels");
    m_trackLoader->addTrackSearchPath(QDir::homePath() + QDir::separator() +
        Config::Common::TRACK_SEARCH_PATH);
}

Game & Game::instance()
{
    assert(Game::m_instance);
    return *Game::m_instance;
}

void Game::setFps(unsigned int fps)
{
    m_renderFps   = fps;
    m_renderDelay = 1000 / m_renderFps;

    m_renderTimer.setInterval(m_renderDelay);
}

void Game::setRenderer(Renderer * newRenderer)
{
    m_renderer = newRenderer;

    // Note that this must be called before loading textures in order
    // to load textures to correct OpenGL context.
    m_renderer->makeCurrent();
    m_renderer->setEventHandler(*m_eventHandler);
}

void Game::setMode(GameMode gameMode)
{
    m_mode = gameMode;
}

Game::GameMode Game::mode() const
{
    return m_mode;
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

Renderer * Game::renderer() const
{
    return m_renderer;
}

bool Game::loadTracks()
{
    // Load track data
    if (int numLoaded = m_trackLoader->loadTracks())
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

bool Game::initAudio()
{
    MCLogger().info() << "Initing SDL audio..";

    // Init SDL with AUDIO only
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        throw MCException("Initing SDL failed!");
    }

    if (!m_soundDevice->init())
    {
        throw MCException("Initing SDL mixer failed!");
    }

    return true;
}

void Game::loadSounds()
{
    const std::string soundConfigPath(
        std::string(Config::Common::dataPath) + QDir::separator().toAscii() + "sounds.conf");
    MCLogger().info() << "Loading sound config from '" << soundConfigPath << "'..";
    m_soundManager->load(soundConfigPath, std::string(Config::Common::dataPath));
}

bool Game::init()
{
    if (initAudio())
    {
        loadSounds();
    }
    else
    {
        return false;
    }

    m_assetManager->load();

    if (loadTracks())
    {
        initScene();
    }
    else
    {
        return false;
    }

    return true;
}

void Game::start()
{
    m_paused = false;
    m_updateTimer.start();
    m_renderTimer.start();
}

void Game::stop()
{
    m_paused = true;
    m_updateTimer.stop();
    m_renderTimer.stop();
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
    QApplication::instance()->exit();
}

void Game::updateFrame()
{
    updateAnimations();
    m_stateMachine->update();
    m_scene->updateFrame(*m_inputHandler, m_timeStep);
}

void Game::updateAnimations()
{
    m_scene->updateAnimations();
}

void Game::renderFrame()
{
    m_renderer->updateFrame();
}

void Game::countRenderFps()
{
    m_renderCount = 0;
}

Game::~Game()
{
    delete m_stateMachine;
    delete m_scene;
    delete m_assetManager;
    delete m_trackLoader;
    delete m_objectFactory;
    delete m_eventHandler;
    delete m_inputHandler;
    delete m_soundDevice;
    delete m_soundManager;

    SDL_Quit();
}
