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

#include "timing.h"
#include "car.h"

Timing::Timing(QObject *parent)
  : QObject(parent)
{
}

void Timing::lapCompleted(const Car & car)
{
    Timing::Times & times = m_times[&car];
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

int Timing::lap(const Car & car) const
{
    if (m_times.contains(&car))
    {
        const Timing::Times & times = m_times[&car];
        return times.lap;
    }

    return -1;
}

int Timing::currentTime(const Car & car) const
{
    if (m_times.contains(&car))
    {
        const Timing::Times & times = m_times[&car];
        return m_time.elapsed() - times.totalTime;
    }

    return -1;
}

int Timing::recordTime(const Car & car) const
{
    if (m_times.contains(&car))
    {
        const Timing::Times & times = m_times[&car];
        return times.recordLapTime;
    }

    return -1;
}

int Timing::lastLapTime(const Car & car) const
{
    if (m_times.contains(&car))
    {
        return m_times[&car].lastLapTime;
    }

    return -1;
}

void Timing::addCar(const Car & car)
{
    m_times[&car] = Timing::Times();
}

void Timing::removeCar(const Car & car)
{
    m_times.remove(&car);
}

bool Timing::newRecordActive(const Car & car) const
{
    if (m_times.contains(&car))
    {
        return m_times[&car].newRecordActive;
    }

    return false;
}

void Timing::setNewRecordActive(const Car & car, bool state)
{
    if (m_times.contains(&car))
    {
        m_times[&car].newRecordActive = state;
    }
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

Timing::~Timing()
{
}
