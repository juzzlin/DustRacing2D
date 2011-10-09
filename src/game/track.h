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

#ifndef TRACK_H
#define TRACK_H

class TrackData;
class MCCamera;

//! A renderable race track object constructed from
//! the given track data.
class Track
{
public:

    //! Constructor.
    //! \param trackData The data that represents the track.
    //!                  Track will take the ownership.
    explicit Track(TrackData * trackData);

    //! Destructor.
    ~Track();

    //! Render as seen through the given camera window.
    void render(MCCamera * camera);

private:

    TrackData * m_trackData;
};

#endif // TRACK_H
