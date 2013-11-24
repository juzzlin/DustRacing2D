// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#include "race.hpp"

#include "car.hpp"
#include "carsoundeffectmanager.hpp"
#include "game.hpp"
#include "layers.hpp"
#include "offtrackdetector.hpp"
#include "renderer.hpp"
#include "settings.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "trackobjectfactory.hpp"
#include "tracktile.hpp"

#include "../common/config.hpp"
#include "../common/targetnodebase.hpp"

#include <algorithm>
#include <cassert>

#include <MCLogger>
#include <MCAssetManager>
#include <MCObjectFactory>
#include <MCShape>
#include <MCShapeView>
#include <MCSurfaceManager>

static const int HUMAN_PLAYER_INDEX1 = 0;
static const int HUMAN_PLAYER_INDEX2 = 1;
static const int UNLOCK_LIMIT        = 6; // Position required to unlock a new track

Race::Race(const Game & game, unsigned int numCars)
: m_numCars(numCars)
, m_lapCount(5)
, m_timing(numCars)
, m_track(nullptr)
, m_started(false)
, m_checkeredFlagEnabled(false)
, m_winnerFinished(false)
, m_isfinishedSignalSent(false)
, m_bestPos(-1)
, m_offTrackCounter(0)
, m_game(game)
{
    createStartGridObjects();

    m_offTrackMessageTimer.setSingleShot(true);
    m_offTrackMessageTimer.setInterval(30000);

    connect(&m_timing, SIGNAL(lapRecordAchieved(int)), this, SLOT(setLapRecord(int)));

    connect(&m_timing, SIGNAL(raceRecordAchieved(int)), this, SLOT(setRaceRecord(int)));
}

void Race::createStartGridObjects()
{
    static MCObjectFactory objectFactory(MCAssetManager::instance());
    for (int i = 0; i < m_numCars; i++)
    {
        MCSurfaceObjectData data("grid");
        data.setBatchMode(true);
        data.setIsStationary(true);
        data.setSurfaceId("grid");
        data.setLayer(Layers::Ground);

        MCObjectPtr object = objectFactory.build(data);
        object->shape()->view()->setHasShadow(false);
        object->shape()->view()->setShaderProgram(&Renderer::instance().program("master"));
        object->setIsPhysicsObject(false);

        m_startGridObjects.push_back(object);
    }
}

void Race::init(Track & track, int lapCount)
{
    m_lapCount = lapCount;

    setTrack(track);
    translateCarsToStartPositions();

    for(Car * car : m_cars)
    {
        car->setCurrentTargetNodeIndex(0);
        car->setPrevTargetNodeIndex(0);
        car->setRouteProgression(0);

        m_stuckHash[car->index()] = StuckTileCounter(nullptr, 0);

        if (car->soundEffectManager())
        {
            car->soundEffectManager()->startEngineSound();
        }
    }

    m_timing.reset();
    m_positions.clear();

    m_checkeredFlagEnabled = false;
    m_winnerFinished       = false;
    m_isfinishedSignalSent = false;
    m_started              = false;
}

void placeCar(Car & car, MCFloat x, MCFloat y, int angle)
{
    car.translate(MCVector2d<MCFloat>(x, y));
    car.rotate(angle);
}

void placeStartGrid(MCObject & grid, MCFloat x, MCFloat y, int angle)
{
    grid.translate(MCVector2d<MCFloat>(x, y));
    grid.rotate(angle);
    grid.addToWorld();
}

