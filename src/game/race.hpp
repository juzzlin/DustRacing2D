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

#include <QObject>
#include <QString>
#include <QTimer>
#include <MCObject>
#include <memory>
#include <unordered_map>
#include <vector>

#include "audiosource.hpp"
#include "timing.hpp"

class Car;
class Game;
class OffTrackDetector;
class Route;
class Track;
class TrackTile;

//! Class that controls the race event, checkpoints and timing.
class Race : public AudioSource
{
    Q_OBJECT

public:

    //! Constructor.
    Race(Game & game, unsigned int numCars);

    //! Destructor.
    virtual ~Race();

    //! Init the race.
    void init(Track & track, int lapCount);

    //! Return true, if race has started.
    bool started();

    //! Get the position of the given car (0 == N/A, 1 == first, 2 == second..).
    unsigned int getPositionOfCar(const Car & car) const;

    //! Get the number of laps.
    int lapCount() const;

    //! Add a car to the race.
    void addCar(Car & car);

    //! Removes cars from the race.
    void removeCars();

    //! Get the timing object.
    Timing & timing();

    bool checkeredFlagEnabled() const;

    Car & getLeadingCar() const;

signals:

    void finished();

    void messageRequested(QString message);

    void tiresChanged(const Car & car);

public slots:

    void start();

    void stopEngineSounds();

    void update();

    void pitStop(Car & car);

private:

    void checkForNewBestPosition(const Car & car);

    void checkIfCarIsStuck(Car & car);

    void checkIfCarIsOffTrack(Car & car);

    void checkIfLapIsCompleted(Car & car, const Route & route, unsigned int currentTargetNodeIndex);

    void clearPositions();

    void clearRaceFlags();

    void createStartGridObjects();

    void initCars();

    void initTiming();

    bool isRaceFinished() const;

    void moveCarOntoPreviousCheckPoint(Car & car);

    void setTrack(Track & track, int lapCount);

    void translateCarsToStartPositions();

    void updateRouteProgress(Car & car);

    typedef std::vector<Car *> CarVector;
    CarVector m_cars;

    // Used only to keep references to the start grid objects.
    // They are common for all races and created only once.
    typedef std::vector<MCObjectPtr> StartGridObjectVector;
    StartGridObjectVector m_startGridObjects;

    // Map from car route progression to car order vector.
    // Tracks the order of the cars in the route.
    typedef std::unordered_map<int, std::vector<int> > PositionHash;
    mutable PositionHash m_positions;

    typedef std::shared_ptr<OffTrackDetector> OffTrackDetectorPtr;
    typedef std::vector<OffTrackDetectorPtr> OTDVector;
    OTDVector m_offTrackDetectors;

    // Data structure to determine if a car is stuck.
    // In that case we move the car onto the previous check point.
    typedef std::pair<TrackTile *, int> StuckTileCounter; // Tile pointer and counter.
    typedef std::unordered_map<int, StuckTileCounter> StuckHash; // Car index to StuckTileCounter.
    StuckHash m_stuckHash;

    int m_numCars;

    int m_lapCount;

    Timing m_timing;

    Track * m_track;

    bool m_started;

    bool m_checkeredFlagEnabled;

    bool m_winnerFinished;

    bool m_isfinishedSignalSent;

    int m_bestPos;

    QTimer m_offTrackMessageTimer;

    int m_offTrackCounter;

    Game & m_game;
};

#endif // RACE_HPP
