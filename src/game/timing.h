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
#include <QTime>

#include <string>
#include <vector>

#include "MiniCore/Core/MCTypes"

class Car;

class Timing : public QObject
{
    Q_OBJECT

public:

    //! Constructor.
    explicit Timing(MCUint cars, QObject * parent = nullptr);

    //! Completes the current lap for the given car.
    void lapCompleted(MCUint index);

    //! Returns the current lap for the given car.
    int lap(MCUint index) const;

    //! Returns last lap time in msecs for the given car or
    //! -1 if invalid car or time not set.
    int lastLapTime(MCUint index) const;

    //! Returns the current lap time in msecs for the given car or
    //! -1 if invalid car or time not set.
    int currentTime(MCUint index) const;

    //! Returns the record lap time in msecs for the given car or
    //! -1 if invalid car or time not set.
    int recordTime(MCUint index) const;

    //! Returns true, if new record just achieved.
    bool newRecordActive(MCUint index) const;

    //! Force the state of active new record.
    void setNewRecordActive(MCUint index, bool state);

    //! Starts the timing.
    void start();

    //! Stops the timing.
    void stop();

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

    std::vector<Timing::Times> m_times;
    QTime m_time;
};

#endif // TIMING_H
