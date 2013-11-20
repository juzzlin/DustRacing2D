// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can REDistribute it and/or modify
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

#include "timingoverlay.hpp"

#include "car.hpp"
#include "game.hpp"
#include "race.hpp"
#include "scene.hpp"
#include "timing.hpp"

#include <MCCamera>
#include <MCAssetManager>

#include <cassert>
#include <sstream>

#include <QObject> // For QObject::tr()
#include <QTimer>

static const int RACE_TIME_POS        = 1;
static const int CURRENT_LAP_TIME_POS = 2;
static const int LAST_LAP_TIME_POS    = 3;
static const int RECORD_LAP_TIME_POS  = 4;
static const int GLYPH_W_TIMES        = 15;
static const int GLYPH_H_TIMES        = 15;
static const int GLYPH_W_POS          = 20;
static const int GLYPH_H_POS          = 20;

static const MCGLColor RED    (1.0, 0.0, 0.0);
static const MCGLColor GREEN  (0.0, 1.0, 0.0);
static const MCGLColor YELLOW (1.0, 1.0, 0.0);
static const MCGLColor WHITE  (1.0, 1.0, 1.0);

TimingOverlay::TimingOverlay()
: m_fontManager(MCAssetManager::textureFontManager())
, m_font(m_fontManager.font("default"))
, m_text("")
, m_car(nullptr)
, m_timing(nullptr)
, m_race(nullptr)
, m_posTexts({
    QObject::tr("---").toStdString(),
    QObject::tr("1st").toStdString(),
    QObject::tr("2nd").toStdString(),
    QObject::tr("3rd").toStdString(),
    QObject::tr("4th").toStdString(),
    QObject::tr("5th").toStdString(),
    QObject::tr("6th").toStdString(),
    QObject::tr("7th").toStdString(),
    QObject::tr("8th").toStdString(),
    QObject::tr("9th").toStdString(),
    QObject::tr("10th").toStdString(),
    QObject::tr("11th").toStdString(),
    QObject::tr("12th").toStdString()})
, m_showLapRecordTime(true)
, m_showRaceTime(true)
{
    assert(Scene::NUM_CARS == static_cast<int>(m_posTexts.size()) - 1);
    m_text.setShadowOffset(2, -2);
}

void TimingOverlay::setCarToFollow(const Car & car)
{
    m_car = &car;
    m_carStatusView.setCarToFollow(car);
}

void TimingOverlay::setRace(Race & race)
{
    m_race   = &race;
    m_timing = &m_race->timing();

    connect(m_timing, SIGNAL(lapRecordAchieved(int)), this, SLOT(setLapRecord(int)));
    connect(m_timing, SIGNAL(raceRecordAchieved(int)), this, SLOT(setRaceRecord(int)));
}

void TimingOverlay::setLapRecord(int)
{
    blinkLapRecord();
}

void TimingOverlay::setRaceRecord(int)
{
    blinkRaceRecord();
}

void TimingOverlay::blinkLapRecord()
{
    static int count = 0;
    if (count < 10)
    {
        QTimer::singleShot(250, this, SLOT(blinkLapRecord()));
        m_showLapRecordTime = !m_showLapRecordTime;
        count++;
    }
    else
    {
        m_showLapRecordTime = true;
        count = 0;
    }
}

void TimingOverlay::blinkRaceRecord()
{
    static int count = 0;
    if (count < 10)
    {
        QTimer::singleShot(250, this, SLOT(blinkRaceRecord()));
        m_showRaceTime = !m_showRaceTime;
        count++;
    }
    else
    {
        m_showRaceTime = true;
        count = 0;
    }
}

void TimingOverlay::render()
{
    if (m_car && m_timing && m_race)
    {
        renderCurrentLap();

        renderPosition();

        renderSpeed();

        renderRaceTime();

        renderCurrentLapTime();

        renderLastLapTime();

        renderRecordLapTime();

        renderCarStatusView();
    }
}

void TimingOverlay::renderCurrentLap()
{
    const int leadersLap = m_timing->leadersLap() + 1;
    const int laps       = m_race->lapCount();

    m_text.setGlyphSize(GLYPH_W_POS, GLYPH_H_POS);

    // Render the current lap number
    std::stringstream ss;
    ss << QObject::tr(" LAP:").toStdString() << (leadersLap <= laps ? leadersLap : laps) << "/" << laps;

    m_text.setText(ss.str());
    m_text.setColor(WHITE);
    m_text.render(0, height() - m_text.height(), nullptr, m_font);
}

