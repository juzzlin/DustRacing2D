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
#include "database.hpp"
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
#include <random>

#include <MCAssetManager>
#include <MCObjectFactory>
#include <MCPhysicsComponent>
#include <MCShape>
#include <MCShapeView>
#include <MCSurfaceManager>

#include "simple_logger.hpp"

Race::Race(Game & game, size_t numCars)
  : m_humanPlayerIndex1(0)
  , m_humanPlayerIndex2(1)
  , m_unlockLimit(6) // Position required to unlock a new track
  , m_numCars(numCars)
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

    connect(&m_timing, &Timing::lapRecordAchieved, this, [this](int msecs) {
        Database::instance().saveLapRecord(*m_track, msecs);
        emit messageRequested(QObject::tr("New lap record!"));
    });

    connect(&m_timing, &Timing::raceRecordAchieved, this, [this](int msecs) {
        if (m_game.hasComputerPlayers())
        {
            Database::instance().saveRaceRecord(*m_track, msecs, static_cast<int>(m_lapCount), m_game.difficultyProfile().difficulty());
            emit messageRequested(QObject::tr("New race record!"));
        }
    });

    connect(&m_timing, &Timing::lapCompleted, this, [this](size_t, int msecs) {
        emit messageRequested(QString::fromWCharArray(Timing::msecsToString(msecs).c_str()));
    });
}

void Race::createStartGridObjects()
{
    static MCObjectFactory objectFactory(MCAssetManager::instance());
    for (size_t i = 0; i < m_numCars; i++)
    {
        MCSurfaceObjectData data("grid");
        data.setIsStationary(true);
        data.setSurfaceId("grid");

        const auto object = objectFactory.build(data);
        object->shape()->view()->setHasShadow(false);
        object->setIsPhysicsObject(false);

        m_startGridObjects.push_back(object);
    }
}

void Race::init(std::shared_ptr<Track> track, size_t lapCount)
{
    setTrack(track, lapCount);

    clearPositions();
    clearRaceFlags();

    initTiming();
    initCars();
}

void Race::initTiming()
{
    const auto lapRecord = Database::instance().loadLapRecord(*m_track);
    m_timing.setLapRecord(lapRecord.second ? lapRecord.first : -1);
    const auto raceRecord = Database::instance().loadRaceRecord(*m_track, static_cast<int>(m_lapCount), m_game.difficultyProfile().difficulty());
    m_timing.setRaceRecord(raceRecord.second ? raceRecord.first : -1);
    m_timing.reset();
}

void Race::initCars()
{
    translateCarsToStartPositions();

    m_statusHash.clear();

    for (auto && car : m_cars)
    {
        car->resetDamage();
        car->resetTireWear();

        m_stuckHash[car->index()] = StuckTileCounter(nullptr, 0);
        m_statusHash[car->index()] = CarStatus {};

        if (car->soundEffectManager())
        {
            car->soundEffectManager()->startEngineSound();
        }
    }
}

void Race::clearPositions()
{
    m_progression.clear();
}

void Race::clearRaceFlags()
{
    m_checkeredFlagEnabled = false;
    m_winnerFinished = false;
    m_isfinishedSignalSent = false;
    m_started = false;
}

void placeCar(Car & car, float x, float y, int angle)
{
    car.translate({ x, y });
    car.rotate(angle);

    juzzlin::L().debug() << "Car " << car.index() << " location: " << car.location();
    juzzlin::L().debug() << "Car " << car.index() << " rotation: " << car.angle();
}

void placeStartGrid(MCObject & grid, float x, float y, int angle)
{
    grid.translate({ x, y });
    grid.rotate(angle);
    grid.addToWorld();

    juzzlin::L().debug() << "Start grid location: " << grid.location();
    juzzlin::L().debug() << "Start grid rotation: " << grid.angle();
}

