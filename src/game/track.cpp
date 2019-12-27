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

#include "map.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"

#include <MCAssetManager>
#include <MCCamera>
#include <MCGLShaderProgram>
#include <MCSurface>

#include <cassert>
#include <memory>

using std::static_pointer_cast;

Track::Track(std::unique_ptr<TrackData> trackData)
  : m_trackData(std::move(trackData))
  , m_rows(m_trackData->map().rows())
  , m_cols(m_trackData->map().cols())
  , m_width(m_cols * TrackTile::width())
  , m_height(m_rows * TrackTile::height())
  , m_asphalt(MCAssetManager::surfaceManager().surface("asphalt"))
{
    assert(m_trackData);
}

size_t Track::width() const
{
    return m_width;
}

size_t Track::height() const
{
    return m_height;
}

TrackData & Track::trackData() const
{
    return *m_trackData;
}

TrackTilePtr Track::trackTileAtLocation(float x, float y) const
{
    // X index
    x = x < 0 ? 0 : x;
    size_t i = static_cast<size_t>(x * m_cols / m_width);
    i = i >= m_cols ? m_cols - 1 : i;

    // Y index
    y = y < 0 ? 0 : y;
    size_t j = static_cast<size_t>(y * m_rows / m_height);
    j = j >= m_rows ? m_rows - 1 : j;

    return static_pointer_cast<TrackTile>(m_trackData->map().getTile(i, j));
}

TrackTilePtr Track::finishLine() const
{
    auto && map = m_trackData->map();
    for (size_t j = 0; j < map.rows(); j++)
    {
        for (size_t i = 0; i < map.cols(); i++)
        {
            const auto tile = static_pointer_cast<TrackTile>(map.getTile(i, j));
            if (tile->tileTypeEnum() == TrackTile::TileType::Finish)
            {
                return tile;
            }
        }
    }

    return nullptr;
}

void Track::calculateVisibleIndices(const MCBBox<int> & r, size_t & i0, size_t & i2, size_t & j0, size_t & j2)

{
    // Calculate which tiles are visible in the Camera window:
    // columns from i0 to i2 and rows from j0 to j2.

    // X low index
    i0 = r.x1() * m_cols / m_width;
    i0 = i0 >= m_cols ? 0 : i0;

    // X high index
    i2 = r.x2() * m_cols / m_width;
    i2 = i2 >= m_cols ? m_cols - 1 : i2;

    // Y low index
    j0 = r.y1() * m_rows / m_height;
    j0 = j0 >= m_rows ? 0 : j0;

    // Y high index
    j2 = r.y2() * m_rows / m_height;
    j2 = j2 >= m_rows ? m_rows - 1 : j2;
}

void Track::render(MCCamera & camera)
{
    // Calculate which tiles are visible
    size_t i2, j2, i0, j0;
    calculateVisibleIndices(camera.bbox(), i0, i2, j0, j2);

    MCGLShaderProgramPtr prog2d = Renderer::instance().program("tile2d");
    prog2d->bind();
    renderAsphalt(camera, prog2d, i0, i2, j0, j2);
    prog2d->release();

    MCGLShaderProgramPtr prog3d = Renderer::instance().program("tile3d");
    prog3d->bind();
    renderTiles(camera, prog3d, i0, i2, j0, j2);
    prog3d->release();
}

void Track::renderAsphalt(MCCamera & camera, MCGLShaderProgramPtr prog, size_t i0, size_t i2, size_t j0, size_t j2)
{
    float x1, y1; // Coordinates mapped to camera

    // Bind common geometry and textures for all asphalt tiles.
    m_asphalt->setShaderProgram(prog);
    m_asphalt->bind();

    const MapBase & map = m_trackData->map();

    // Loop through the visible tile matrix and draw the tiles
    size_t x;
    size_t y = j0 * TrackTile::height();
    const size_t initX = i0 * TrackTile::width();
    for (size_t j = j0; j <= j2; j++)
    {
        x = initX;
        for (size_t i = i0; i <= i2; i++)
        {
            const auto tile = static_pointer_cast<TrackTile>(map.getTile(i, j));
            if (tile->hasAsphalt())
            {
                x1 = x;
                y1 = y;
                camera.mapToCamera(x1, y1);
                prog->setTransform(0, MCVector3dF(x1 + TrackTile::width() / 2, y1 + TrackTile::height() / 2, 0));
                m_asphalt->render();
            }

            x += TrackTile::width();
        }

        y += TrackTile::height();
    }
}

void Track::renderTiles(MCCamera & camera, MCGLShaderProgramPtr prog, size_t i0, size_t i2, size_t j0, size_t j2)
{
    float x1, y1; // Coordinates mapped to camera

    struct SortedTile
    {
        TrackTile * tile;
        float x1, y1;
    };

    // The tiles are sorted with respect to their surface in order
    // to minimize GPU context switches.
    std::map<MCSurfacePtr, std::vector<SortedTile>> sortedTiles;

    auto && map = m_trackData->map();

    // Loop through the visible tile matrix and sort the tiles.
    size_t initX = i0 * TrackTile::width();
    size_t x;
    size_t y = j0 * TrackTile::height();
    for (size_t j = j0; j <= j2; j++)
    {
        x = initX;
        for (size_t i = i0; i <= i2; i++)
        {
            const auto tile = static_pointer_cast<TrackTile>(map.getTile(i, j));
            if (auto surface = tile->surface())
            {
                x1 = x;
                y1 = y;
                camera.mapToCamera(x1, y1);

                sortedTiles[surface].push_back({ tile.get(), x1, y1 });
            }

            x += TrackTile::width();
        }

        y += TrackTile::height();
    }

    // Render the tiles.
    for (auto && iter : sortedTiles)
    {
        const auto surface = iter.first;
        surface->setShaderProgram(prog);
        surface->bind();

        for (size_t i = 0; i < iter.second.size(); i++)
        {
            x1 = iter.second[i].x1;
            y1 = iter.second[i].y1;

            const auto tile = iter.second[i].tile;
            prog->setTransform(tile->rotation(), MCVector3dF(x1 + TrackTile::width() / 2, y1 + TrackTile::height() / 2, 0));
            prog->setScale(TrackTile::width() / surface->width(), TrackTile::height() / surface->height(), 1.0f);
            surface->render();
        }
    }
}

void Track::setNext(std::weak_ptr<Track> next)
{
    m_next = next;
}

std::weak_ptr<Track> Track::next() const
{
    return m_next;
}

void Track::setPrev(std::weak_ptr<Track> prev)
{
    m_prev = prev;
}

std::weak_ptr<Track> Track::prev() const
{
    return m_prev;
}
