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

#include "MiniCore/Core/MCCamera"
#include "MiniCore/Text/MCTextureFontManager"
#include "MiniCore/Text/MCTextureText"

#include <sstream>

TimingOverlay::TimingOverlay()
: m_fontManager(MCTextureFontManager::instance())
, m_defaultMonospace(m_fontManager.font("default"))
, m_pCar(nullptr)
, m_pTiming(nullptr)
, m_pRace(nullptr)
, m_posTexts({"---", "1st", "2nd", "3rd", "4th", "5th", "6th", "7th", "8th", "9th", "10th"})
, m_showRecordTime(true)
{
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
        const int shadowY        = -2;
        const int shadowX        =  2;
        const int index          = m_pCar->index();
        const int leadersLap     = m_pTiming->leadersLap() + 1;
        const int lap            = m_pTiming->lap(m_pCar->index()) + 1;
        const int currentLapTime = m_pTiming->currentTime(index);
        const int lastLapTime    = m_pTiming->lastLapTime(index);
        const int recordLapTime  = m_pTiming->lapRecord();

        const unsigned int pos   = m_pRace->getPositionOfCar(*m_pCar);
        const unsigned int laps  = m_pRace->lapCount();

        MCTextureText text("");

        // Render the current lap number
        {
            std::stringstream ss;
            ss << " LAP:" << leadersLap << "/" << laps;
            text.setText(ss.str());
            text.setGlyphSize(20, 20);
            text.setColor(1.0, 1.0, 1.0);
            text.setShadowOffset(shadowX, shadowY);
            text.renderWithShadow(0, height() - text.textHeight(),
                nullptr, m_defaultMonospace);
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

            text.setText(ss.str());
            text.setGlyphSize(20, 20);
            text.setColor(1.0, 1.0, 0.0);
            text.setShadowOffset(shadowX, shadowY);
            text.renderWithShadow(0, height() - text.textHeight() * 2,
                nullptr, m_defaultMonospace);
        }

        // Render speed
        {
            int speed = m_pCar->speedInKmh();
            speed = speed < 0 ? 0 : speed;

            std::stringstream ss;
            ss << " " << speed;
            if (speed < 100)
            {
                text.setColor(1.0, 1.0, 1.0);
            }
            else if (speed < 200)
            {
                text.setColor(1.0, 1.0, 0.0);
            }
            else
            {
                text.setColor(1.0, 0.0, 0.0);
            }

            text.setText(ss.str());
            text.setGlyphSize(40, 40);
            text.setShadowOffset(shadowX, shadowY);
            const int h = text.textHeight();
            text.renderWithShadow(0, h, nullptr, m_defaultMonospace);

            text.setText(" KM/H");
            text.setGlyphSize(20, 20);
            text.setColor(1.0, 1.0, 1.0);
            text.renderWithShadow(0, 2 * text.textHeight() + h,
                nullptr, m_defaultMonospace);
        }

        // Render the current lap time
        {
            std::string currentLapTimeStr = m_pTiming->msecsToString(currentLapTime);
            std::stringstream ss;
            ss << "  " << currentLapTimeStr;
            text.setText(ss.str());
            text.setGlyphSize(20, 20);

            // Set color to white, if lastLapTime is not set.
            if (lastLapTime == -1)
            {
                text.setColor(1.0, 1.0, 1.0);
            }
            // Set color to green, if current time is ahead of the last lap time.
            else if (currentLapTime < lastLapTime)
            {
                text.setColor(0.0, 1.0, 0.0);
            }
            // Set color to red (current time is slower than the last lap time).
            else
            {
                text.setColor(1.0, 0.0, 0.0);
            }

            text.setShadowOffset(shadowX, shadowY);
            text.renderWithShadow(
                width() - text.textWidth(),
                height() - text.textHeight(),
                nullptr,
                m_defaultMonospace);
        }

        // Render the last lap time
        {
            std::string lastLapTimeStr = m_pTiming->msecsToString(lastLapTime);
            std::stringstream ss;
            ss << "L:" << lastLapTimeStr;
            text.setText(ss.str());
            text.setGlyphSize(20, 20);
            text.setShadowOffset(shadowX, shadowY);
            text.renderWithShadow(
                width() - text.textWidth(),
                height() - text.textHeight() * 2,
                nullptr,
                m_defaultMonospace);
        }

        // Render the record lap time
        {
            if (m_showRecordTime)
            {
                std::string recordLapTimeStr = m_pTiming->msecsToString(recordLapTime);
                std::stringstream ss;
                ss << "R:" << recordLapTimeStr;
                MCTextureText recordLapTimeText(ss.str());
                recordLapTimeText.setGlyphSize(20, 20);
                recordLapTimeText.setShadowOffset(shadowX, shadowY);
                recordLapTimeText.renderWithShadow(
                    width() - recordLapTimeText.textWidth(),
                    height() - recordLapTimeText.textHeight() * 3,
                    nullptr,
                    m_defaultMonospace);
            }
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
