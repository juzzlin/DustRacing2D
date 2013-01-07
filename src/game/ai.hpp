// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef AI_HPP
#define AI_HPP

#include <MCVector2d>

class Car;
class Route;
class TargetNodeBase;
class Track;
class TrackTile;

//! Class that implements the artificial intelligence of the computer players.
class AI
{
public:

    //! Constructor.
    AI(Car & car);

    //! Update.
    void update(bool isRaceCompleted);

    //! Set the current race track.
    void setTrack(Track & track);

    //! Get associated car.
    Car & car() const;

private:

    //! Steering logic.
    void steerControl(TargetNodeBase & tnode);

    //! Brake/accelerate logic.
    void speedControl(TrackTile & currentTile, bool isRaceCompleted);

    void setRandomTolerance();

    Car         & m_car;
    Track       * m_track;
    const Route * m_route;
    unsigned int  m_targetIndex;
    int           m_lastDiff;
    int           m_lastTargetNodeIndex;
    MCVector2dF   m_randomTolerance;
};

#endif // AI_HPP
