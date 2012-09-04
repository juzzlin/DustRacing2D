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

#ifndef TIMING_HPP
#define TIMING_HPP

#include <QObject>
#include <QTime>
#include <string>
#include <vector>
#include <MCTypes>

class Car;

class Timing : public QObject
{
    Q_OBJECT

public:

    //! Constructor.
    explicit Timing(MCUint cars, QObject * parent = nullptr);

    //! Completes the current lap for the given car.
    void lapCompleted(MCUint index, bool isHuman);

    //! Completes the race for the given car.
    void setRaceCompleted(MCUint index, bool state);

    //! Returns if the race is completed for the given car.
    bool raceCompleted(MCUint index) const;

    //! Returns the current lap for the given car.
    int lap(MCUint index) const;

    //! Returns the lap for the leading car.
    int leadersLap() const;

    //! Returns last lap time in msecs for the given car or
    //! -1 if invalid car or time not set.
    int lastLapTime(MCUint index) const;

    //! Returns the current lap time in msecs for the given car or
    //! -1 if invalid car or time not set.
    int currentTime(MCUint index) const;

    //! Returns the record lap time in msecs for the given car or
    //! -1 if invalid car or time not set.
    int recordTime(MCUint index) const;

    //! Return the current lap record or -1 if not set.
    int lapRecord() const;

    //! Set the current lap record.
    void setLapRecord(int msecs);

    //! Returns true, if new lap record just achieved.
    bool newLapRecordAchieved() const;

    //! Force the state of active new lap record.
    void setNewLapRecordAchieved(bool state);

    //! Starts the timing.
    void start();

    //! Stops the timing.
    void stop();

    //! Resets the timing.
    void reset();

    //! Converts msecs to string "mm:ss.zz".
    static std::string msecsToString(int msec);

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
        , raceCompleted(false)
        {}

        int lastLapTime;
        int recordLapTime; // Personal best
        int totalTime;
        int lap;
        bool raceCompleted;
    };

    std::vector<Timing::Times> m_times;
    QTime m_time;
    bool m_started;
    int m_lapRecord;
    bool m_newLapRecordAchieved;
};

#endif // TIMING_HPP
