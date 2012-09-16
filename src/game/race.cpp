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
#include "messageoverlay.hpp"
#include "settings.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "tracktile.hpp"

#include "../common/config.hpp"
#include "../common/targetnodebase.hpp"

#include <algorithm>
#include <cassert>

static const int HUMAN_PLAYER_INDEX = 0;

Race::Race(unsigned int numCars, MessageOverlay & messageOverlay)
: m_lapCount(0)
, m_timing(numCars)
, m_track(nullptr)
, m_started(false)
, m_checkeredFlagEnabled(false)
, m_winnerFinished(false)
, m_bestPos(-1)
, m_messageOverlay(messageOverlay)
{
}

void Race::init()
{
    for(Car * pCar : m_cars)
    {
        pCar->setCurrentTargetNodeIndex(0);
        pCar->setPrevTargetNodeIndex(0);
        pCar->setRouteProgression(0);
    }

    m_timing.reset();
    m_positions.clear();

    m_checkeredFlagEnabled = false;
    m_winnerFinished       = false;
    m_started              = false;
}

void Race::start()
{
    if (!m_started)
    {
        m_timing.start();
        m_messageOverlay.addMessage("GO!!!");
        m_started  = true;
    }
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
        Car                  & leader = getLeadingCar();
        const Route          & route  = m_track->trackData().route();
        const TargetNodeBase & tnode  = route.get(leader.currentTargetNodeIndex());

        if (tnode.index() >=
            static_cast<int>(9 * route.numNodes() / 10))
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

bool isInsideCheckPoint(Car & car, TargetNodeBase & tnode, int tolerance)
{
    if (car.location().i() < tnode.location().x() - TrackTile::TILE_W / 2 - tolerance)
    {
        return false;
    }
    else if (car.location().i() > tnode.location().x() + TrackTile::TILE_W / 2 + tolerance)
    {
        return false;
    }
    else if (car.location().j() < tnode.location().y() - TrackTile::TILE_H / 2 - tolerance)
    {
        return false;
    }
    else if (car.location().j() > tnode.location().y() + TrackTile::TILE_H / 2 + tolerance)
    {
        return false;
    }

    return true;
}

void Race::updateRouteProgress(Car & car)
{
    const Route    & route = m_track->trackData().route();
    unsigned int     index = car.currentTargetNodeIndex();
    TargetNodeBase & tnode = route.get(index);

    if (m_timing.isActive(car.index()))
    {
        if (!m_timing.raceCompleted(car.index()))
        {
            // Give a bit more tolerance for other than the finishing check point.
            const int tolerance = index == 0 ? 0 : TrackTile::TILE_H / 20;
            if (isInsideCheckPoint(car, tnode, tolerance))
            {
                // Lap finished?
                if (index == 0 && car.prevTargetNodeIndex() + 1 == static_cast<int>(route.numNodes()))
                {
                    m_timing.lapCompleted(car.index(), car.isHuman());

                    // Check if we have a new lap record
                    if (m_timing.newLapRecordAchieved())
                    {
                        Settings::instance().saveLapRecord(*m_track, m_timing.lapRecord());
                        m_messageOverlay.addMessage("New lap record!");
                    }

                    // Finish the race if winner has already finished.
                    if (m_winnerFinished)
                    {
                        m_timing.setRaceCompleted(car.index(), true);
                    }
                }

                // Increase progress and update the positions hash
                car.setRouteProgression(car.routeProgression() + 1);
                m_positions[car.routeProgression()].push_back(car.index());

                // Switch to next check point
                car.setPrevTargetNodeIndex(index);
                if (++index >= route.numNodes())
                {
                    index = 0;
                }
            }

            car.setCurrentTargetNodeIndex(index);
        }
        else
        {
            // Check if the race is completed fo a human player and if so,
            // check if new best pos achieved and save it.
            if (car.isHuman())
            {
                const int pos = getPositionOfCar(car);
                if (pos < m_bestPos || m_bestPos == -1)
                {
                    Settings::instance().saveBestPos(*m_track, pos);
                    m_messageOverlay.addMessage("New best pos!");
                }
            }

            m_timing.setIsActive(car.index(), false);
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
    m_track   = &track;
    m_lapCount = m_track->trackData().lapCount();
    m_bestPos  = Settings::instance().loadBestPos(*m_track);

    m_timing.setLapRecord(Settings::instance().loadLapRecord(*m_track));
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

bool Race::finished() const
{
    return m_timing.raceCompleted(HUMAN_PLAYER_INDEX);
}

Race::~Race()
{
}
