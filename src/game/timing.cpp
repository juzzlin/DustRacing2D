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

#include "timing.hpp"
#include "car.hpp"

#include <cassert>

Timing::Timing(MCUint cars, QObject *parent)
  : QObject(parent)
  , m_times(cars, Timing::Times())
  , m_started(false)
{
}

void Timing::lapCompleted(MCUint index)
{
    assert(index < m_times.size());

    Timing::Times & times = m_times[index];
    times.lap++;

    const int elapsed = m_time.elapsed();
    times.lastLapTime = elapsed - times.totalTime;
    times.totalTime   = elapsed;

    times.newRecordAchieved = false;
    if (times.lastLapTime < times.recordLapTime ||
        times.recordLapTime == -1)
    {
        times.recordLapTime     = times.lastLapTime;
        times.newRecordAchieved = true;
    }
}

void Timing::setRaceCompleted(MCUint index, bool state)
{
    assert(index < m_times.size());
    Timing::Times & times = m_times[index];
    times.raceCompleted = state;
}

bool Timing::raceCompleted(MCUint index) const
{
    assert(index < m_times.size());
    const Timing::Times & times = m_times[index];
    return times.raceCompleted;
}

int Timing::lap(MCUint index) const
{
    assert(index < m_times.size());
    return m_times[index].lap;
}

int Timing::leadersLap() const
{
    int maxLap = 0;

    for (unsigned int index = 0; index < m_times.size(); index++)
    {
        const int lap = m_times[index].lap;
        if (lap > maxLap)
        {
            maxLap = lap;
        }
    }

    return maxLap;
}

int Timing::currentTime(MCUint index) const
{
    if (!m_started)
    {
        return 0;
    }

    assert(index < m_times.size());
    const Timing::Times & times = m_times[index];
    return m_time.elapsed() - times.totalTime;
}

int Timing::recordTime(MCUint index) const
{
    if (!m_started)
    {
        return -1;
    }

    assert(index < m_times.size());
    return m_times[index].recordLapTime;
}

int Timing::lastLapTime(MCUint index) const
{
    if (!m_started)
    {
        return -1;
    }

    assert(index < m_times.size());
    return m_times[index].lastLapTime;
}

bool Timing::newRecordAchieved(MCUint index) const
{
    assert(index < m_times.size());
    return m_times[index].newRecordAchieved;
}

void Timing::setNewRecordAchieved(MCUint index, bool state)
{
    assert(index < m_times.size());
    m_times[index].newRecordAchieved = state;
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

std::string Timing::msecsToString(int msec) const
{
    if (msec < 0)
    {
        return "--:--:--.---";
    }

    const int hh = msec / 3600000;
    const int hr = msec % 3600000;
    const int mm = hr   / 60000;
    const int mr = hr   % 60000;
    const int ss = mr   / 1000;
    const int ms = mr   % 1000;

    const QTime time(hh, mm, ss, ms);
    return time.toString("hh:mm:ss.zzz").toStdString();
}

