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
#include "startlights.hpp"
#include "startlightsoverlay.hpp"
#include "timingoverlay.hpp"
#include "track.hpp"
#include "trackloader.hpp"

#include "MiniCore/Core/MCCamera"
#include "MiniCore/Core/MCLogger"
#include "MiniCore/Core/MCObjectFactory"
#include "MiniCore/Core/MCTextureManager"
#include "MiniCore/Text/MCTextureFontManager"

#include <QDir>
#include <QTime>

#include <cassert>

namespace
{
static const unsigned int MAX_PLAYERS = 2;
static const unsigned int NUM_CARS    = 10;
}

Game::Game()
: m_pRenderer(nullptr)
, m_pScene(nullptr)
, m_pTextureManager(new MCTextureManager)
, m_pTextureFontManager(new MCTextureFontManager(*m_pTextureManager))
, m_pObjectFactory(new MCObjectFactory(*m_pTextureManager))
, m_pTrackLoader(new TrackLoader(*m_pTextureManager, *m_pObjectFactory))
, m_pTimingOverlay(nullptr)
, m_pCamera(nullptr)
, m_pInputHandler(new InputHandler(MAX_PLAYERS))
, m_updateFps(60)
, m_timeStep(1.0f / m_updateFps)
, m_renderCount(0)
, m_availableRenderTime(0)
{
    connect(&m_frameUpdateTimer, SIGNAL(timeout()), this, SLOT(updateFrame()));
    connect(&m_animationUpdateTimer, SIGNAL(timeout()), this, SLOT(updateAnimations()));
    connect(&m_renderCountTimer, SIGNAL(timeout()), this, SLOT(countRenderFps()));
    m_renderCountTimer.setInterval(1000);

    m_pTrackLoader->addTrackSearchPath(QString(Config::Common::dataPath) +
        QDir::separator() + "levels");
}

void Game::setTargetUpdateFps(unsigned int fps)
{
    m_updateFps = fps;
    m_timeStep  = 1.0f / m_updateFps;
}

void Game::setRenderer(Renderer * newRenderer)
{
    m_pRenderer = newRenderer;

    // Note that this must be called before loading textures in order
    // to load textures to correct OpenGL context.
    m_pRenderer->makeCurrent();
    m_pRenderer->setInputHandler(m_pInputHandler);
}

void Game::finish()
{
    disconnect(&m_frameUpdateTimer, SIGNAL(timeout()), this, SLOT(updateFrame()));
    disconnect(&m_renderCountTimer, SIGNAL(timeout()), this, SLOT(countRenderFps()));
}

Renderer * Game::renderer() const
{
    return m_pRenderer;
}

void Game::loadSurfaces()
{
    // Load texture data
    const std::string textureConfigPath = std::string(Config::Common::dataPath) +
        QDir::separator().toAscii() + "textures.conf";

    MCLogger().info() << "Loading texture config from '" << textureConfigPath << "'..";

    // Load textures / surfaces
    m_pTextureManager->load(textureConfigPath, Config::Common::dataPath);
}

void Game::loadFonts()
{
    const std::string fontConfigPath = std::string(Config::Common::dataPath) +
        QDir::separator().toAscii() + "fonts.conf";

    MCLogger().info() << "Loading font config from '" << fontConfigPath << "'..";

    // Load fonts
    m_pTextureFontManager->load(fontConfigPath);
}

bool Game::loadTracks()
{
    // Load track data
    if (int numLoaded = m_pTrackLoader->loadTracks())
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
    assert(m_pRenderer);

    // Create the scene
    m_pScene = new Scene(*m_pRenderer, NUM_CARS);

    // Set the default track
    m_pScene->setActiveTrack(*m_pTrackLoader->track(0));

    // Set the current game scene. Renderer calls render()
    // for all objects in the scene.
    m_pRenderer->setScene(m_pScene);

    m_pCamera = new MCCamera(
        Scene::width(), Scene::height(),
        0, 0,
        m_pScene->activeTrack().width(),
        m_pScene->activeTrack().height());

    m_pTimingOverlay = new TimingOverlay;
    m_pTimingOverlay->setDimensions(
        Scene::width(), Scene::height());
    m_pScene->setTimingOverlay(*m_pTimingOverlay);
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
    m_pRenderer->updateFrame(m_pCamera);
    m_pScene->updateFrame(*m_pInputHandler, *m_pCamera, m_timeStep);
}

void Game::updateAnimations()
{
    m_pScene->updateAnimations();
}

void Game::countRenderFps()
{
    m_renderCount = 0;
}

Game::~Game()
{
    delete m_pScene;
    delete m_pTrackLoader;
    delete m_pTextureManager;
    delete m_pObjectFactory;
    delete m_pCamera;
    delete m_pInputHandler;
    delete m_pTimingOverlay;
}
