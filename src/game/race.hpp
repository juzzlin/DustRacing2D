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

#ifndef RACE_HPP
#define RACE_HPP

#include <MCObject>
#include <QObject>
#include <QString>
#include <QTimer>

#include <chrono>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "audiosource.hpp"
#include "timing.hpp"
#include "tracktile.hpp"

class Car;
class Game;
class OffTrackDetector;
class Route;
class Track;

//! Class that controls the race event, checkpoints and timing.
class Race : public AudioSource
{
    Q_OBJECT

public:
    Race(Game & game, size_t numCars);
    virtual ~Race() override;

    using TrackS = std::shared_ptr<Track>;
    void initialize(TrackS track, size_t lapCount);

    bool started() const;

    size_t lapCount() const;

    void addCar(Car & car);
    void removeCars();

    using TimingW = std::weak_ptr<Timing>;
    TimingW timing() const;

    bool checkeredFlagEnabled() const;

    size_t getCurrentTargetNodeIndex(size_t carIndex) const;

    Car & getLoser() const;
    Car & getLeader() const;

    size_t position(size_t carIndex) const;

signals:

    void finished();
    void messageRequested(QString message);
    void tiresChanged(const Car & car);

    void lapRecordAchieved(int msec);
    void raceRecordAchieved(int msec);

public slots:

    void start();

    void stopEngineSounds();

    void update(std::chrono::milliseconds timeStep);

    void pitStop(Car & car);

private:
    void checkForNewBestPosition(const Car & car);
    void checkIfCarIsStuck(Car & car);
    void checkIfCarIsOffTrack(Car & car);
    void checkIfLapIsCompleted(Car & car, const Route & route, size_t currentTargetNodeIndex);

    void clearPositions();
    void clearRaceFlags();

    void createStartGridObjects();

    void initCars();
    void initTiming();

    bool isRaceFinished() const;

    void moveCarOntoPreviousCheckPoint(Car & car);

    void setTrack(TrackS track, size_t lapCount);

    void translateCarsToStartPositions();

    void updateRouteProgress(Car & car);
    void updatePositions();

    const size_t m_humanPlayerIndex1;
    const size_t m_humanPlayerIndex2;

    // Position required to unlock a new track
    const size_t m_unlockLimit = 6;

    using CarVector = std::vector<Car *>;
    CarVector m_cars;

    // Used only to keep references to the start grid objects.
    // They are common for all races and created only once.
    using StartGridObjectVector = std::vector<MCObjectPtr>;
    StartGridObjectVector m_startGridObjects;

    // Map from car route progression to car order vector.
    // Tracks the order of the cars in the route.
    using ProgressionHash = std::unordered_map<size_t, std::vector<size_t>>;
    mutable ProgressionHash m_progression;

    using OffTrackDetectorS = std::shared_ptr<OffTrackDetector>;
    using OTDVector = std::vector<OffTrackDetectorS>;
    OTDVector m_offTrackDetectors;

    // Data structure to determine if a car is stuck.
    // In that case we move the car onto the previous check point.
    using StuckTileCounter = std::pair<TrackTileS, int>; // Tile pointer and counter.
    using StuckHash = std::unordered_map<size_t, StuckTileCounter>; // Car index to StuckTileCounter.
    StuckHash m_stuckHash;

    struct CarStatus
    {
        size_t currentTargetNodeIndex = 0;
        size_t nextTargetNodeIndex = 0;
        size_t prevTargetNodeIndex = 0;

        size_t routeProgression = 0;

        size_t position = 0;
    };
    using StatusHash = std::unordered_map<size_t, CarStatus>; // Car index to CarStatus.
    StatusHash m_statusHash;

    const size_t m_numCars;

    size_t m_lapCount;

    using TimingS = std::shared_ptr<Timing>;
    TimingS m_timing;
    TrackS m_track;

    bool m_started = false;
    bool m_checkeredFlagEnabled = false;
    bool m_winnerFinished = false;
    bool m_isfinishedSignalSent = false;

    std::optional<size_t> m_bestPos;

    QTimer m_offTrackMessageTimer;

    int m_offTrackCounter = 0;

    Game & m_game;
};

#endif // RACE_HPP
