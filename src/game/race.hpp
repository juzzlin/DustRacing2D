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
    Race(unsigned int numCars);

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

    //! Get the position of the given car (0 == N/A, 1 == first, 2 == second..).
    unsigned int getPositionOfCar(const Car & car) const;

    //! Get the number of laps.
    unsigned int lapCount() const;

    //! Add a car to the race.
    void addCar(Car & car);

    //! Get the timing object.
    Timing & timing();

    bool checkeredFlagEnabled() const;

    Car & getLeadingCar() const;

private:

    void updateRouteProgress(Car & car);
    void saveLapRecord(int msecs);
    int loadLapRecord() const;

    typedef std::vector<Car *> CarVector;
    CarVector m_cars;

    // Map from car route progression to car order vector.
    // Tracks the order of the cars in the route.
    typedef std::unordered_map<int, std::vector<int> > PositionHash;
    mutable PositionHash m_positions;

    unsigned int m_lapCount;

    Timing m_timing;

    Track * m_pTrack;

    bool m_started;

    bool m_checkeredFlagEnabled;

    bool m_winnerFinished;
};

#endif // RACE_HPP