void Race::translateCarsToStartPositions()
{
    assert(m_track);

    if (const auto finishLine = m_track->finishLine())
    {
        // Reverse order
        auto order = m_cars;
        std::reverse(order.begin(), order.end());

        // Move the human player to a starting place that equals the best position
        // of the current race track.
        if (m_game.hasComputerPlayers() && !m_game.hasTwoHumanPlayers())
        {
            const auto bestPos = Database::instance().loadBestPos(*m_track, static_cast<int>(m_lapCount), m_game.difficultyProfile().difficulty());
            if (bestPos.second)
            {
                order.insert(order.begin() + bestPos.first - 1, *m_cars.begin());
                order.pop_back();
            }
        }

        // Position the cars into two queues.
        const float oddOffset = TrackTile::width() / 8;
        const float gridOffset = TrackTile::width() / 12;
        const float tileWidth = static_cast<float>(TrackTile::width());
        const float tileHeight = static_cast<float>(TrackTile::height());
        const float spacing = 0.75f * TrackTile::width();
        const float startTileX = static_cast<float>(finishLine->location().x());
        const float startTileY = static_cast<float>(finishLine->location().y());
        const int routeDirection = finishLine->rotation() % 360;
        juzzlin::L().debug() << "Route direction: " << routeDirection;
        switch (routeDirection)
        {
        case 90:
        case -270:
        {
            float moveDueToBridge = 0;
            for (size_t i = 0; i < order.size(); i++)
            {
                const float y = startTileY + (i % 2) * tileHeight / 3 - tileHeight / 6;
                float x = startTileX + (i / 2) * spacing + (i % 2) * oddOffset + moveDueToBridge;
                while (m_track->trackTileAtLocation(x, y)->tileTypeEnum() == TrackTile::TileType::Bridge)
                {
                    moveDueToBridge += tileWidth;
                    x += moveDueToBridge;
                }
                placeCar(*order.at(i), x, y, 180);
                placeStartGrid(*m_startGridObjects.at(i), x - gridOffset, y, 180);
            }
            break;
        }
        default:
        case 270:
        case -90:
        {
            float moveDueToBridge = 0;
            for (size_t i = 0; i < order.size(); i++)
            {
                const float y = startTileY + (i % 2) * tileHeight / 3 - tileHeight / 6;
                float x = startTileX - (i / 2) * spacing + (i % 2) * oddOffset - moveDueToBridge;
                while (m_track->trackTileAtLocation(x, y)->tileTypeEnum() == TrackTile::TileType::Bridge)
                {
                    moveDueToBridge += tileWidth;
                    x -= moveDueToBridge;
                }
                placeCar(*order.at(i), x, y, 0);
                placeStartGrid(*m_startGridObjects.at(i), x + gridOffset, y, 0);
            }
            break;
        }
        case 0:
        {
            float moveDueToBridge = 0;
            for (size_t i = 0; i < order.size(); i++)
            {
                const float x = startTileX + (i % 2) * tileWidth / 3 - tileWidth / 6;
                float y = startTileY - (i / 2) * spacing + (i % 2) * oddOffset - moveDueToBridge;
                while (m_track->trackTileAtLocation(x, y)->tileTypeEnum() == TrackTile::TileType::Bridge)
                {
                    moveDueToBridge += tileHeight;
                    y -= moveDueToBridge;
                }
                placeCar(*order.at(i), x, y, 90);
                placeStartGrid(*m_startGridObjects.at(i), x, y + gridOffset, 90);
            }
            break;
        }
        case 180:
        case -180:
        {
            float moveDueToBridge = 0;
            for (size_t i = 0; i < order.size(); i++)
            {
                const float x = startTileX + (i % 2) * tileWidth / 3 - tileWidth / 6;
                float y = startTileY + (i / 2) * spacing + (i % 2) * oddOffset + moveDueToBridge;
                while (m_track->trackTileAtLocation(x, y)->tileTypeEnum() == TrackTile::TileType::Bridge)
                {
                    moveDueToBridge += tileHeight;
                    y += moveDueToBridge;
                }
                placeCar(*order.at(i), x, y, 270);
                placeStartGrid(*m_startGridObjects.at(i), x, y - gridOffset, 270);
            }
            break;
        }
        }
    }
    else
    {
        juzzlin::L().error() << "Finish line tile not found in track '" << m_track->trackData().name().toStdString() << "'";
    }
}

