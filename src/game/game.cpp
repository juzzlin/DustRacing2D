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
#include "scene.h"
#include "renderer.h"
#include "trackloader.h"
#include "MiniCore/Core/MCLogger"
#include "MiniCore/Core/MCTextureManager"
#include <QDir>

Game::Game()
: m_scene(nullptr)
, m_renderer(nullptr)
, m_textureManager(new MCTextureManager())
, m_trackLoader(new TrackLoader(m_textureManager))
, m_timer()
, m_targetFps(30)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateFrame()));

    m_trackLoader->addTrackSearchPath(QString(Config::Common::DATA_PATH) +
                                      QDir::separator() + "levels");
}

void Game::setTargetFps(unsigned int fps)
{
    m_targetFps = fps;
}

void Game::setRenderer(Renderer * newRenderer)
{
    m_renderer = newRenderer;
}

Renderer * Game::renderer() const
{
    return m_renderer;
}

bool Game::init()
{
    if (int numLoaded = m_trackLoader->loadTracks())
    {
        MCLogger::logInfo("%d track(s) loaded.", numLoaded);
    }
    else
    {
        MCLogger::logError("No valid tracks found.");
        return false;
    }

    return true;
}

void Game::start()
{

    m_timer.setInterval(1000 / m_targetFps);
    m_timer.start();
}

void Game::stop()
{
    m_timer.stop();
}

void Game::updateFrame()
{
    m_scene->updateFrame();

    if (m_renderer)
    {
        m_renderer->updateFrame();
    }
}

Game::~Game()
{
    delete m_trackLoader;
    delete m_textureManager;
}
