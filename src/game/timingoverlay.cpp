// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "timingoverlay.hpp"
#include "race.hpp"
#include "timing.hpp"
#include "car.hpp"
#include <MCCamera>
#include <MCTextureFontManager>
#include <sstream>

TimingOverlay::TimingOverlay()
: m_fontManager(MCTextureFontManager::instance())
, m_font(m_fontManager.font("default"))
, m_text("")
, m_pCar(nullptr)
, m_pTiming(nullptr)
, m_pRace(nullptr)
, m_posTexts({"---", "1st", "2nd", "3rd", "4th", "5th", "6th", "7th", "8th", "9th", "10th"})
, m_showRecordTime(true)
{
    m_text.setShadowOffset(2, -2);
}

void TimingOverlay::setCarToFollow(const Car & car)
{
    m_pCar = &car;
}

void TimingOverlay::setTiming(Timing & timing)
{
    m_pTiming = &timing;
}

void TimingOverlay::setRace(Race & race)
{
    m_pRace = &race;
}

void TimingOverlay::render()
{
    // TODO: Refactor and optimize.
    if (m_pCar && m_pTiming && m_pRace)
    {
        const int index          = m_pCar->index();
        const int leadersLap     = m_pTiming->leadersLap() + 1;
        const int lap            = m_pTiming->lap(m_pCar->index()) + 1;
        const int currentLapTime = m_pTiming->currentTime(index);
        const int lastLapTime    = m_pTiming->lastLapTime(index);
        const int recordLapTime  = m_pTiming->lapRecord();
        const int pos            = m_pRace->getPositionOfCar(*m_pCar);
        const int laps           = m_pRace->lapCount();

        m_text.setGlyphSize(20, 20);

        // Render the current lap number
        {
            std::stringstream ss;

            if (leadersLap <= laps)
            {
                ss << " LAP:" << leadersLap << "/" << laps;
            }
            else
            {
                ss << " WINNER FINISHED";
            }

            m_text.setText(ss.str());
            m_text.setColor(1.0, 1.0, 1.0);
            m_text.render(0, height() - m_text.height(), nullptr, m_font);
        }

        // Render the position
        {
            std::stringstream ss;
            ss << " POS:" << m_posTexts.at(pos);

            const int lapDiff = leadersLap - lap;
            if (lapDiff > 0)
            {
                ss << "+" << lapDiff << (lapDiff == 1 ? "LAP" : "LAPS");
            }

            m_text.setText(ss.str());
            m_text.setColor(1.0, 1.0, 0.0);
            m_text.render(0, height() - m_text.height() * 2, nullptr, m_font);
        }

        // Render speed
        {
            int speed = m_pCar->speedInKmh();
            speed = speed < 0 ? 0 : speed;

            std::stringstream ss;
            ss << " " << speed;
            if (speed < 100)
            {
                m_text.setColor(1.0, 1.0, 1.0);
            }
            else if (speed < 200)
            {
                m_text.setColor(1.0, 1.0, 0.0);
            }
            else
            {
                m_text.setColor(1.0, 0.0, 0.0);
            }

            m_text.setText(ss.str());
            m_text.setGlyphSize(40, 40);
            const int h = m_text.height();
            m_text.render(0, h, nullptr, m_font);

            m_text.setText(" KM/H");
            m_text.setGlyphSize(20, 20);
            m_text.setColor(1.0, 1.0, 1.0);
            m_text.render(0, 2 * m_text.height() + h, nullptr, m_font);
        }

        // Render the current lap time
        {
            std::stringstream ss;

            if (m_pTiming->raceCompleted(index))
            {
                ss << "  " << m_pTiming->msecsToString(lastLapTime);

                m_text.setColor(1.0, 1.0, 1.0);
            }
            else
            {
                ss << "  " << m_pTiming->msecsToString(currentLapTime);

                // Set color to white, if lastLapTime is not set.
                if (lastLapTime == -1 || currentLapTime == lastLapTime)
                {
                    m_text.setColor(1.0, 1.0, 1.0);
                }
                // Set color to green, if current time is ahead of the last lap time.
                else if (currentLapTime < lastLapTime)
                {
                    m_text.setColor(0.0, 1.0, 0.0);
                }
                // Set color to red (current time is slower than the last lap time).
                else
                {
                    m_text.setColor(1.0, 0.0, 0.0);
                }
            }

            m_text.setText(ss.str());
            m_text.setGlyphSize(20, 20);
            m_text.render(
                width() - m_text.width(),
                height() - m_text.height(),
                nullptr,
                m_font);
        }

        m_text.setGlyphSize(20, 20);
        m_text.setColor(1.0, 1.0, 1.0);

        // Render the last lap time
        {
            std::stringstream ss;
            ss << "L:" << m_pTiming->msecsToString(lastLapTime);
            m_text.setText(ss.str());
            m_text.render(
                width() - m_text.width(),
                height() - m_text.height() * 2,
                nullptr,
                m_font);
        }

        // Render the record lap time
        if (m_showRecordTime)
        {
            std::stringstream ss;
            ss << "R:" << m_pTiming->msecsToString(recordLapTime);
            m_text.setText(ss.str());
            m_text.render(
                width()  - m_text.width(),
                height() - m_text.height() * 3,
                nullptr,
                m_font);
        }
    }
}

bool TimingOverlay::update()
{
    // Blink the record time a couple of times if a new record time set.
    // 60 Hz update rate is assumed here.

    static int blink   = 0;
    static int blinked = 0;

    if (m_pTiming->newLapRecordAchieved())
    {
        if (blinked < 8)
        {
            blink++;
            if (blink > 15)
            {
                m_showRecordTime = !m_showRecordTime;
                blink = 0;
                blinked++;
            }
        }
        else
        {
            m_pTiming->setNewLapRecordAchieved(false);
            blinked = 0;
            m_showRecordTime = true;
        }
    }
    else
    {
        m_showRecordTime = true;
    }

    return true;
}
