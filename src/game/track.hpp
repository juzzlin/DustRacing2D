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

#include "updateableif.hpp"

#include <MCBBox>
#include <MCTypes>

class TrackData;
class TrackTile;
class MCCamera;
class MCGLShaderProgram;
class MCSurface;

//! A renderable race track object constructed from
//! the given track data.
class Track : public UpdateableIf
{
public:

    //! Constructor.
    //! \param pTrackData The data that represents the track.
    //!                   Track will take the ownership.
    explicit Track(TrackData * pTrackData);

    //! Destructor.
    virtual ~Track();

    //! Render as seen through the given camera window.
    void render(MCCamera * pCamera);

    //! Return width in pixels.
    MCUint width() const;

    //! Return height in pixels.
    MCUint height() const;

    //! Return the track data.
    TrackData & trackData() const;

    //! Return pointer to the tile at the given location.
    TrackTile * trackTileAtLocation(MCUint x, MCUint y) const;

    //! Return pointer to the finish line tile.
    TrackTile * finishLine() const;

    //! \reimp
    virtual bool update();

    //! \reimp
    virtual void reset();

    //! Set the next track.
    void setNext(Track & next);

    //! Return the next track or nullptr.
    Track * next() const;

private:

    void calculateVisibleIndices(const MCBBox<int> & r,
        MCUint & i0, MCUint & i2, MCUint & j0, MCUint & j2);

    void renderAsphalt(
        MCCamera * pCamera, MCGLShaderProgram & prog, MCUint i0, MCUint i2, MCUint j0, MCUint j2);

    void renderTiles(
        MCCamera * pCamera, MCGLShaderProgram & prog, MCUint i0, MCUint i2, MCUint j0, MCUint j2);

    TrackData * m_pTrackData;
    MCUint      m_rows, m_cols, m_width, m_height;
    MCFloat     m_scale;
    MCSurface & m_asphalt;
    Track     * m_next;
};

#endif // TRACK_HPP
