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

#ifndef AILOGIC_HPP
#define AILOGIC_HPP

#include "MiniCore/Core/MCTypes"

class Car;
class Route;
class Track;
class TrackTile;

//! Class that implements the logic for artificial intelligence.
class AiLogic
{
public:

    //! Constructor.
    AiLogic(Car & car);

    //! Destructor.
    virtual ~AiLogic();

    //! Update.
    void update();

    //! Set the current race track.
    void setTrack(Track & track);

private:

    void steer(TrackTile & targetTile, TrackTile & currentTile);

    Car         & m_car;
    Track       * m_track;
    const Route * m_route;
    unsigned int  m_targetIndex;
    int           m_lastDiff;
};

#endif // AILOGIC_HPP
