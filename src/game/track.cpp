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

#include <MCAssetManager>
#include <MCCamera>
#include <MCGLShaderProgram>
#include <MCSurface>

#include <cassert>
#include <memory>

using std::static_pointer_cast;

Track::Track(TrackData * trackData)
: m_trackData(trackData)
, m_rows(m_trackData->map().rows())
, m_cols(m_trackData->map().cols())
, m_width(m_cols * TrackTile::TILE_W)
, m_height(m_rows * TrackTile::TILE_H)
, m_asphalt(MCAssetManager::surfaceManager().surface("asphalt"))
, m_next(nullptr)
, m_prev(nullptr)
{
    assert(trackData);
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
    return *m_trackData;
}

TrackTilePtr Track::trackTileAtLocation(MCUint x, MCUint y) const
{
    // X index
    MCUint i = x * m_cols / m_width;
    i = i >= m_cols ? m_cols - 1 : i;

    // Y index
    MCUint j = y * m_rows / m_height;
    j = j >= m_rows ? m_rows - 1 : j;

    return static_pointer_cast<TrackTile>(m_trackData->map().getTile(i, j));
}

TrackTilePtr Track::finishLine() const
{
    const MapBase & map = m_trackData->map();
    for (MCUint j = 0; j < map.rows(); j++)
    {
        for (MCUint i = 0; i < map.cols(); i++)
        {
            TrackTilePtr pTile = static_pointer_cast<TrackTile>(map.getTile(i, j));
            if (pTile->tileTypeEnum() == TrackTile::TT_FINISH)
            {
                return pTile;
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

void Track::render(MCCamera * camera)
{
    // Get the Camera window
    MCBBox<MCFloat> cameraBox(camera->bbox());

    // Calculate which tiles are visible
    MCUint i2, j2, i0, j0;
    calculateVisibleIndices(cameraBox, i0, i2, j0, j2);

    MCGLShaderProgramPtr prog2d = Renderer::instance().program("tile2d");
    prog2d->bind();
    renderAsphalt(camera, prog2d, i0, i2, j0, j2);
    prog2d->release();

    MCGLShaderProgramPtr prog3d = Renderer::instance().program("tile3d");
    prog3d->bind();
    renderTiles(camera, prog3d, i0, i2, j0, j2);
    prog3d->release();
}

void Track::renderAsphalt(
    MCCamera * camera, MCGLShaderProgramPtr prog, MCUint i0, MCUint i2, MCUint j0, MCUint j2)
{
    MCFloat x1, y1; // Coordinates mapped to camera

    // Bind common geometry and textures for all asphalt tiles.
    m_asphalt.setShaderProgram(prog);
    m_asphalt.bind();

    const MapBase & map = m_trackData->map();

    // Loop through the visible tile matrix and draw the tiles
    int initX = i0 * TrackTile::TILE_W;
    int x     = initX;
    int y     = j0 * TrackTile::TILE_H;
    for (MCUint j = j0; j <= j2; j++)
    {
        x = initX;
        for (MCUint i = i0; i <= i2; i++)
        {
            auto tile = static_pointer_cast<TrackTile>(map.getTile(i, j));

            if (tile->hasAsphalt())
            {
                x1 = x;
                y1 = y;
                camera->mapToCamera(x1, y1);
                prog->setTransform(0, MCVector3dF(x1 + TrackTile::TILE_W / 2, y1 + TrackTile::TILE_H / 2, 0));
                m_asphalt.render();
            }

            x += TrackTile::TILE_W;
        }

        y += TrackTile::TILE_H;
    }
}

void Track::renderTiles(
    MCCamera * camera, MCGLShaderProgramPtr prog, MCUint i0, MCUint i2, MCUint j0, MCUint j2)
{
    MCFloat x1, y1; // Coordinates mapped to camera

    struct SortedTile
    {
        TrackTile * tile;
        MCFloat x1, y1;
    };

    // The tiles are sorted with respect to their surface in order
    // to minimize GPU context switches.
    std::map<MCSurface *, std::vector<SortedTile> > sortedTiles;

    const MapBase & map = m_trackData->map();

    // Loop through the visible tile matrix and sort the tiles.
    int initX = i0 * TrackTile::TILE_W;
    int x     = initX;
    int y     = j0 * TrackTile::TILE_H;
    for (MCUint j = j0; j <= j2; j++)
    {
        x = initX;
        for (MCUint i = i0; i <= i2; i++)
        {
            auto tile = static_pointer_cast<TrackTile>(map.getTile(i, j));
            if (MCSurface * surface = tile->surface())
            {
                x1 = x;
                y1 = y;
                camera->mapToCamera(x1, y1);

                SortedTile sortedTile;
                sortedTile.tile = tile.get();
                sortedTile.x1 = x1;
                sortedTile.y1 = y1;
                sortedTiles[surface].push_back(sortedTile);
            }

            x += TrackTile::TILE_W;
        }

        y += TrackTile::TILE_H;
    }

    // Render the tiles.
    auto iter = sortedTiles.begin();
    while (iter != sortedTiles.end())
    {
        MCSurface * surface = iter->first;
        surface->setShaderProgram(prog);
        surface->bind();

        for (unsigned int i = 0; i < iter->second.size(); i++)
        {
            x1 = iter->second[i].x1;
            y1 = iter->second[i].y1;

            const TrackTile * tile = iter->second[i].tile;
            prog->setTransform(tile->rotation(), MCVector3dF(x1 + TrackTile::TILE_W / 2, y1 + TrackTile::TILE_H / 2, 0));
            prog->setScale(TrackTile::TILE_W / surface->width(), TrackTile::TILE_H / surface->height(), 1.0f);
            surface->render();
        }

        iter++;
    }
}

void Track::setNext(Track & next)
{
    m_next = &next;
}

Track * Track::next() const
{
    return m_next;
}

void Track::setPrev(Track & prev)
{
    m_prev = &prev;
}

Track * Track::prev() const
{
    return m_prev;
}

Track::~Track()
{
    delete m_trackData;
}
