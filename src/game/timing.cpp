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

#include "timing.hpp"
#include "car.hpp"

#include <cassert>

Timing::Timing(MCUint cars, QObject *parent)
: QObject(parent)
, m_times(cars, Timing::Times())
, m_started(false)
, m_lapRecord(-1)
, m_raceRecord(-1)
{
}

void Timing::lapCompleted(MCUint index, bool isHuman)
{
    Timing::Times & times = m_times.at(index);
    times.lap++;

    const int elapsed = m_time.elapsed();
    times.lastLapTime = elapsed - times.raceTime;
    times.raceTime    = elapsed;

    // Check if a new personal record achieved.
    if (times.lastLapTime < times.recordLapTime ||
        times.recordLapTime == -1)
    {
        times.recordLapTime = times.lastLapTime;
    }

    // Check if a new lap record achieved.
    // Accept new lap records only by human players.
    if (isHuman)
    {
        if (times.lastLapTime < m_lapRecord || m_lapRecord == -1)
        {
            m_lapRecord = times.lastLapTime;

            emit lapRecordAchieved(m_lapRecord);
        }
    }
}

void Timing::setRaceCompleted(MCUint index, bool state, bool isHuman)
{
    Timing::Times & times = m_times.at(index);
    times.raceCompleted = state;
    times.raceTime = m_time.elapsed();

    if (isHuman)
    {
        if (times.raceTime < m_raceRecord || m_raceRecord == -1)
        {
            m_raceRecord = times.raceTime;

            emit raceRecordAchieved(m_raceRecord);
        }
    }
}

bool Timing::raceCompleted(MCUint index) const
{
    const Timing::Times & times = m_times.at(index);
    return times.raceCompleted;
}

void Timing::setIsActive(MCUint index, bool state)
{
    Timing::Times & times = m_times.at(index);
    times.isActive = state;
}

bool Timing::isActive(MCUint index) const
{
    const Timing::Times & times = m_times.at(index);
    return times.isActive;
}

int Timing::lap(MCUint index) const
{
    return m_times.at(index).lap;
}

int Timing::leadersLap() const
{
    int maxLap = 0;

    for (unsigned int index = 0; index < m_times.size(); index++)
    {
        const int lap = m_times.at(index).lap;
        if (lap > maxLap)
        {
            maxLap = lap;
        }
    }

    return maxLap;
}

int Timing::currentLapTime(MCUint index) const
{
    if (!m_started)
    {
        return 0;
    }

    const Timing::Times & times = m_times.at(index);
    return m_time.elapsed() - times.raceTime;
}

int Timing::recordLapTime(MCUint index) const
{
    if (!m_started)
    {
        return -1;
    }

    return m_times.at(index).recordLapTime;
}

int Timing::raceTime() const
{
    if (!m_started)
    {
        return 0;
    }

    return m_time.elapsed();
}

int Timing::raceTime(MCUint index) const
{
    if (!m_times.at(index).raceCompleted)
    {
        return raceTime();
    }
    else
    {
        return m_times.at(index).raceTime;
    }
}

int Timing::recordRaceTime(MCUint index) const
{
    if (!m_started)
    {
        return -1;
    }

    return m_times.at(index).recordRaceTime;
}

int Timing::lapRecord() const
{
    return m_lapRecord;
}

void Timing::setLapRecord(int msecs)
{
    m_lapRecord = msecs;
}

int Timing::raceRecord() const
{
    return m_raceRecord;
}

void Timing::setRaceRecord(int msecs)
{
    m_raceRecord = msecs;
}

int Timing::lastLapTime(MCUint index) const
{
    if (!m_started)
    {
        return -1;
    }

    return m_times.at(index).lastLapTime;
}

void Timing::start()
{
    m_time = QTime(0, 0, 0, 0);
    m_time.start();
    m_started = true;
}

void Timing::stop()
{
    m_started = false;
}

void Timing::reset()
{
    m_time    = QTime(0, 0, 0, 0);
    m_started = false;

    for (Timing::Times & time : m_times)
    {
        time = Timing::Times();
    }
}

std::wstring Timing::msecsToString(int msec)
{
    if (msec < 0)
    {
        return L"--:--.--";
    }

    const int hh = msec / 3600000;
    const int hr = msec % 3600000;
    const int mm = hr   / 60000;
    const int mr = hr   % 60000;
    const int ss = mr   / 1000;
    const int ms = mr   % 1000;

    const QTime time(hh, mm, ss, ms);
    std::wstring strTime(time.toString("mm:ss.zzz").toStdWString());
    strTime.erase(strTime.length() - 1, 1); // Reduce .zzz to .zz
    return strTime;
}

