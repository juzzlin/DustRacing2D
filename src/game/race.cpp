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

#include "race.hpp"

#include "car.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"

#include <algorithm>
#include <cassert>

static const unsigned int MAX_CARS = 24;

Race::Race()
  : m_lapCount(0)
  , m_timing(MAX_CARS)
  , m_pTrack(nullptr)
  , m_started(false)
  , m_checkeredFlagEnabled(false)
{
}

void Race::init()
{
    for(Car * pCar : m_cars)
    {
        m_routeHash[pCar->index()] = 0;
    }

    m_positions.clear();
}

void Race::start()
{
    m_timing.start();
    m_started = true;
}

bool Race::started()
{
    return m_started;
}

void Race::update()
{
    for(Car * pCar : m_cars)
    {
        checkRoute(*pCar);
    }

    // Enable the checkered flag if leader has done at least 95% of the last lap.
    if (m_timing.leadersLap() + 1 == static_cast<int>(m_lapCount))
    {
        Car & leader = getLeadingCar();
        const TrackTile * pCurrent = m_pTrack->trackTileAtLocation(leader.getX(), leader.getY());
        if (pCurrent->routeIndex() != -1 &&
            pCurrent->routeIndex() > 9 * m_pTrack->trackData().route().length() / 10)
        {
            m_checkeredFlagEnabled = true;
        }
    }
}

void Race::checkRoute(Car & car)
{
    assert(m_pTrack);

    const TrackTile * pCurrent = m_pTrack->trackTileAtLocation(car.getX(), car.getY());

    // Lap progressed?
    if (pCurrent->routeIndex() == m_routeHash[car.index()] + 1)
    {
        m_routeHash[car.index()] = pCurrent->routeIndex();
        m_positions[pCurrent->routeIndex()][m_timing.lap(car.index())].push_back(car.index());
    }
    // Lap finished?
    else if (m_routeHash[car.index()] ==
        static_cast<int>(m_pTrack->trackData().route().length()) - 1)
    {
        if (pCurrent->routeIndex() == 1)
        {
            m_routeHash[car.index()] = 1;
            m_positions[pCurrent->routeIndex()][m_timing.lap(car.index())].push_back(car.index());
            m_timing.lapCompleted(car.index());
        }
    }
}

unsigned int Race::getPositionOfCar(int index) const
{
    const int currentTile = m_routeHash[index];
    const std::vector<int> & order = m_positions[currentTile][m_timing.leadersLap()];
    for (unsigned int i = 0; i < order.size(); i++)
    {
        if (order[i] == index)
        {
            return i + 1;
        }
    }

    return 0; // Should return the last valid position?
}

Car & Race::getLeadingCar() const
{
    unsigned int bestPos = m_cars.size();
    Car        * bestCar = m_cars.back();

    for (unsigned int i = 0; i < m_cars.size(); i++)
    {
        const unsigned int pos = getPositionOfCar(i);
        if (pos > 0 && pos < bestPos)
        {
            bestCar = m_cars.at(i);
            bestPos = pos;
        }
    }

    return *bestCar;
}

void Race::setTrack(Track & track)
{
    m_pTrack = &track;
}

void Race::setLapCount(unsigned int laps)
{
    m_lapCount = laps;
}

unsigned int Race::lapCount() const
{
    return m_lapCount;
}

void Race::addCar(Car & car)
{
    if (find(m_cars.begin(), m_cars.end(), &car) == m_cars.end())
    {
        m_cars.push_back(&car);
    }
}

Timing & Race::timing()
{
    return m_timing;
}

bool Race::checkeredFlagEnabled() const
{
    return m_checkeredFlagEnabled;
}

Race::~Race()
{
}