void Race::start()
{
    if (!m_started)
    {
        m_timing.start();
        m_started = true;
    }
}

void Race::stopEngineSounds()
{
    for (auto && car : m_cars)
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
        auto && route = m_track->trackData().route();
        auto && targetNode = route.get(m_statusHash[getLeader().index()].currentTargetNodeIndex);
        if (targetNode->index() >= static_cast<int>(9 * route.numNodes() / 10))
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

    for (auto && otd : m_offTrackDetectors)
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

bool isInsideCheckPoint(Car & car, TargetNodeBasePtr targetNode, size_t tolerance)
{
    const int halfWidth = static_cast<int>(targetNode->size().width() / 2);
    const int halfHeight = static_cast<int>(targetNode->size().height() / 2);

    if (car.location().i() < static_cast<float>(targetNode->location().x()) - halfWidth - tolerance)
    {
        return false;
    }
    else if (car.location().i() > static_cast<float>(targetNode->location().x()) + halfWidth + tolerance)
    {
        return false;
    }
    else if (car.location().j() < static_cast<float>(targetNode->location().y()) - halfHeight - tolerance)
    {
        return false;
    }
    else if (car.location().j() > static_cast<float>(targetNode->location().y()) + halfHeight + tolerance)
    {
        return false;
    }

    return true;
}

void Race::updateRouteProgress(Car & car)
{
    auto && route = m_track->trackData().route();
    size_t currentTargetNodeIndex = m_statusHash[car.index()].currentTargetNodeIndex;
    size_t nextTargetNodeIndex = m_statusHash[car.index()].nextTargetNodeIndex;
    const auto targetNode = route.get(currentTargetNodeIndex);

    // Give a bit more tolerance for other than the finishing check point.
    const size_t tolerance = (!currentTargetNodeIndex ? 0 : TrackTile::height() / 20);

    // Car still racing
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

            if (isInsideCheckPoint(car, targetNode, tolerance))
            {
                updatePositions();

                checkIfLapIsCompleted(car, route, currentTargetNodeIndex);

                // Increase progress and update the positions hash
                m_progression[++m_statusHash[car.index()].routeProgression].push_back(car.index());

                // Switch to next check point
                m_statusHash[car.index()].prevTargetNodeIndex = currentTargetNodeIndex;
                if (++currentTargetNodeIndex >= route.numNodes())
                {
                    currentTargetNodeIndex = 0;
                }

                nextTargetNodeIndex = currentTargetNodeIndex;
                if (++nextTargetNodeIndex >= route.numNodes())
                {
                    nextTargetNodeIndex = 0;
                }
            }

            m_statusHash[car.index()].currentTargetNodeIndex = currentTargetNodeIndex;
            m_statusHash[car.index()].nextTargetNodeIndex = nextTargetNodeIndex;
        }
        else
        {
            checkForNewBestPosition(car);

            m_timing.setIsActive(car.index(), false);
        }
    }
    // Car has finished: cooldown laps
    else
    {
        if (isInsideCheckPoint(car, targetNode, tolerance))
        {
            // Switch to next check point
            m_statusHash[car.index()].prevTargetNodeIndex = currentTargetNodeIndex;
            if (++currentTargetNodeIndex >= route.numNodes())
            {
                currentTargetNodeIndex = 0;
            }

            nextTargetNodeIndex = currentTargetNodeIndex;
            if (++nextTargetNodeIndex >= route.numNodes())
            {
                nextTargetNodeIndex = 0;
            }
        }

        m_statusHash[car.index()].currentTargetNodeIndex = currentTargetNodeIndex;
        m_statusHash[car.index()].nextTargetNodeIndex = nextTargetNodeIndex;
    }
}

