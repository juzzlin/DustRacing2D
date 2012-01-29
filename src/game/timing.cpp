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

    times.newRecordActive = false;
    if (times.lastLapTime < times.recordLapTime ||
        times.recordLapTime == -1)
    {
        times.recordLapTime   = times.lastLapTime;
        times.newRecordActive = true;
    }
}

int Timing::lap(MCUint index) const
{
    assert(index < m_times.size());
    return m_times[index].lap;
}

int Timing::currentTime(MCUint index) const
{
    assert(index < m_times.size());
    const Timing::Times & times = m_times[index];
    return m_time.elapsed() - times.totalTime;
}

int Timing::recordTime(MCUint index) const
{
    assert(index < m_times.size());
    return m_times[index].recordLapTime;
}

int Timing::lastLapTime(MCUint index) const
{
    assert(index < m_times.size());
    return m_times[index].lastLapTime;
}

bool Timing::newRecordActive(MCUint index) const
{
    assert(index < m_times.size());
    return m_times[index].newRecordActive;
}

void Timing::setNewRecordActive(MCUint index, bool state)
{
    assert(index < m_times.size());
    m_times[index].newRecordActive = state;
}

void Timing::start()
{
    m_time = QTime(0, 0, 0, 0);
    m_time.start();
}

void Timing::stop()
{
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