void Race::translateCarsToStartPositions()
{
    assert(m_track);

    if (TrackTile * finishLine = m_track->finishLine())
    {
        const MCFloat startTileX = finishLine->location().x();
        const MCFloat startTileY = finishLine->location().y();
        const MCFloat tileWidth  = TrackTile::TILE_W;
        const MCFloat tileHeight = TrackTile::TILE_H;
        const MCFloat spacing    = 0.75 * TrackTile::TILE_W;
        const MCFloat oddOffset  = TrackTile::TILE_W / 8;
        const MCFloat gridOffset = 24;

        // Reverse order
        std::vector<Car *> order = m_cars;
        std::reverse(order.begin(), order.end());

        // Move the human player to a starting place that equals the best position
        // of the current race track.
        if (m_game.hasComputerPlayers() && !m_game.hasTwoHumanPlayers())
        {
            const int bestPos = Settings::instance().loadBestPos(*m_track, m_lapCount);
            if (bestPos > 0)
            {
                order.insert(order.begin() + bestPos - 1, *m_cars.begin());
                order.pop_back();
            }
        }

        // Position the cars into two queues.
        const int routeDirection = finishLine->rotation() % 360;
        switch (routeDirection)
        {
        case 90:
        case -270:
            for (MCUint i = 0; i < order.size(); i++)
            {
                const MCFloat rowPos = (i / 2) * spacing + (i % 2) * oddOffset;
                const MCFloat colPos = (i % 2) * tileHeight / 3 - tileHeight / 6;
                placeCar(*order.at(i), startTileX + rowPos, startTileY + colPos, 180);
                placeStartGrid(*m_startGridObjects.at(i), startTileX + rowPos - gridOffset, startTileY + colPos, 180);
            }
            break;

        default:
        case 270:
        case -90:
            for (MCUint i = 0; i < order.size(); i++)
            {
                const MCFloat rowPos = (i / 2) * spacing + (i % 2) * oddOffset;
                const MCFloat colPos = (i % 2) * tileHeight / 3 - tileHeight / 6;
                placeCar(*order.at(i), startTileX - rowPos, startTileY + colPos, 0);
                placeStartGrid(*m_startGridObjects.at(i), startTileX - rowPos + gridOffset, startTileY + colPos, 0);
            }
            break;

        case 0:
            for (MCUint i = 0; i < order.size(); i++)
            {
                const MCFloat rowPos = (i % 2) * tileWidth / 3 - tileWidth / 6;
                const MCFloat colPos = (i / 2) * spacing + (i % 2) * oddOffset;
                placeCar(*order.at(i), startTileX + rowPos, startTileY - colPos, 90);
                placeStartGrid(*m_startGridObjects.at(i), startTileX + rowPos, startTileY - colPos + gridOffset, 90);
            }
            break;

        case 180:
        case -180:
            for (MCUint i = 0; i < order.size(); i++)
            {
                const MCFloat rowPos = (i % 2) * tileWidth  / 3 - tileWidth / 6;
                const MCFloat colPos = (i / 2) * spacing + (i % 2) * oddOffset;
                placeCar(*order.at(i), startTileX + rowPos, startTileY + colPos, 270);
                placeStartGrid(*m_startGridObjects.at(i), startTileX + rowPos, startTileY + colPos - gridOffset, 270);
            }
            break;
        }
    }
    else
    {
        MCLogger().error() << "Finish line tile not found in track '" <<
            m_track->trackData().name().toStdString() << "'";
    }
}

void Race::setRaceRecord(int msecs)
{
    Settings::instance().saveRaceRecord(*m_track, msecs, m_lapCount);

    emit messageRequested(QObject::tr("New race record!"));
}

void Race::setLapRecord(int msecs)
{
    Settings::instance().saveLapRecord(*m_track, msecs);

    emit messageRequested(QObject::tr("New lap record!"));
}

void Race::start()
{
    if (!m_started)
    {
        m_timing.start();
        m_started  = true;
    }
}