void Race::updatePositions()
{
    auto positions = m_cars;
    std::sort(positions.begin(), positions.end(), [this](auto lhs, auto rhs) -> bool {
        // Easy comparison: cars have reached different checkpoints
        if (m_statusHash[lhs->index()].routeProgression > m_statusHash[rhs->index()].routeProgression)
        {
            return true;
        }
        else if (m_statusHash[lhs->index()].routeProgression < m_statusHash[rhs->index()].routeProgression)
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
            auto && progression = m_progression[m_statusHash[lhs->index()].routeProgression];
            for (size_t index : progression)
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
    size_t position = 1;
    for (auto && car : positions)
    {
        m_statusHash[car->index()].position = position;
        position++;
    }
}

void Race::checkIfLapIsCompleted(Car & car, const Route & route, size_t currentTargetNodeIndex)
{
    if (currentTargetNodeIndex == 0 && m_statusHash[car.index()].prevTargetNodeIndex + 1 == route.numNodes())
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
            const size_t position = m_statusHash[car.index()].position;
            if (static_cast<int>(position) < m_bestPos || m_bestPos == -1)
            {
                Database::instance().saveBestPos(*m_track, static_cast<int>(position), static_cast<int>(m_lapCount), m_game.difficultyProfile().difficulty());
                emit messageRequested(QObject::tr("A new best pos!"));
            }

            auto && next = m_track->next().lock();
            if (next && next->trackData().isLocked())
            {
                if (position <= m_unlockLimit)
                {
                    next->trackData().setIsLocked(false);
                    Database::instance().saveTrackUnlockStatus(*next, static_cast<int>(m_lapCount), m_game.difficultyProfile().difficulty());
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
                counter.first = nullptr;
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
    const auto targetNode = m_track->trackData().route().get(m_statusHash[car.index()].prevTargetNodeIndex);
    const double randRadius = TrackTile::width() / 4;
    std::mt19937 engine;
    std::uniform_real_distribution<double> dist(-randRadius, randRadius);
    const float x = static_cast<float>(targetNode->location().x() + dist(engine));
    const float y = static_cast<float>(targetNode->location().y() + dist(engine));
    car.translate({ x, y });
    car.physicsComponent().reset();
    juzzlin::L().debug() << "Moved stuck car " << car.index() << " to (" << x << ", " << y << ")";
}

size_t Race::getCurrentTargetNodeIndex(const Car & car) const
{
    return m_statusHash.at(car.index()).currentTargetNodeIndex;
}

Car & Race::getLeader() const
{
    auto bestPos = m_cars.size();
    auto bestCar = m_cars.back();

    for (auto && car : m_cars)
    {
        const auto position = m_statusHash.at(car->index()).position;
        if (position > 0 && position < bestPos)
        {
            bestCar = car;
            bestPos = position;
        }
    }

    return *bestCar;
}

Car & Race::getLoser() const
{
    size_t bestValue = 0;
    auto bestCar = m_cars.back();

    for (auto && car : m_cars)
    {
        const auto position = m_statusHash.at(car->index()).position;
        if (position > bestValue)
        {
            bestCar = car;
            bestValue = position;
        }
    }

    return *bestCar;
}

size_t Race::position(const Car & car) const
{
    return m_statusHash.at(car.index()).position;
}

void Race::setTrack(std::shared_ptr<Track> track, size_t lapCount)
{
    m_lapCount = lapCount;
    m_track = track;
    const auto bestPos = Database::instance().loadBestPos(*m_track, static_cast<int>(m_lapCount), m_game.difficultyProfile().difficulty());
    m_bestPos = bestPos.second ? bestPos.first : -1;

    for (auto && otd : m_offTrackDetectors)
    {
        otd->setTrack(track);
    }
}

size_t Race::lapCount() const
{
    return m_lapCount;
}

void Race::addCar(Car & car)
{
    if (find(m_cars.begin(), m_cars.end(), &car) == m_cars.end())
    {
        m_cars.push_back(&car);
        m_offTrackDetectors.push_back(std::make_shared<OffTrackDetector>(car));
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
        return m_timing.raceCompleted(m_humanPlayerIndex1) && m_timing.raceCompleted(m_humanPlayerIndex2);
    }

    return m_timing.raceCompleted(m_humanPlayerIndex1);
}

Race::~Race() = default;
