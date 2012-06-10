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
        const int recordLapTime  = m_pTiming->recordTime(index);

        const unsigned int pos   = m_pRace->getPositionOfCar(*m_pCar);
        const unsigned int laps  = m_pRace->lapCount();

        // Render the current lap number
        {
            std::stringstream ss;
            ss << " LAP:" << leadersLap << "/" << laps;
            MCTextureText lapText(ss.str());
            lapText.setGlyphSize(20, 20);
            lapText.setColor(1.0, 1.0, 1.0);
            lapText.setShadowOffset(shadowX, shadowY);
            lapText.renderWithShadow(0, height() - lapText.textHeight(),
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

            MCTextureText posText(ss.str());
            posText.setGlyphSize(20, 20);
            posText.setColor(1.0, 1.0, 0.0);
            posText.setShadowOffset(shadowX, shadowY);
            posText.renderWithShadow(0, height() - posText.textHeight() * 2,
                nullptr, m_defaultMonospace);
        }

        // Render speed
        {
            int speed = m_pCar->speedInKmh();
            speed = speed < 0 ? 0 : speed;

            MCTextureText speedText("");
            std::stringstream ss;
            ss << " " << speed;
            if (speed < 100)
            {
                speedText.setColor(1.0, 1.0, 1.0);
            }
            else if (speed < 200)
            {
                speedText.setColor(1.0, 1.0, 0.0);
            }
            else
            {
                speedText.setColor(1.0, 0.0, 0.0);
            }

            speedText.setText(ss.str());
            speedText.setGlyphSize(40, 40);
            speedText.setShadowOffset(shadowX, shadowY);
            const int h = speedText.textHeight();
            speedText.renderWithShadow(0, h, nullptr, m_defaultMonospace);

            speedText.setText(" KM/H");
            speedText.setGlyphSize(20, 20);
            speedText.setColor(1.0, 1.0, 1.0);
            speedText.renderWithShadow(0, 2 * speedText.textHeight() + h,
                nullptr, m_defaultMonospace);
        }

        // Render the current lap time
        {
            std::string currentLapTimeStr = m_pTiming->msecsToString(currentLapTime);
            std::stringstream ss;
            ss << "  " << currentLapTimeStr;
            MCTextureText currentLapTimeText(ss.str());
            currentLapTimeText.setGlyphSize(20, 20);

            // Set color to white, if lastLapTime is not set.
            if (lastLapTime == -1)
            {
                currentLapTimeText.setColor(1.0, 1.0, 1.0);
            }
            // Set color to green, if current time is ahead of the last lap time.
            else if (currentLapTime < lastLapTime)
            {
                currentLapTimeText.setColor(0.0, 1.0, 0.0);
            }
            // Set color to red (current time is slower than the last lap time).
            else
            {
                currentLapTimeText.setColor(1.0, 0.0, 0.0);
            }

            currentLapTimeText.setShadowOffset(shadowX, shadowY);
            currentLapTimeText.renderWithShadow(
                width() - currentLapTimeText.textWidth(),
                height() - currentLapTimeText.textHeight(),
                nullptr,
                m_defaultMonospace);
        }

        // Render the last lap time
        {
            std::string lastLapTimeStr = m_pTiming->msecsToString(lastLapTime);
            std::stringstream ss;
            ss << "L:" << lastLapTimeStr;
            MCTextureText lastLapTimeText(ss.str());
            lastLapTimeText.setGlyphSize(20, 20);
            lastLapTimeText.setShadowOffset(shadowX, shadowY);
            lastLapTimeText.renderWithShadow(
                width() - lastLapTimeText.textWidth(),
                height() - lastLapTimeText.textHeight() * 2,
                nullptr,
                m_defaultMonospace);
        }

        // Render the record lap time
        {
            static int blink   = 0;
            static int blinked = 0;
            static bool show   = true;

            // Blink the record time a couple of times if a new record time set.
            // 100 Hz update rate is assumed here.
            if (m_pTiming->newRecordActive(m_pCar->index()))
            {
                if (blinked < 8)
                {
                    blink++;
                    if (blink > 15)
                    {
                        show  = !show;
                        blink = 0;
                        blinked++;
                    }
                }
                else
                {
                    m_pTiming->setNewRecordActive(m_pCar->index(), false);
                    blinked = 0;
                    show = true;
                }
            }
            else
            {
                show = true;
            }

            if (show)
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
