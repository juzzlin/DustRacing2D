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

#ifndef RACE_HPP
#define RACE_HPP

#include <QObject>

#include <memory>
#include <unordered_map>
#include <vector>

#include "timing.hpp"

class Car;
class Game;
class MessageOverlay;
class OffTrackDetector;
class Route;
class Track;
class TrackTile;

//! Class that controls the race event, checkpoints and timing.
class Race : public QObject
{
    Q_OBJECT

public:

    //! Constructor.
    Race(const Game & game, unsigned int numCars, MessageOverlay & messageOverlay);

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

    //! Return true if the race has finished
    bool finished() const;

    Car & getLeadingCar() const;

public slots:

    //! Start the race and timing.
    void start();

    //! Update situation.
    void update();

private:

    void checkForNewLapRecord();

    void checkForNewBestPosition(const Car & car);

    void checkIfCarIsStuck(Car & car);

    bool isLapCompleted(Car & car, const Route & route, unsigned int currentTargetNodeIndex);

    void moveCarOntoPreviousCheckPoint(Car & car);

    void setTrack(Track & track);

    void translateCarsToStartPositions();

    void updateRouteProgress(Car & car);

    typedef std::vector<Car *> CarVector;
    CarVector m_cars;

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

    int              m_lapCount;
    Timing           m_timing;
    Track          * m_track;
    bool             m_started;
    bool             m_checkeredFlagEnabled;
    bool             m_winnerFinished;
    int              m_bestPos;
    MessageOverlay & m_messageOverlay;
    const Game     & m_game;
};

#endif // RACE_HPP