void TimingOverlay::renderPosition()
{
    const int pos        = m_race->getPositionOfCar(*m_car);
    const int lap        = m_timing->lap(m_car->index()) + 1;
    const int leadersLap = m_timing->leadersLap() + 1;

    m_text.setGlyphSize(GLYPH_W_POS, GLYPH_H_POS);

    std::stringstream ss;
    ss << QObject::tr(" POS:").toStdString();
    ss << m_posTexts.at(pos);

    const int lapDiff = leadersLap - lap;
    if (lapDiff > 0)
    {
        ss << "+"
           << lapDiff
           << (lapDiff == 1 ? QObject::tr("LAP").toStdString() : QObject::tr("LAPS").toStdString());
    }

    m_text.setText(ss.str());

    if (pos > 8)
    {
        m_text.setColor(RED);
    }
    else if (pos > 4)
    {
        m_text.setColor(YELLOW);
    }
    else if (pos > 1)
    {
        m_text.setColor(WHITE);
    }
    else if (pos == 1)
    {
        m_text.setColor(GREEN);
    }
    else
    {
        m_text.setColor(WHITE);
    }

    m_text.render(0, height() - m_text.height() * 2, nullptr, m_font);
}

void TimingOverlay::renderSpeed()
{
    int speed = m_car->speedInKmh();
    speed = speed < 0 ? 0 : speed;

    std::stringstream ss;
    ss << " " << speed;
    if (speed < 100)
    {
        m_text.setColor(WHITE);
    }
    else if (speed < 200)
    {
        m_text.setColor(YELLOW);
    }
    else
    {
        m_text.setColor(RED);
    }

    m_text.setText(ss.str());
    m_text.setGlyphSize(40, 40);
    const int h = m_text.height();
    m_text.render(0, h, nullptr, m_font);

    m_text.setText(QObject::tr(" KM/H").toStdString());
    m_text.setGlyphSize(20, 20);
    m_text.setColor(WHITE);
    m_text.render(0, 2 * m_text.height() + h, nullptr, m_font);
}

void TimingOverlay::renderCurrentLapTime()
{
    const int lastLapTime = m_timing->lastLapTime(m_car->index());
    const int currentLapTime = m_timing->currentLapTime(m_car->index());

    std::stringstream ss;
    ss << QObject::tr("LAP:").toStdString();
    if (m_timing->raceCompleted(m_car->index()))
    {
        ss << m_timing->msecsToString(lastLapTime);

        m_text.setColor(WHITE);
    }
    else
    {
        ss << m_timing->msecsToString(currentLapTime);

        // Set color to WHITE, if lastLapTime is not set.
        if (lastLapTime == -1 || currentLapTime == lastLapTime)
        {
            m_text.setColor(WHITE);
        }
        // Set color to GREEN, if current time is ahead of the last lap time.
        else if (currentLapTime < lastLapTime)
        {
            m_text.setColor(GREEN);
        }
        // Set color to RED (current time is slower than the last lap time).
        else
        {
            m_text.setColor(RED);
        }
    }

    m_text.setText(ss.str());
    m_text.setGlyphSize(GLYPH_W_TIMES, GLYPH_H_TIMES);
    m_text.render(
        width()  - m_text.width(),
        height() - m_text.height() * CURRENT_LAP_TIME_POS,
        nullptr,
        m_font);
}

void TimingOverlay::renderLastLapTime()
{
    const int lastLapTime = m_timing->lastLapTime(m_car->index());

    m_text.setGlyphSize(GLYPH_W_TIMES, GLYPH_H_TIMES);
    m_text.setColor(WHITE);

    std::stringstream ss;
    ss << QObject::tr("L:").toStdString() << m_timing->msecsToString(lastLapTime);
    m_text.setText(ss.str());
    m_text.render(
        width()  - m_text.width(),
        height() - m_text.height() * LAST_LAP_TIME_POS,
        nullptr,
        m_font);
}

void TimingOverlay::renderRecordLapTime()
{
    if (m_showLapRecordTime)
    {
        const int recordLapTime = m_timing->lapRecord();

        m_text.setGlyphSize(GLYPH_W_TIMES, GLYPH_H_TIMES);
        m_text.setColor(WHITE);

        std::stringstream ss;
        ss << QObject::tr("R:").toStdString() << m_timing->msecsToString(recordLapTime);
        m_text.setText(ss.str());
        m_text.render(
            width()  - m_text.width(),
            height() - m_text.height() * RECORD_LAP_TIME_POS,
            nullptr,
            m_font);
    }
}

void TimingOverlay::renderRaceTime()
{
    if (m_showRaceTime)
    {
        const int raceTime = m_timing->raceTime(m_car->index());

        m_text.setGlyphSize(GLYPH_W_TIMES, GLYPH_H_TIMES);
        m_text.setColor(WHITE);

        std::stringstream ss;
        ss << QObject::tr("TOT:").toStdString() << m_timing->msecsToString(raceTime);
        m_text.setText(ss.str());
        m_text.render(
            width()  - m_text.width(),
            height() - m_text.height() * RACE_TIME_POS,
            nullptr,
            m_font);
    }
}

void TimingOverlay::renderCarStatusView()
{
    m_carStatusView.setPos(width() - m_carStatusView.width(), m_carStatusView.height() + 10);
    m_carStatusView.render();
}

bool TimingOverlay::update()
{
    return true;
}
