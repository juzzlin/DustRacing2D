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
#include <string>

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
    void lapCompleted(const Car & car);

    //! Returns last lap time in msecs for the given car or
    //! -1 if invalid car or time not set.
    int lastLapTime(const Car & car);

    //! Returns the current lap time in msecs for the given car or
    //! -1 if invalid car or time not set.
    int currentTime(const Car & car);

    //! Returns the record lap time in msecs for the given car or
    //! -1 if invalid car or time not set.
    int recordTime(const Car & car);

    //! Returns true, if new record just achieved.
    bool newRecordActive(const Car & car) const;

    //! Force the state of active new record.
    void setNewRecordActive(const Car & car, bool state);

    //! Starts the timing.
    void start();

    //! Stops the timing.
    void stop();

    //! Adds the given car to the timing.
    void addCar(const Car & car);

    //! Removes the given car from the timing.
    void removeCar(const Car & car);

    //! Converts msecs to string "hh:mm:ss.zzz".
    std::string msecsToString(int msec) const;

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
        , newRecordActive(false)
        {}

        int lastLapTime;
        int recordLapTime;
        int totalTime;
        int lap;
        bool newRecordActive;
    };

    QHash<const Car *, Timing::Times> m_times;
    QTime m_time;
};

#endif // TIMING_H