void Race::stopEngineSounds()
{
    for(Car * car : m_cars)
    {
        if (car->soundEffectManager())
        {
            car->soundEffectManager()->stopEngineSound();
        }
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
    if (m_timing.leadersLap() + 1 == m_lapCount)
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
    else if (m_timing.leadersLap() == m_lapCount)
    {
        if (!m_winnerFinished)
        {
            m_winnerFinished = true;

            Car & leader = getLeadingCar();
            m_timing.setRaceCompleted(leader.index(), true, leader.isHuman());

            if (m_game.mode() == Game::TimeTrial)
            {
                emit messageRequested(QObject::tr("The Time Trial has ended!"));
            }
            else
            {
                emit messageRequested(QObject::tr("The winner has finished!"));
            }
        }
    }

    for (OffTrackDetectorPtr otd : m_offTrackDetectors)
    {
        otd->update();
    }

    if (isRaceFinished() && !m_isfinishedSignalSent)
    {
        emit finished();
        m_isfinishedSignalSent = true;
    }
}

void Race::pitStop(Car & car)
{
    if (m_timing.lap(car.index()) > 0)
    {
        const QString pitSoundHandle("pit");
        emit messageRequested(QObject::tr("Pit stop!"));
        emit locationChanged(pitSoundHandle, car.location().i(), car.location().j());
        emit playRequested(pitSoundHandle, false);
        emit tiresChanged(car);
    }
}

bool isInsideCheckPoint(Car & car, TargetNodeBase & tnode, int tolerance)
{
    const int width2  = tnode.size().width()  / 2;
    const int height2 = tnode.size().height() / 2;

    if (car.location().i() < tnode.location().x() - width2 - tolerance)
    {
        return false;
    }
    else if (car.location().i() > tnode.location().x() + width2 + tolerance)
    {
        return false;
    }
    else if (car.location().j() < tnode.location().y() - height2 - tolerance)
    {
        return false;
    }
    else if (car.location().j() > tnode.location().y() + height2 + tolerance)
    {
        return false;
    }

    return true;
}

void Race::updateRouteProgress(Car & car)
{
    const Route    & route = m_track->trackData().route();
    unsigned int     currentTargetNodeIndex = car.currentTargetNodeIndex();
    TargetNodeBase & tnode = route.get(currentTargetNodeIndex);

    if (m_timing.isActive(car.index()))
    {
        if (!m_timing.raceCompleted(car.index()))
        {
            // Check is car is stuck and if so, move onto nearest asphalt tile.
            if (!car.isHuman())
            {
                checkIfCarIsStuck(car);
            }

            // Check is car is off track and display a message.
            if (car.isHuman())
            {
                checkIfCarIsOffTrack(car);
            }

            // Give a bit more tolerance for other than the finishing check point.
            const int tolerance = (currentTargetNodeIndex == 0 ? 0 : TrackTile::TILE_H / 20);
            if (isInsideCheckPoint(car, tnode, tolerance))
            {
                checkIfLapIsCompleted(car, route, currentTargetNodeIndex);

                // Increase progress and update the positions hash
                car.setRouteProgression(car.routeProgression() + 1);
                m_positions[car.routeProgression()].push_back(car.index());

                // Switch to next check point
                car.setPrevTargetNodeIndex(currentTargetNodeIndex);
                if (++currentTargetNodeIndex >= route.numNodes())
                {
                    currentTargetNodeIndex = 0;
                }
            }

            car.setCurrentTargetNodeIndex(currentTargetNodeIndex);
        }
        else
        {
            checkForNewBestPosition(car);

            m_timing.setIsActive(car.index(), false);
        }
    }
}

void Race::checkIfLapIsCompleted(Car & car, const Route & route, unsigned int currentTargetNodeIndex)
{
    if (currentTargetNodeIndex == 0 &&
        car.prevTargetNodeIndex() + 1 == static_cast<int>(route.numNodes()))
    {
        m_timing.lapCompleted(car.index(), car.isHuman());

        // Finish the race if winner has already finished.
        if (m_winnerFinished)
        {
            m_timing.setRaceCompleted(car.index(), true, car.isHuman());
        }
    }
}

void Race::checkForNewBestPosition(const Car & car)
{
    // Check if the race is completed for a human player and if so,
    // check if new best pos achieved and save it.
    if (m_game.mode() == Game::OnePlayerRace || m_game.mode() == Game::TwoPlayerRace)
    {
        if (car.isHuman())
        {
            const int pos = getPositionOfCar(car);
            if (pos < m_bestPos || m_bestPos == -1)
            {
                Settings::instance().saveBestPos(*m_track, pos, m_lapCount);
                emit messageRequested(QObject::tr("A new best pos!"));
            }

            Track * next = m_track->next();
            if (next && next->trackData().isLocked())
            {
                if (pos <= UNLOCK_LIMIT)
                {
                    next->trackData().setIsLocked(false);
                    Settings::instance().saveTrackUnlockStatus(*next, m_lapCount);
                    emit messageRequested(QObject::tr("A new track unlocked!"));
                }
                else
                {
                    emit messageRequested(QObject::tr("Better luck next time.."));
                }
            }
        }
    }
}

void Race::checkIfCarIsStuck(Car & car)
{
    if (started())
    {
        static const int STUCK_LIMIT = 60 * 5; // 5 secs.

        TrackTile * currentTile = m_track->trackTileAtLocation(car.location().i(), car.location().j());

        StuckTileCounter & counter = m_stuckHash[car.index()];
        if (counter.first == nullptr || counter.first != currentTile)
        {
            counter.first  = currentTile;
            counter.second = 0;
        }
        else if (counter.first == currentTile && !currentTile->hasAsphalt())
        {
            if (++counter.second >= STUCK_LIMIT)
            {
                moveCarOntoPreviousCheckPoint(car);
            }
        }
    }
}

void Race::checkIfCarIsOffTrack(Car & car)
{
    if (car.isOffTrack() && !m_offTrackMessageTimer.isActive())
    {
        static const int OFF_TRACK_LIMIT = 60; // 1 sec

        m_offTrackCounter++;
        if (m_offTrackCounter > OFF_TRACK_LIMIT)
        {
            if (rand() % 2 == 0)
            {
                emit messageRequested(QObject::tr("You must stay on track!"));
            }
            else
            {
                emit messageRequested(QObject::tr("Watch your tires!"));
            }
            m_offTrackMessageTimer.start();
        }
    }
    else
    {
        m_offTrackCounter = 0;
    }
}

void Race::moveCarOntoPreviousCheckPoint(Car & car)
{
    const Route    & route = m_track->trackData().route();
    TargetNodeBase & tnode = route.get(car.prevTargetNodeIndex());

    car.translate(MCVector3dF(tnode.location().x(), tnode.location().y()));
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
    m_bestPos = Settings::instance().loadBestPos(*m_track, m_lapCount);

    m_timing.setLapRecord(Settings::instance().loadLapRecord(*m_track));

    for (OffTrackDetectorPtr otd : m_offTrackDetectors)
    {
        otd->setTrack(track);
    }
}

int Race::lapCount() const
{
    return m_lapCount;
}

void Race::addCar(Car & car)
{
    if (find(m_cars.begin(), m_cars.end(), &car) == m_cars.end())
    {
        m_cars.push_back(&car);
        m_offTrackDetectors.push_back(OffTrackDetectorPtr(new OffTrackDetector(car)));
    }
}

void Race::removeCars()
{
    m_cars.clear();
    m_offTrackDetectors.clear();
}

Timing & Race::timing()
{
    return m_timing;
}

bool Race::checkeredFlagEnabled() const
{
    return m_checkeredFlagEnabled;
}

bool Race::isRaceFinished() const
{
    if (m_game.hasTwoHumanPlayers())
    {
        return
            m_timing.raceCompleted(HUMAN_PLAYER_INDEX1) &&
            m_timing.raceCompleted(HUMAN_PLAYER_INDEX2);
    }

    return m_timing.raceCompleted(HUMAN_PLAYER_INDEX1);
}

Race::~Race()
{
}
