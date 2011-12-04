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

#ifndef RACE_H
#define RACE_H

#include <unordered_map>
#include <vector>

class Car;
class Track;
class TrackTile;

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

    //! Update situation.
    void update();

    //! Set the current race track.
    void setTrack(Track * pTrack);

    //! Set the number of laps.
    void setLapCount(unsigned int laps);

    //! Add a car to the race.
    void addCar(Car * pCar);

private:

    void checkRoute(Car * pCar);

    typedef std::vector<Car *> CarVector;
    CarVector m_cars;

    typedef std::unordered_map<Car *, int> RouteHash;
    RouteHash m_routeHash;

    unsigned int m_lapCount;

    Track * m_pTrack;
};

#endif // RACE_H
