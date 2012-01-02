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

void Timing::lapCompleted(Car & car)
{
    Timing::Times & times = m_times[&car];
    times.lap++;

    const int elapsed = m_time.elapsed();
    times.lastLapTime = elapsed - times.totalTime;
    times.totalTime   = elapsed;

    if (times.lastLapTime < times.recordLapTime)
    {
        times.recordLapTime = times.lastLapTime;
    }
}

int Timing::lastLapTime(Car & car)
{
    if (m_times.contains(&car))
    {
        return m_times[&car].lastLapTime;
    }

    return -1;
}

void Timing::addCar(Car & car)
{
    m_times[&car] = Timing::Times();
}

void Timing::removeCar(Car & car)
{
    m_times.remove(&car);
}

void Timing::start()
{
    m_time = QTime(0, 0, 0, 0);
    m_time.start();
}

void Timing::stop()
{
}

QString Timing::msecToString(int msec) const
{
    int hh = msec / 3600000;
    int mm = (msec % 3600000) / 60000;
    int ss = (msec % 3600000 % 60000) / 1000;
    int ms = (msec % 3600000 % 60000 % 1000);

    QTime time(hh, mm, ss, ms);
    return time.toString("hh:mm:ss.zzz");
}

Timing::~Timing()
{
}
