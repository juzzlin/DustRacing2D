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

#include "race.h"

#include "car.h"
#include "track.h"
#include "trackdata.h"
#include "tracktile.h"
#include <algorithm>
#include <cassert>

Race::Race()
: m_lapCount(0)
, m_pTrack(nullptr)
{
}

void Race::init()
{
    for(Car * pCar : m_cars)
    {
        m_routeHash[pCar] = 0;
    }
}

void Race::update()
{
    for(Car * pCar : m_cars)
    {
        checkRoute(pCar);
    }
}

void Race::checkRoute(Car * pCar)
{
    assert(m_pTrack);

    TrackTile * pCurrent = m_pTrack->trackTileAtLocation(
        pCar->getX(), pCar->getY());

    // Lap progressed?
    if (pCurrent->routeIndex() == m_routeHash[pCar] + 1)
    {
        m_routeHash[pCar] = pCurrent->routeIndex();
    }
    // Lap finished?
    else if (m_routeHash[pCar] ==
        static_cast<int>(m_pTrack->trackData().route().length()) - 1)
    {
        if (pCurrent->routeIndex() == 1)
        {
            m_routeHash[pCar] = 1;
            std::cout << "A New lap!" << std::endl;
        }
    }
}

void Race::setTrack(Track * pTrack)
{
    m_pTrack = pTrack;
}

void Race::setLapCount(unsigned int laps)
{
    m_lapCount = laps;
}

void Race::addCar(Car * pCar)
{
    if (find(m_cars.begin(), m_cars.end(), pCar) == m_cars.end())
    {
        m_cars.push_back(pCar);
    }
}

Race::~Race()
{
}
