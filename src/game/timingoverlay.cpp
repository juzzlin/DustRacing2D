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

#include "timingoverlay.h"
#include "timing.h"

#include "MiniCore/Core/MCCamera"
#include "MiniCore/Text/MCTextureFontManager"
#include "MiniCore/Text/MCTextureText"

#include <sstream>

TimingOverlay::TimingOverlay()
  : m_fontManager(MCTextureFontManager::instance())
  , m_defaultMonospace(m_fontManager.font("default"))
  , m_pCar(nullptr)
  , m_pTiming(nullptr)
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

void TimingOverlay::render()
{
    if (m_pCar && m_pTiming)
    {
        const int shadowY = -2;
        const int shadowX = 2;

        // Render the current lap
        {
            const int lap = m_pTiming->lap(*m_pCar) + 1;
            std::stringstream ss;
            ss << " LAP:" << lap;
            MCTextureText lapText(ss.str());
            lapText.setGlyphSize(20, 20);
            lapText.renderShadow(
                shadowX + 0,
                shadowY + height() - lapText.textHeight(),
                nullptr,
                m_defaultMonospace);
            lapText.render(
                0,
                height() - lapText.textHeight(),
                nullptr,
                m_defaultMonospace);
        }

        // Render the current lap time
        {
            const int currentLapTime = m_pTiming->currentTime(*m_pCar);
            std::string currentLapTimeStr = m_pTiming->msecsToString(currentLapTime);
            std::stringstream ss;
            ss << "  " << currentLapTimeStr;
            MCTextureText currentLapTimeText(ss.str());
            currentLapTimeText.setGlyphSize(20, 20);
            currentLapTimeText.renderShadow(
                shadowX + width() - currentLapTimeText.textWidth(),
                shadowY + height() - currentLapTimeText.textHeight(),
                nullptr,
                m_defaultMonospace);
            currentLapTimeText.render(
                width() - currentLapTimeText.textWidth(),
                height() - currentLapTimeText.textHeight(),
                nullptr,
                m_defaultMonospace);
        }

        // Render the last lap time
        {
            const int lastLapTime = m_pTiming->lastLapTime(*m_pCar);
            std::string lastLapTimeStr = m_pTiming->msecsToString(lastLapTime);
            std::stringstream ss;
            ss << "L:" << lastLapTimeStr;
            MCTextureText lastLapTimeText(ss.str());
            lastLapTimeText.setGlyphSize(20, 20);
            lastLapTimeText.renderShadow(
                shadowX + width() - lastLapTimeText.textWidth(),
                shadowY + height() - lastLapTimeText.textHeight() * 2,
                nullptr,
                m_defaultMonospace);
            lastLapTimeText.render(
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

            // 60 Hz update rate is assumed here
            if (m_pTiming->newRecordActive(*m_pCar))
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
                    m_pTiming->setNewRecordActive(*m_pCar, false);
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
                const int recordLapTime = m_pTiming->recordTime(*m_pCar);
                std::string recordLapTimeStr = m_pTiming->msecsToString(recordLapTime);
                std::stringstream ss;
                ss << "R:" << recordLapTimeStr;
                MCTextureText recordLapTimeText(ss.str());
                recordLapTimeText.setGlyphSize(20, 20);
                recordLapTimeText.renderShadow(
                    shadowX + width() - recordLapTimeText.textWidth(),
                    shadowY + height() - recordLapTimeText.textHeight() * 3,
                    nullptr,
                    m_defaultMonospace);
                recordLapTimeText.render(
                    width() - recordLapTimeText.textWidth(),
                    height() - recordLapTimeText.textHeight() * 3,
                    nullptr,
                    m_defaultMonospace);
            }
        }
    }
}

TimingOverlay::~TimingOverlay()
{
}
