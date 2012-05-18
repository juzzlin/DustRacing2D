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

Race::Race(unsigned int numCars)
  : m_lapCount(0)
  , m_timing(numCars)
  , m_pTrack(nullptr)
  , m_started(false)
  , m_checkeredFlagEnabled(false)
  , m_winnerFinished(false)
{
}

void Race::init()
{
    for(Car * pCar : m_cars)
    {
        pCar->setCurrentRouteIndex(0);
        pCar->setRouteProgression(0);
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
        updateRouteProgress(*pCar);
    }

    // Enable the checkered flag if leader has done at least 95% of the last lap.
    if (m_timing.leadersLap() + 1 == static_cast<int>(m_lapCount))
    {
        Car & leader = getLeadingCar();
        const TrackTile * pCurrent = m_pTrack->trackTileAtLocation(leader.getX(), leader.getY());
        if (pCurrent->routeIndex() != -1 &&
            pCurrent->routeIndex() >
            static_cast<int>(9 * m_pTrack->trackData().route().length() / 10))
        {
            m_checkeredFlagEnabled = true;
        }
    }
    // Check if winner has finished
    else if (m_timing.leadersLap() == static_cast<int>(m_lapCount))
    {
        m_winnerFinished = true;

        Car & leader = getLeadingCar();
        m_timing.setRaceCompleted(leader.index(), true);
    }
}

void Race::updateRouteProgress(Car & car)
{
    const TrackTile * pCurrent = m_pTrack->trackTileAtLocation(car.getX(), car.getY());

    // Lap progressed?
    if (pCurrent->routeIndex() == car.currentRouteIndex() + 1)
    {
        car.setCurrentRouteIndex(pCurrent->routeIndex());
        car.setRouteProgression(car.routeProgression() + 1);

        m_positions[car.routeProgression()].push_back(car.index());
    }
    // Lap finished?
    else if (car.currentRouteIndex() ==
        static_cast<int>(m_pTrack->trackData().route().length()) - 1)
    {
        if (pCurrent->routeIndex() == 1)
        {
            car.setCurrentRouteIndex(1);
            car.setRouteProgression(car.routeProgression() + 1);

            m_positions[car.routeProgression()].push_back(car.index());

            m_timing.lapCompleted(car.index());

            if (m_winnerFinished)
            {
                m_timing.setRaceCompleted(car.index(), true);
            }
        }
    }
}

unsigned int Race::getPositionOfCar(const Car & car) const
{
    const std::vector<int> & order = m_positions[car.routeProgression()];
    for (unsigned int i = 0; i < order.size(); i++)
    {
        if (order[i] == static_cast<int>(car.index()))
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
        const unsigned int pos = getPositionOfCar(*m_cars[i]);
        if (pos > 0 && pos < bestPos)
        {
            bestCar = m_cars[i];
            bestPos = pos;
        }
    }

    return *bestCar;
}

void Race::setTrack(Track & track)
{
    m_pTrack   = &track;
    m_lapCount = m_pTrack->trackData().lapCount();
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
