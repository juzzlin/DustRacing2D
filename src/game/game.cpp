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

#include "../common/config.h"
#include "game.h"
#include "inputhandler.h"
#include "scene.h"
#include "renderer.h"
#include "track.h"
#include "trackloader.h"
#include "MiniCore/Core/MCCamera"
#include "MiniCore/Core/MCLogger"
#include "MiniCore/Util/MCTextureManager"
#include <QDir>
#include <QTime>

Game::Game()
: m_pRenderer(nullptr)
, m_pScene(nullptr)
, m_pTextureManager(new MCTextureManager)
, m_pTrackLoader(new TrackLoader(m_pTextureManager))
, m_pCamera(nullptr)
, m_pInputHandler(new InputHandler)
, m_updateTimer()
, m_renderTimer()
, m_updateFps(30)
, m_timeStep(1.0f / m_updateFps)
, m_renderCount(0)
, m_availableRenderTime(0)
{
    connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(updateFrame()));
    connect(&m_renderTimer, SIGNAL(timeout()), this, SLOT(countRenderFps()));
    m_renderTimer.setInterval(1000);

    m_pTrackLoader->addTrackSearchPath(QString(Config::Common::DATA_PATH) +
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
    disconnect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(updateFrame()));
    disconnect(&m_renderTimer, SIGNAL(timeout()), this, SLOT(countRenderFps()));
}

Renderer * Game::renderer() const
{
    return m_pRenderer;
}

bool Game::init()
{
    try
    {
        // Load texture data
        QString textureConfigPath = QString(Config::Common::DATA_PATH) +
            QDir::separator() + "textures.conf";

        MCLogger::logInfo("Loading texture config from %s..",
            textureConfigPath.toStdString().c_str());

        m_pTextureManager->load(textureConfigPath,
            Config::Common::DATA_PATH);

        // Load track data
        if (int numLoaded = m_pTrackLoader->loadTracks())
        {
            MCLogger::logInfo("%d track(s) loaded.", numLoaded);
        }
        else
        {
            MCLogger::logError("No valid tracks found.");
            return false;
        }

        // Create the scene
        m_pScene = new Scene(m_pTextureManager->surface("car001"));

        // Set the default track
        m_pScene->setActiveTrack(m_pTrackLoader->track(0));

        // Set the current game scene. Renderer calls render()
        // for all objects in the scene.
        m_pRenderer->setScene(m_pScene);

        m_pCamera = new MCCamera(
            Config::Game::WINDOW_WIDTH, Config::Game::WINDOW_HEIGHT,
            0, 0,
            m_pScene->activeTrack()->width(),
            m_pScene->activeTrack()->height());
    }
    catch (MCException & e)
    {
        MCLogger::logFatal("%s.", e.what());
        return false;
    }

    return true;
}

void Game::start()
{
    m_updateTimer.setInterval(1000 / m_updateFps);
    m_updateTimer.start();

    m_renderTimer.start();
}

void Game::stop()
{
    m_updateTimer.stop();
    m_renderTimer.stop();
}

void Game::updateFrame()
{
    static int renderTime = 0;
    static QTime updateTimer;
    updateTimer.start();

    m_pScene->updateFrame(m_pInputHandler, m_pCamera, m_timeStep);

    const int updateTime      = updateTimer.elapsed();
    const int availableTimeMs = 1000 / m_updateFps;
    if (m_availableRenderTime >= renderTime)
    {
        static QTime renderTimer;
        renderTimer.start();
        renderFrame();
        renderTime = (renderTimer.elapsed() + renderTime) / 2;
        m_renderCount++;

        m_availableRenderTime -= renderTime;
    }
    else
    {
        m_availableRenderTime += availableTimeMs - updateTime;
    }
}

void Game::renderFrame()
{
    if (m_pRenderer)
    {
        m_pRenderer->updateFrame(m_pCamera);
    }
}

void Game::countRenderFps()
{
    //std::cout << m_renderCount << std::endl;
    m_renderCount = 0;
}

Game::~Game()
{
    delete m_pScene;
    delete m_pTrackLoader;
    delete m_pTextureManager;
    delete m_pCamera;
    delete m_pInputHandler;
}
