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

#ifndef RACE_HPP
#define RACE_HPP

#include <unordered_map>
#include <vector>

#include "timing.hpp"

class Car;
class Track;

//! Class that controls the race event.
class Race
{
public:

    //! Constructor.
    Race();

    //! Destructor.
    virtual ~Race();

    //! Init the race.
    void init();

    //! Start the race and timing.
    void start();

    //! Return true, if race has started.
    bool started();

    //! Update situation.
    void update();

    //! Set the current race track.
    void setTrack(Track & track);

    //! Set the number of laps.
    void setLapCount(unsigned int laps);

    //! Get the position of the given car (0 == N/A, 1 == first, 2 == second..).
    unsigned int getPositionOfCar(int index) const;

    //! Get the number of laps.
    unsigned int lapCount() const;

    //! Add a car to the race.
    void addCar(Car & car);

    //! Get the timing object.
    Timing & timing();

private:

    void checkRoute(Car & car);

    typedef std::vector<Car *> CarVector;
    CarVector m_cars;

    // Map from car index to tile index.
    // Tracks how cars are progressing on the route / lap.
    typedef std::unordered_map<int, int> RouteHash;
    mutable RouteHash m_routeHash;

    // Map from tile index to lap index to car order vector.
    // Tracks the order of the cars in the route.
    typedef std::unordered_map<int, std::vector<int> > LapHash;
    typedef std::unordered_map<int, LapHash> PositionHash;
    mutable PositionHash m_positions;

    unsigned int m_lapCount;

    Timing m_timing;

    Track * m_pTrack;

    bool m_started;
};

#endif // RACE_HPP
