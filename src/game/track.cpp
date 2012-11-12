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

#include "track.hpp"

#include "renderer.hpp"
#include "scene.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"
#include "map.hpp"

#include <MCCamera>
#include <MCGLShaderProgram>
#include <MCSurface>
#include <MCSurfaceManager>

#include <cassert>

Track::Track(TrackData * pTrackData)
: m_pTrackData(pTrackData)
, m_rows(m_pTrackData->map().rows())
, m_cols(m_pTrackData->map().cols())
, m_width(m_cols * TrackTile::TILE_W)
, m_height(m_rows * TrackTile::TILE_H)
, m_scale(0.0)
, m_asphalt(MCSurfaceManager::instance().surface("asphalt"))
, m_grass(MCSurfaceManager::instance().surface("grass"))
, m_next(nullptr)
{
    assert(pTrackData);
}

MCUint Track::width() const
{
    return m_width;
}

MCUint Track::height() const
{
    return m_height;
}

TrackData & Track::trackData() const
{
    return *m_pTrackData;
}

TrackTile * Track::trackTileAtLocation(MCUint x, MCUint y) const
{
    // X index
    MCUint i = x * m_cols / m_width;
    i = i >= m_cols ? m_cols - 1 : i;

    // Y index
    MCUint j = y * m_rows / m_height;
    j = j >= m_rows ? m_rows - 1 : j;

    return static_cast<TrackTile *>(m_pTrackData->map().getTile(i, j));
}

TrackTile * Track::finishLine() const
{
    const MapBase & rMap = m_pTrackData->map();
    for (MCUint j = 0; j < rMap.rows(); j++)
    {
        for (MCUint i = 0; i < rMap.cols(); i++)
        {
            if (TrackTile * pTile = static_cast<TrackTile *>(rMap.getTile(i, j)))
            {
                if (pTile->tileTypeEnum() == TrackTile::TT_FINISH)
                {
                    return pTile;
                }
            }
        }
    }

    return nullptr;
}

void Track::calculateVisibleIndices(const MCBBox<int> & r,
    MCUint & i0, MCUint & i2, MCUint & j0, MCUint & j2)

{
    // Calculate which tiles are visible in the Camera window:
    // columns from i0 to i2 and rows from j0 to j2.

    // X low index
    i0 = r.x1() * m_cols / m_width;
    i0 = i0 >= m_cols ? 0 : i0;

    // X high index
    i2 = r.x2() * m_cols / m_width;
    i2 = i2  >= m_cols ? m_cols - 1 : i2;

    // Y low index
    j0 = r.y1() * m_rows / m_height;
    j0 = j0 >= m_rows ? 0 : j0;

    // Y high index
    j2 = r.y2() * m_rows / m_height;
    j2 = j2  >= m_rows ? m_rows - 1 : j2;
}

void Track::render(MCCamera * pCamera)
{
    // Get the Camera window
    MCBBox<MCFloat> cameraBox(pCamera->bbox());

    // Calculate which tiles are visible
    MCUint i2, j2, i0, j0;
    calculateVisibleIndices(cameraBox, i0, i2, j0, j2);

    MCGLShaderProgram & prog = Renderer::instance().program("tile");
    prog.bind();

    renderAsphalt(pCamera, prog, i0, i2, j0, j2);
    renderTiles(pCamera, prog, i0, i2, j0, j2);

    prog.release();
}

void Track::renderAsphalt(
    MCCamera * pCamera, MCGLShaderProgram & prog, MCUint i0, MCUint i2, MCUint j0, MCUint j2)
{
    const MapBase & rMap = m_pTrackData->map();

    static const int w = TrackTile::TILE_W;
    static const int h = TrackTile::TILE_H;

    MCFloat x1, y1; // Coordinates mapped to camera

    // Set common client state for all tiles using the same surface.
    m_asphalt.enableClientState(true, true);

    // Loop through the visible tile matrix and draw the tiles
    int initX = i0 * w;
    int x     = initX;
    int y     = j0 * h;
    for (MCUint j = j0; j <= j2; j++)
    {
        x = initX;
        for (MCUint i = i0; i <= i2; i++)
        {
            if (TrackTile * pTile = static_cast<TrackTile *>(rMap.getTile(i, j)))
            {
                if (pTile->hasAsphalt())
                {
                    x1 = x;
                    y1 = y;
                    pCamera->mapToCamera(x1, y1);
                    prog.translate(MCVector3dF(x1 + w / 2, y1 + h / 2, 0));
                    prog.rotate(0);
                    m_asphalt.renderVBOs(false);
                }
            }

            x += w;
        }

        y += h;
    }

    m_asphalt.enableClientState(false, false);
}

void Track::renderTiles(
    MCCamera * pCamera, MCGLShaderProgram & prog, MCUint i0, MCUint i2, MCUint j0, MCUint j2)
{
    const MapBase & rMap = m_pTrackData->map();

    static const int w = TrackTile::TILE_W;
    static const int h = TrackTile::TILE_H;

    MCFloat x1, y1; // Coordinates mapped to camera

    // Set common client state for all tiles using the same surface.
    static_cast<TrackTile *>(rMap.getTile(0, 0))->surface()->enableClientState(true, false);

    // Loop through the visible tile matrix and draw the tiles
    int initX = i0 * w;
    int x     = initX;
    int y     = j0 * h;
    for (MCUint j = j0; j <= j2; j++)
    {
        x = initX;
        for (MCUint i = i0; i <= i2; i++)
        {
            if (TrackTile * pTile = static_cast<TrackTile *>(rMap.getTile(i, j)))
            {
                if (MCSurface * pSurface = pTile->surface())
                {
                    x1 = x;
                    y1 = y;
                    pCamera->mapToCamera(x1, y1);
                    prog.translate(MCVector3dF(x1 + w / 2, y1 + h / 2, 0));
                    prog.rotate(pTile->rotation());
                    pSurface->bindTexture();
                    pSurface->renderVBOs(false);
                }
            }

            x += w;
        }

        y += h;
    }

    static_cast<TrackTile *>(rMap.getTile(0, 0))->surface()->enableClientState(false, false);
}

bool Track::update()
{
    // This helps to remove some glitches in tile boundaries.
    m_scale = 1.001;
    return false;
}

void Track::reset()
{
    m_scale = 0.0;
}

void Track::setNext(Track & next)
{
    m_next = &next;
}

Track * Track::next() const
{
    return m_next;
}

Track::~Track()
{
    delete m_pTrackData;
}
