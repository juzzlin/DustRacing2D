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

#ifndef TIMING_HPP
#define TIMING_HPP

#include <QObject>
#include <string>
#include <vector>

class Car;

class Timing : public QObject
{
    Q_OBJECT

public:
    //! Constructor.
    explicit Timing(size_t cars, QObject * parent = nullptr);

    //! Completes the current lap for the given car.
    void setLapCompleted(size_t index, bool isHuman);

    //! Completes the race for the given car.
    void setRaceCompleted(size_t index, bool state, bool isHuman);

    //! \return if the race is completed for the given car.
    bool raceCompleted(size_t index) const;

    //! Toggle timing activity of the given car.
    void setIsActive(size_t index, bool state);

    //! \return timing activity of the given car.
    bool isActive(size_t index) const;

    //! \return the current lap for the given car.
    size_t lap(size_t index) const;

    //! \return the lap for the leading car.
    size_t leadersLap() const;

    //! \return the race time in msecs.
    int raceTime() const;

    //! \return the race time of given car in msecs.
    int raceTime(size_t index) const;

    /*! \return the record race time for the given car in msecs or
     *  -1 if invalid car or time not set. */
    int recordRaceTime(size_t index) const;

    /*! \return last lap time in msecs for the given car or
     *  -1 if invalid car or time not set. */
    int lastLapTime(size_t index) const;

    /*! \return the current lap time in msecs for the given car or
     *  -1 if invalid car or time not set. */
    int currentLapTime(size_t index) const;

    /*! \return the record lap time in msecs for the given car or
     *  -1 if invalid car or time not set. */
    int recordLapTime(size_t index) const;

    //! \return the current lap record or -1 if not set.
    int lapRecord() const;

    //! Set the current lap record.
    void setLapRecord(int msecs);

    //! \return the current race record or -1 if not set.
    int raceRecord() const;

    //! Set the current race record.
    void setRaceRecord(int msecs);

    //! Starts the timing.
    void start();

    //! Stops the timing.
    void stop();

    //! Resets the timing.
    void reset();

    //! Increase timer assuming 60 Hz update rate
    void tick();

    //! Converts msecs to string "mm:ss.zz".
    static std::wstring msecsToString(int msec);

signals:

    void lapCompleted(size_t index, int msec);

    void lapRecordAchieved(int msec);

    void raceRecordAchieved(int msec);

private:
    //! Timing structure.
    struct Times
    {
        int lastLapTime = -1;

        int recordLapTime = -1; // Personal best

        int raceTime = 0;

        int recordRaceTime = 0; // Personal best

        size_t lap = 0;

        bool raceCompleted = false;

        bool isActive = true;
    };

    std::vector<Timing::Times> m_times;

    int m_time;

    bool m_started;

    int m_lapRecord;

    int m_raceRecord;
};

#endif // TIMING_HPP
