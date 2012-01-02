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

#ifndef TIMING_H
#define TIMING_H

#include <QObject>
#include <QHash>
#include <QTime>

class Car;

class Timing : public QObject
{
    Q_OBJECT

public:

    //! Constructor.
    explicit Timing(QObject * parent = nullptr);

    //! Destructor.
    ~Timing();

    //! Completes the current lap for the given car.
    void lapCompleted(Car & car);

    //! Returns last lap time in 10th of secs for the given car or
    //! -1 if invalid car or time not set.
    int lastLapTime(Car & car);

    //! Starts the timing.
    void start();

    //! Stops the timing.
    void stop();

    //! Adds the given car to the timing.
    void addCar(Car & car);

    //! Removes the given car from the timing.
    void removeCar(Car & car);

    //! Converts milliseconds to string "hh:mm:ss.zzz".
    QString msecToString(int msec) const;

private:

    //! Timing structure.
    class Times
    {
    public:
        Times()
        : lastLapTime(-1)
        , recordLapTime(-1)
        , totalTime(0)
        , lap(0)
        {}

        int lastLapTime;
        int recordLapTime;
        int totalTime;
        int lap;
    };

    QHash<Car *, Timing::Times> m_times;
    QTime m_time;
};

#endif // TIMING_H
