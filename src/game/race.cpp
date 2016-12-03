// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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
#include <MCPhysicsComponent>
#include <MCShape>
#include <MCShapeView>
#include <MCSurfaceManager>

static const int HUMAN_PLAYER_INDEX1 = 0;
static const int HUMAN_PLAYER_INDEX2 = 1;
static const int UNLOCK_LIMIT        = 6; // Position required to unlock a new track

Race::Race(Game & game, unsigned int numCars)
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

    connect(&m_timing, &Timing::lapRecordAchieved, [this] (int msecs) {
        Settings::instance().saveLapRecord(*m_track, msecs);
        emit messageRequested(QObject::tr("New lap record!"));
    });

    connect(&m_timing, &Timing::raceRecordAchieved, [this] (int msecs) {
        if (m_game.hasComputerPlayers()) {
            Settings::instance().saveRaceRecord(*m_track, msecs, m_lapCount, m_game.difficultyProfile().difficulty());
            emit messageRequested(QObject::tr("New race record!"));
        }
    });

    connect(&m_timing, &Timing::lapCompleted, [this] (unsigned int, int msecs) {
        emit messageRequested(QString::fromWCharArray(Timing::msecsToString(msecs).c_str()));
    });
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

        MCObjectPtr object = objectFactory.build(data);
        object->shape()->view()->setHasShadow(false);
        object->setIsPhysicsObject(false);

        m_startGridObjects.push_back(object);
    }
}

void Race::init(Track & track, int lapCount)
{
    setTrack(track, lapCount);

    clearPositions();
    clearRaceFlags();

    initTiming();
    initCars();
}

void Race::initTiming()
{
    m_timing.setLapRecord(Settings::instance().loadLapRecord(*m_track));
    m_timing.setRaceRecord(Settings::instance().loadRaceRecord(*m_track, m_lapCount, m_game.difficultyProfile().difficulty()));
    m_timing.reset();
}

void Race::initCars()
{
    translateCarsToStartPositions();

    for (auto && car : m_cars)
    {
        car->setNextTargetNodeIndex(0);
        car->setCurrentTargetNodeIndex(0);
        car->setPrevTargetNodeIndex(0);
        car->setRouteProgression(0);
        car->resetDamage();
        car->resetTireWear();

        m_stuckHash[car->index()] = StuckTileCounter(nullptr, 0);

        if (car->soundEffectManager())
        {
            car->soundEffectManager()->startEngineSound();
        }
    }
}

void Race::clearPositions()
{
    m_progression.clear();

    for (auto && car : m_cars) {
        car->setPosition(0);
    }
}

void Race::clearRaceFlags()
{
    m_checkeredFlagEnabled = false;
    m_winnerFinished       = false;
    m_isfinishedSignalSent = false;
    m_started              = false;
}

void placeCar(Car & car, float x, float y, int angle)
{
    car.translate(MCVector2d<float>(x, y));
    car.rotate(angle);
}

void placeStartGrid(MCObject & grid, float x, float y, int angle)
{
    grid.translate(MCVector2dF(x, y));
    grid.rotate(angle);
    grid.addToWorld();
}

