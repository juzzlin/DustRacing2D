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
#include "inputhandler.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "statemachine.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "trackloader.hpp"
#include "trackselectionmenu.hpp"

#include "MiniCore/Core/MCCamera"
#include "MiniCore/Core/MCLogger"
#include "MiniCore/Core/MCObjectFactory"
#include "MiniCore/Core/MCTextureManager"
#include "MiniCore/Text/MCTextureFontManager"

#include <QDir>
#include <QTime>
#include <QCoreApplication>

#include <cassert>

namespace
{
static const unsigned int MAX_PLAYERS = 2;
static const unsigned int NUM_CARS    = 10;
}

Game::Game()
: m_stateMachine(new StateMachine)
, m_renderer(nullptr)
, m_scene(nullptr)
, m_textureManager(new MCTextureManager)
, m_textureFontManager(new MCTextureFontManager(*m_textureManager))
, m_objectFactory(new MCObjectFactory(*m_textureManager))
, m_trackLoader(new TrackLoader(*m_textureManager, *m_objectFactory))
, m_inputHandler(new InputHandler(MAX_PLAYERS))
, m_updateFps(60)
, m_timeStep(1.0 / m_updateFps)
, m_renderCount(0)
, m_availableRenderTime(0)
{
    connect(&m_frameUpdateTimer, SIGNAL(timeout()), this, SLOT(updateFrame()));
    connect(&m_animationUpdateTimer, SIGNAL(timeout()), this, SLOT(updateAnimations()));
    connect(&m_renderCountTimer, SIGNAL(timeout()), this, SLOT(countRenderFps()));
    m_renderCountTimer.setInterval(1000);

    m_trackLoader->addTrackSearchPath(QString(Config::Common::dataPath) +
        QDir::separator() + "levels");
}

void Game::setTargetUpdateFps(unsigned int fps)
{
    m_updateFps = fps;
    m_timeStep  = 1.0 / m_updateFps;
}

void Game::setRenderer(Renderer * newRenderer)
{
    m_renderer = newRenderer;

    // Note that this must be called before loading textures in order
    // to load textures to correct OpenGL context.
    m_renderer->makeCurrent();
    m_renderer->setInputHandler(m_inputHandler);
}

void Game::finish()
{
    disconnect(&m_frameUpdateTimer, SIGNAL(timeout()), this, SLOT(updateFrame()));
    disconnect(&m_renderCountTimer, SIGNAL(timeout()), this, SLOT(countRenderFps()));
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
        MCLogger().info() << numLoaded << " track(s) loaded.";
    }
    else
    {
        MCLogger().error() << "No valid tracks found.";
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
    m_renderer->setScene(m_scene);
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
    m_frameUpdateTimer.setInterval(1000 / m_updateFps);
    m_frameUpdateTimer.start();

    m_animationUpdateTimer.setInterval(1000 / 60); // Always 60 Hz
    m_animationUpdateTimer.start();

    m_renderCountTimer.start();
}

void Game::stop()
{
    m_frameUpdateTimer.stop();
    m_animationUpdateTimer.stop();
    m_renderCountTimer.stop();
}

void Game::updateFrame()
{
    m_stateMachine->update();
    m_renderer->updateFrame(m_scene->camera());
    m_scene->updateFrame(*m_inputHandler, m_timeStep);
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
    delete m_inputHandler;
}
