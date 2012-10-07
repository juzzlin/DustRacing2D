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

#include "../common/config.hpp"

#include "game.hpp"
#include "eventhandler.hpp"
#include "inputhandler.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "settings.hpp"
#include "statemachine.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "trackloader.hpp"
#include "trackselectionmenu.hpp"

#include <MCCamera>
#include <MCLogger>
#include <MCObjectFactory>
#include <MCSurfaceManager>
#include <MCTextureFontManager>

#include <QDir>
#include <QTime>
#include <QApplication>

#include <cassert>
#include <chrono>
#include <thread>

namespace
{
static const unsigned int MAX_PLAYERS = 2;
static const unsigned int NUM_CARS    = 10;
}

Game * Game::m_instance = nullptr;

Game::Game()
: m_stateMachine(new StateMachine)
, m_renderer(nullptr)
, m_scene(nullptr)
, m_textureManager(new MCSurfaceManager)
, m_textureFontManager(new MCTextureFontManager(*m_textureManager))
, m_objectFactory(new MCObjectFactory(*m_textureManager))
, m_trackLoader(new TrackLoader(*m_textureManager, *m_objectFactory))
, m_inputHandler(new InputHandler(MAX_PLAYERS))
, m_eventHandler(new EventHandler(*m_inputHandler))
, m_updateFps(60)
, m_updateDelay(1000 / m_updateFps)
, m_timeStep(1.0 / m_updateFps)
, m_renderCount(0)
, m_availableRenderTime(0)
, m_paused(false)
, m_exit(false)
, m_settings(new Settings)
{
    assert(!Game::m_instance);
    Game::m_instance = this;

    // Connect pause toggling
    connect(m_eventHandler, SIGNAL(pauseToggled()), this, SLOT(togglePause()));

    // Connect signal that exits the whole game
    connect(m_eventHandler, SIGNAL(gameExited()), this, SLOT(exitGame()));

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

void Game::setTargetUpdateFps(unsigned int fps)
{
    m_updateFps   = fps;
    m_timeStep    = 1.0 / m_updateFps;
    m_updateDelay = 1000 / m_updateFps;
}

void Game::setRenderer(Renderer * newRenderer)
{
    m_renderer = newRenderer;

    // Note that this must be called before loading textures in order
    // to load textures to correct OpenGL context.
    m_renderer->makeCurrent();
    m_renderer->setEventHandler(*m_eventHandler);
}

void Game::finish()
{
}

Renderer * Game::renderer() const
{
    return m_renderer;
}

void Game::loadSurfaces()
{
    // Load texture data
    const std::string textureConfigPath = std::string(Config::Common::dataPath) +
        QDir::separator().toAscii() + "textures.conf";

    MCLogger().info() << "Loading texture config from '" << textureConfigPath << "'..";

    // Load textures / surfaces
    m_textureManager->load(textureConfigPath, Config::Common::dataPath);
}

void Game::loadFonts()
{
    const std::string fontConfigPath = std::string(Config::Common::dataPath) +
        QDir::separator().toAscii() + "fonts.conf";

    MCLogger().info() << "Loading font config from '" << fontConfigPath << "'..";

    // Load fonts
    m_textureFontManager->load(fontConfigPath);
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
        MCLogger().error() << "No valid race tracks found.";
        return false;
    }

    return true;
}

void Game::initScene()
{
    assert(m_stateMachine);
    assert(m_renderer);

    // Create the scene
    m_scene = new Scene(*m_stateMachine, *m_renderer, NUM_CARS);

    // Add tracks to the menu.
    for (unsigned int i = 0; i < m_trackLoader->tracks(); i++)
    {
        m_scene->trackSelectionMenu().addTrack(*m_trackLoader->track(i));
    }

    // Set the current game scene. Renderer calls render()
    // for all objects in the scene.
    m_renderer->setScene(*m_scene);
}

bool Game::init()
{
    try
    {
        // Load surfaces / textures
        loadSurfaces();

        // Load textured fonts
        loadFonts();

        // Load race tracks
        if (loadTracks())
        {
            // Init the game scene
            initScene();

            // Init succeeded
            return true;
        }
    }
    catch (MCException & e)
    {
        MCLogger().fatal() << e.what();

        // Init failed
        return false;
    }

    // Init failed
    return false;
}

void Game::start()
{
    m_paused = false;

    mainLoop();
}

void Game::stop()
{
    m_paused = true;
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
        QApplication::instance()->quit();
    }
}

void Game::exitGame()
{
    m_exit = true;
}

void Game::updateFrame()
{
    m_stateMachine->update();
    m_renderer->updateFrame(m_scene->camera());
    m_scene->updateFrame(*m_inputHandler, m_timeStep);
}

void Game::mainLoop()
{
    while (!m_paused && !m_exit)
    {
        QTime updateTime;
        updateTime.start();

        QApplication::processEvents();
        updateAnimations();
        updateFrame();

        int delay = m_updateDelay - updateTime.elapsed();
        delay = delay < 0 ? 0 : delay;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }

    if (m_exit)
    {
        // For some weird reason calling QApplication::instance()->exit()
        // didn't exit the application. It might have something to do
        // with QApplication::processEvents() above.
        ::exit(EXIT_SUCCESS);
    }
}

void Game::updateAnimations()
{
    m_scene->updateAnimations();
}

void Game::countRenderFps()
{
    m_renderCount = 0;
}

Game::~Game()
{
    delete m_stateMachine;
    delete m_scene;
    delete m_trackLoader;
    delete m_textureManager;
    delete m_objectFactory;
    delete m_eventHandler;
    delete m_inputHandler;
    delete m_settings;
}
