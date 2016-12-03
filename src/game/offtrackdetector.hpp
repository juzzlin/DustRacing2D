// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef OFFTRACKDETECTOR_HPP
#define OFFTRACKDETECTOR_HPP

#include <MCVector2d>

class Car;
class Track;
class TrackTile;

//! Detects if a car is off the track.
class OffTrackDetector
{
public:

    //! Constructor.
    OffTrackDetector(Car & car);

    //! Set the current track.
    void setTrack(Track & track);

    //! Update.
    void update();

private:

    //! Test if the given location is off the track on the given tile.
    bool isOffTrack(MCVector2dF tire, const TrackTile & tile) const;

    Car & m_car;

    Track * m_track;

    float m_tileWLimit;

    float m_tileHLimit;
};

#endif // OFFTRACKDETECTOR_HPP
