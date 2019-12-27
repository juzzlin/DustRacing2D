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

#ifndef TRACK_HPP
#define TRACK_HPP

#include "tracktile.hpp"
#include "updateableif.hpp"

#include <MCBBox>
#include <MCGLShaderProgram>

#include <memory>

class TrackData;
class MCCamera;
class MCSurface;

//! A renderable race track object constructed from
//! the given track data.
class Track
{
public:
    /*! Constructor.
     * \param trackData The data that represents the track. */
    explicit Track(std::unique_ptr<TrackData> trackData);

    //! Render as seen through the given camera window.
    void render(MCCamera & camera);

    //! Return width in length units.
    size_t width() const;

    //! Return height in length units.
    size_t height() const;

    //! Return the track data.
    TrackData & trackData() const;

    //! Return pointer to the tile at the given location.
    //! It is assumed that x >= 0 and y >= 0.
    TrackTilePtr trackTileAtLocation(float x, float y) const;

    //! Return pointer to the finish line tile.
    TrackTilePtr finishLine() const;

    //! Set the next track.
    void setNext(std::weak_ptr<Track> next);

    //! Return the next track or nullptr.
    std::weak_ptr<Track> next() const;

    //! Set the prev track.
    void setPrev(std::weak_ptr<Track> prev);

    //! Return the prev track or nullptr.
    std::weak_ptr<Track> prev() const;

private:
    void calculateVisibleIndices(const MCBBox<int> & r, size_t & i0, size_t & i2, size_t & j0, size_t & j2);

    void renderAsphalt(MCCamera & camera, MCGLShaderProgramPtr prog, size_t i0, size_t i2, size_t j0, size_t j2);

    void renderTiles(MCCamera & camera, MCGLShaderProgramPtr prog, size_t i0, size_t i2, size_t j0, size_t j2);

    std::unique_ptr<TrackData> m_trackData;

    size_t m_rows, m_cols, m_width, m_height;

    std::shared_ptr<MCSurface> m_asphalt;

    std::weak_ptr<Track> m_next;

    std::weak_ptr<Track> m_prev;
};

#endif // TRACK_HPP
