// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef OFFTRACKDETECTOR_HPP
#define OFFTRACKDETECTOR_HPP

class Car;
class Track;

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

    Car   & m_car;
    Track * m_pTrack;
};

#endif // OFFTRACKDETECTOR_HPP
