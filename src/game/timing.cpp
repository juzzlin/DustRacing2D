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

#include "simple_logger.hpp"

#include <QString>

#include <algorithm>
#include <cassert>

Timing::Timing(size_t cars, QObject * parent)
  : QObject { parent }
  , m_times { cars, Timing::Times() }
  , m_time { 0ms }
  , m_started { false }
  , m_lapRecord { -1 }
  , m_raceRecord { -1 }
{
}

void Timing::setLapCompleted(size_t index, bool isHuman)
{
    Timing::Times & times = m_times.at(index);
    times.lap++;

    times.lastLapTime = static_cast<int>(m_time.count() - times.raceTime.count());
    times.raceTime = m_time;

    juzzlin::L().debug() << "Lap (" << times.lap << ") completed for car index=" << index << ": " << times.lastLapTime;
    juzzlin::L().debug() << "Current personal best time: " << times.recordLapTime;

    // Check if a new personal record achieved.
    if (times.lastLapTime < times.recordLapTime || times.recordLapTime == -1)
    {
        times.recordLapTime = times.lastLapTime;
        juzzlin::L().debug() << "New personal best time: " << times.recordLapTime;
    }

    // Check if a new lap record achieved.
    // Accept new lap records only by human players.
    if (isHuman)
    {
        juzzlin::L().debug() << "Human lap completed: " << times.lastLapTime;
        juzzlin::L().debug() << "Current lap record: " << m_lapRecord;

        if (times.lastLapTime < m_lapRecord || m_lapRecord == -1)
        {
            m_lapRecord = times.lastLapTime;
            juzzlin::L().debug() << "New lap record: " << m_lapRecord;

            emit lapRecordAchieved(m_lapRecord);
        }

        emit lapCompleted(index, times.lastLapTime);
    }
}

void Timing::setRaceCompleted(size_t index, bool state, bool isHuman)
{
    auto && times = m_times.at(index);
    times.raceCompleted = state;
    times.raceTime = m_time;

    if (isHuman)
    {
        if (times.raceTime.count() < m_raceRecord || m_raceRecord == -1)
        {
            m_raceRecord = static_cast<int>(times.raceTime.count());
            emit raceRecordAchieved(m_raceRecord);
        }
    }
}

bool Timing::raceCompleted(size_t index) const
{
    return m_times.at(index).raceCompleted;
}

void Timing::setIsActive(size_t index, bool state)
{
    m_times.at(index).isActive = state;
}

bool Timing::isActive(size_t index) const
{
    return m_times.at(index).isActive;
}

size_t Timing::lap(size_t index) const
{
    return m_times.at(index).lap;
}

size_t Timing::leadersLap() const
{
    if (m_times.empty())
    {
        return 0;
    }
    else
    {
        return std::max_element(
                 m_times.begin(), m_times.end(),
                 [](auto const & a, auto const & b) {
                     return a.lap < b.lap;
                 })
          ->lap;
    }
}

int Timing::currentLapTime(size_t index) const
{
    return !m_started ? 0 : static_cast<int>(m_time.count() - m_times.at(index).raceTime.count());
}

int Timing::recordLapTime(size_t index) const
{
    return !m_started ? -1 : m_times.at(index).recordLapTime;
}

std::chrono::milliseconds Timing::raceTime() const
{
    return m_started ? m_time : 0ms;
}

std::chrono::milliseconds Timing::raceTime(size_t index) const
{
    return !m_times.at(index).raceCompleted ? raceTime() : m_times.at(index).raceTime;
}

int Timing::recordRaceTime(size_t index) const
{
    return !m_started ? -1 : m_times.at(index).recordRaceTime;
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

int Timing::lastLapTime(size_t index) const
{
    return !m_started ? -1 : m_times.at(index).lastLapTime;
}

void Timing::start()
{
    m_time = 0ms;
    m_started = true;
}

void Timing::stop()
{
    m_started = false;
}

void Timing::reset()
{
    m_time = 0ms;
    m_started = false;

    for (Timing::Times & time : m_times)
    {
        time = Timing::Times();
    }
}

void Timing::tick(std::chrono::milliseconds timeStep)
{
    if (m_started)
    {
        m_time += timeStep;
    }
}

std::wstring Timing::msecsToString(int msec)
{
    if (msec < 0)
    {
        return L"--:--.--";
    }

    const int hr = msec % 3600000;
    const int mm = hr / 60000;
    const int mr = hr % 60000;
    const int ss = mr / 1000;
    const int ms = mr % 1000;

    return QString {}.asprintf("%02d:%02d.%02d", mm, ss, ms / 10).toStdWString();
}