void Race::translateCarsToStartPositions()
{
    assert(m_track);

    if (TrackTilePtr finishLine = m_track->finishLine())
    {
        const float startTileX = finishLine->location().x();
        const float startTileY = finishLine->location().y();
        const float tileWidth  = TrackTile::TILE_W;
        const float tileHeight = TrackTile::TILE_H;
        const float spacing    = 0.75 * TrackTile::TILE_W;
        const float oddOffset  = TrackTile::TILE_W / 8;
        const float gridOffset = 24;

        // Reverse order
        auto order = m_cars;
        std::reverse(order.begin(), order.end());

        // Move the human player to a starting place that equals the best position
        // of the current race track.
        if (m_game.hasComputerPlayers() && !m_game.hasTwoHumanPlayers())
        {
            const int bestPos = Settings::instance().loadBestPos(*m_track, m_lapCount, m_game.difficultyProfile().difficulty());
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
            for (unsigned int i = 0; i < order.size(); i++)
            {
                const float rowPos = (i / 2) * spacing + (i % 2) * oddOffset;
                const float colPos = (i % 2) * tileHeight / 3 - tileHeight / 6;
                placeCar(*order.at(i), startTileX + rowPos, startTileY + colPos, 180);
                placeStartGrid(*m_startGridObjects.at(i), startTileX + rowPos - gridOffset, startTileY + colPos, 180);
            }
            break;

        default:
        case 270:
        case -90:
            for (unsigned int i = 0; i < order.size(); i++)
            {
                const float rowPos = (i / 2) * spacing + (i % 2) * oddOffset;
                const float colPos = (i % 2) * tileHeight / 3 - tileHeight / 6;
                placeCar(*order.at(i), startTileX - rowPos, startTileY + colPos, 0);
                placeStartGrid(*m_startGridObjects.at(i), startTileX - rowPos + gridOffset, startTileY + colPos, 0);
            }
            break;

        case 0:
            for (unsigned int i = 0; i < order.size(); i++)
            {
                const float rowPos = (i % 2) * tileWidth / 3 - tileWidth / 6;
                const float colPos = (i / 2) * spacing + (i % 2) * oddOffset;
                placeCar(*order.at(i), startTileX + rowPos, startTileY - colPos, 90);
                placeStartGrid(*m_startGridObjects.at(i), startTileX + rowPos, startTileY - colPos + gridOffset, 90);
            }
            break;

        case 180:
        case -180:
            for (unsigned int i = 0; i < order.size(); i++)
            {
                const float rowPos = (i % 2) * tileWidth  / 3 - tileWidth / 6;
                const float colPos = (i / 2) * spacing + (i % 2) * oddOffset;
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
    for(auto && car : m_cars)
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
    for (auto && car : m_cars)
    {
        updateRouteProgress(*car);
    }

    // Enable the checkered flag if leader has done at least 95% of the last lap.
    if (m_timing.leadersLap() + 1 == m_lapCount)
    {
        auto && leader = getLeader();
        auto && route = m_track->trackData().route();
        auto tnode = route.get(leader.currentTargetNodeIndex());

        if (tnode->index() >= static_cast<int>(9 * route.numNodes() / 10))
        {
            if (!m_checkeredFlagEnabled)
            {
                const QString bellSoundHandle("bell");
                emit playRequested(bellSoundHandle, false);

                m_checkeredFlagEnabled = true;
            }
        }
    }
    // Check if winner has finished
    else if (m_timing.leadersLap() == m_lapCount)
    {
        if (!m_winnerFinished)
        {
            m_winnerFinished = true;

            auto && leader = getLeader();
            m_timing.setRaceCompleted(leader.index(), true, leader.isHuman());

            if (m_game.mode() == Game::Mode::TimeTrial)
            {
                emit messageRequested(QObject::tr("The Time Trial has ended!"));
            }
            else
            {
                emit messageRequested(QObject::tr("The winner has finished!"));
                emit playRequested("cheering", false);
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

    m_timing.tick();
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

        car.resetDamage();
        car.resetTireWear();
    }
}

bool isInsideCheckPoint(Car & car, TargetNodeBasePtr tnode, int tolerance)
{
    const int width2  = tnode->size().width()  / 2;
    const int height2 = tnode->size().height() / 2;

    if (car.location().i() < tnode->location().x() - width2 - tolerance)
    {
        return false;
    }
    else if (car.location().i() > tnode->location().x() + width2 + tolerance)
    {
        return false;
    }
    else if (car.location().j() < tnode->location().y() - height2 - tolerance)
    {
        return false;
    }
    else if (car.location().j() > tnode->location().y() + height2 + tolerance)
    {
        return false;
    }

    return true;
}

void Race::updateRouteProgress(Car & car)
{
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
            auto && route = m_track->trackData().route();
            unsigned int currentTargetNodeIndex = car.currentTargetNodeIndex();
            unsigned int nextTargetNodeIndex = car.nextTargetNodeIndex();
            auto tnode = route.get(currentTargetNodeIndex);
            const int tolerance = (currentTargetNodeIndex == 0 ? 0 : TrackTile::TILE_H / 20);
            if (isInsideCheckPoint(car, tnode, tolerance))
            {
                updatePositions();

                checkIfLapIsCompleted(car, route, currentTargetNodeIndex);

                // Increase progress and update the positions hash
                car.setRouteProgression(car.routeProgression() + 1);
                m_progression[car.routeProgression()].push_back(car.index());

                // Switch to next check point
                car.setPrevTargetNodeIndex(currentTargetNodeIndex);
                if (++currentTargetNodeIndex >= route.numNodes())
                {
                    currentTargetNodeIndex = 0;
                }

                nextTargetNodeIndex = currentTargetNodeIndex;
                if (++nextTargetNodeIndex >= route.numNodes()){
                    nextTargetNodeIndex = 0;
                }
            }

            car.setCurrentTargetNodeIndex(currentTargetNodeIndex);
            car.setNextTargetNodeIndex(nextTargetNodeIndex);
        }
        else
        {
            checkForNewBestPosition(car);

            m_timing.setIsActive(car.index(), false);
        }
    }
}

void Race::updatePositions()
{
    CarVector positions = m_cars;

    std::sort(positions.begin(), positions.end(), [=](Car * lhs, Car * rhs) -> bool {
        // Easy comparison: cars have reached different checkpoints
        if (lhs->routeProgression() > rhs->routeProgression())
        {
            return true;
        }
        else if (lhs->routeProgression() < rhs->routeProgression())
        {
            return false;
        }
        // For cars that have reached the same checkpoint do comparison
        // by the position inside that checkpoint
        else
        {
            int lhsPos = 0;
            int rhsPos = 0;
            int pos = 0;
            auto && progression = m_progression[lhs->routeProgression()];
            for (int index : progression)
            {
                if (m_cars[index] == lhs)
                {
                    lhsPos = pos;
                }
                else if (m_cars[index] == rhs)
                {
                    rhsPos = pos;
                }

                pos++;
            }

            return lhsPos < rhsPos;
        }
    });

    // Store current position to car objects for fast access
    int pos = 1;
    for (auto && car : positions)
    {
        car->setPosition(pos);
        pos++;
    }
}

void Race::checkIfLapIsCompleted(Car & car, const Route & route, unsigned int currentTargetNodeIndex)
{
    if (currentTargetNodeIndex == 0 &&
        car.prevTargetNodeIndex() + 1 == static_cast<int>(route.numNodes()))
    {
        m_timing.setLapCompleted(car.index(), car.isHuman());

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
    if (m_game.mode() == Game::Mode::OnePlayerRace || m_game.mode() == Game::Mode::TwoPlayerRace)
    {
        if (car.isHuman())
        {
            const int pos = car.position();
            if (pos < m_bestPos || m_bestPos == -1)
            {
                Settings::instance().saveBestPos(*m_track, pos, m_lapCount, m_game.difficultyProfile().difficulty());
                emit messageRequested(QObject::tr("A new best pos!"));
            }

            auto next = m_track->next();
            if (next && next->trackData().isLocked())
            {
                if (pos <= UNLOCK_LIMIT)
                {
                    next->trackData().setIsLocked(false);
                    Settings::instance().saveTrackUnlockStatus(*next, m_lapCount, m_game.difficultyProfile().difficulty());
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

        auto currentTile = m_track->trackTileAtLocation(car.location().i(), car.location().j());
        auto && counter = m_stuckHash[car.index()];
        if (counter.first == nullptr || counter.first != currentTile)
        {
            counter.first = currentTile;
            counter.second = 0;
        }
        else if (counter.first == currentTile)
        {
            if (++counter.second >= STUCK_LIMIT)
            {
                moveCarOntoPreviousCheckPoint(car);
                counter.first  = nullptr;
                counter.second = 0;
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
    // Randomize the target location a bit, because otherwise multiple
    // stuck cars could be sent to the exactly same location and that would
    // result in really bad things.
    auto && route = m_track->trackData().route();
    auto tnode = route.get(car.prevTargetNodeIndex());
    const int randRadius = 64;
    car.translate(MCVector3dF(
        tnode->location().x() + rand() % randRadius - randRadius / 2,
        tnode->location().y() + rand() % randRadius - randRadius / 2));
    car.physicsComponent().reset();
}

Car & Race::getLeader() const
{
    int bestPos = m_cars.size();
    auto bestCar = m_cars.back();

    for (unsigned int i = 0; i < m_cars.size(); i++)
    {
        const auto pos = m_cars[i]->position();
        if (pos > 0 && pos < bestPos)
        {
            bestCar = m_cars[i];
            bestPos = pos;
        }
    }

    return *bestCar;
}

Car & Race::getLoser() const
{
    int bestValue = 0;
    auto bestCar = m_cars.back();

    for (auto && car : m_cars)
    {
        const int pos = car->position();
        if (pos > bestValue)
        {
            bestCar = car;
            bestValue = pos;
        }
    }

    return *bestCar;
}

void Race::setTrack(Track & track, int lapCount)
{
    m_lapCount = lapCount;
    m_track    = &track;
    m_bestPos  = Settings::instance().loadBestPos(*m_track, m_lapCount, m_game.difficultyProfile().difficulty());

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
