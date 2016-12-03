// This file is part of Dust Racing 2D.
// Copyright (C) 2017 Jussi Lind <jussi.lind@iki.fi>
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

#include "minimap.hpp"

#include "game.hpp"
#include "graphicsfactory.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "tracktile.hpp"

#include <MCSurface>

#include "../common/mapbase.hpp"

#include <memory>

Minimap::Minimap()
    : m_markerSurface(&GraphicsFactory::generateMinimapMarker())
{
    m_markerSurface->setShaderProgram(Renderer::instance().program("default"));
    m_markerSurface->setAlphaBlend(true);
}

Minimap::Minimap(Car & carToFollow, const MapBase & trackMap, int x, int y, int size)
{
    initialize(carToFollow, trackMap, x, y, size);
}

void Minimap::initialize(Car & carToFollow, const MapBase & trackMap, int x, int y, int size)
{
    m_carToFollow = &carToFollow;

    m_center = MCVector3dF(x, y);

    // Set m_tileW and m_tileH so that they are squares
    m_tileW = size / trackMap.cols();
    m_tileH = size / trackMap.rows();

    if (m_tileW > m_tileH)
    {
        m_tileW = m_tileH;
    }
    else
    {
        m_tileH = m_tileW;
    }

    float initX, initY;

    // Center the map
    if (trackMap.cols() % 2 == 0)
    {
        initX = x - trackMap.cols() * m_tileW / 2 + m_tileW / 4;
    }
    else
    {
        initX = x - trackMap.cols() * m_tileW / 2;
    }

    initY = y - trackMap.rows() * m_tileH / 2;

    m_map.clear();

    // Loop through the visible tile matrix and store relevant tiles
    float tileX, tileY;
    tileY = initY;
    for (unsigned int j = 0; j < trackMap.rows(); j++)
    {
        tileX = initX;
        for (unsigned int i = 0; i < trackMap.cols(); i++)
        {
            auto tile = std::static_pointer_cast<TrackTile>(trackMap.getTile(i, j));
            auto surface = tile->previewSurface();
            if (surface && !tile->excludeFromMinimap())
            {
                surface->setShaderProgram(Renderer::instance().program("default"));
                surface->setColor(MCGLColor(1.0, 1.0, 1.0));
                surface->setSize(m_tileH, m_tileW);

                MinimapTile minimapTile;
                minimapTile.pos = MCVector3dF(tileX + m_tileW / 2, tileY + m_tileH / 2);
                minimapTile.rotation = tile->rotation();

                m_map[surface].push_back(minimapTile);
            }

            tileX += m_tileW;
        }

        tileY += m_tileH;
    }

    m_sceneW = trackMap.cols() * TrackTile::TILE_W;
    m_sceneH = trackMap.rows() * TrackTile::TILE_H;

    m_size = MCVector3dF(trackMap.cols() * m_tileW, trackMap.rows() * m_tileH);
}

void Minimap::renderMap()
{
    for (auto && i : m_map)
    {
        auto surface = i.first;
        surface->bind();
        surface->setColor(MCGLColor(1.0, 1.0, 1.0));
        surface->setSize(m_tileH, m_tileW);

        for (auto && j : i.second)
        {
            const MinimapTile minimapTile = j;
            surface->render(nullptr, minimapTile.pos, minimapTile.rotation);
        }
    }
}

void Minimap::renderMarkers(const Minimap::CarVector & cars, const Race & race)
{
    m_markerSurface->setSize(m_tileH * 0.75f, m_tileW * 0.75f);

    auto && leader = race.getLeader();

    auto && loser = race.getLoser();

    for (auto && car : cars)
    {
        if (car.get() == m_carToFollow)
        {
            const auto yellow = MCGLColor(0.9f, 0.9f, 0.1f, 0.8f);
            m_markerSurface->setColor(yellow);
        }
        else if (car.get() == &leader)
        {
            const auto green = MCGLColor(0.1f, 0.9f, 0.1f, 0.8f);
            m_markerSurface->setColor(green);
        }
        else if (car.get() == &loser)
        {
            const auto red = MCGLColor(0.9f, 0.1f, 0.1f, 0.8f);
            m_markerSurface->setColor(red);
        }
        else
        {
            const auto gray = MCGLColor(0.2f, 0.2f, 0.2f, 0.8f);
            m_markerSurface->setColor(gray);
        }

        m_markerSurface->render(nullptr, m_center + car->location() * m_size.i() / m_sceneW - m_size * 0.5f, 0);
    }
}

void Minimap::render(const Minimap::CarVector & cars, const Race & race)
{
    renderMap();

    renderMarkers(cars, race);
}
