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

#include <MCAssetManager>
#include <MCCamera>

#include <cassert>
#include <sstream>

#include <QObject> // For QObject::tr()
#include <QTimer>

static const int RACE_TIME_POS = 1;
static const int CURRENT_LAP_TIME_POS = 2;
static const int LAST_LAP_TIME_POS = 3;
static const int RECORD_LAP_TIME_POS = 4;
static const int GLYPH_W_TIMES = 15;
static const int GLYPH_H_TIMES = 15;
static const int GLYPH_W_POS = 20;
static const int GLYPH_H_POS = 20;

static const MCGLColor RED(1.0, 0.0, 0.0);
static const MCGLColor GREEN(0.0, 1.0, 0.0);
static const MCGLColor YELLOW(1.0, 1.0, 0.0);
static const MCGLColor WHITE(1.0, 1.0, 1.0);

TimingOverlay::TimingOverlay()
  : m_fontManager(MCAssetManager::textureFontManager())
  , m_font(m_fontManager.font(Game::instance().fontName()))
  , m_text(L"")
  , m_car(nullptr)
  , m_posTexts({ QObject::tr("---").toStdWString(),
                 QObject::tr("1st").toStdWString(),
                 QObject::tr("2nd").toStdWString(),
                 QObject::tr("3rd").toStdWString(),
                 QObject::tr("4th").toStdWString(),
                 QObject::tr("5th").toStdWString(),
                 QObject::tr("6th").toStdWString(),
                 QObject::tr("7th").toStdWString(),
                 QObject::tr("8th").toStdWString(),
                 QObject::tr("9th").toStdWString(),
                 QObject::tr("10th").toStdWString(),
                 QObject::tr("11th").toStdWString(),
                 QObject::tr("12th").toStdWString() })
  , m_showLapRecordTime(true)
  , m_showRaceTime(true)
  , m_showCarStatus(true)
{
    assert(Scene::carCount() + 1 <= m_posTexts.size());
    m_text.setShadowOffset(2, -2);
}

void TimingOverlay::setCarToFollow(const Car & car)
{
    m_car = &car;
    m_carStatusView.setCarToFollow(car);
}

void TimingOverlay::setRace(std::shared_ptr<Race> race)
{
    m_race = race;

    connect(&m_race->timing(), &Timing::lapRecordAchieved, this, &TimingOverlay::setLapRecord);
    connect(&m_race->timing(), &Timing::raceRecordAchieved, this, &TimingOverlay::setRaceRecord);
    connect(m_race.get(), &Race::tiresChanged, this, static_cast<void (TimingOverlay::*)()>(&TimingOverlay::blinkCarStatus));
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
        QTimer::singleShot(250, this, &TimingOverlay::blinkLapRecord);
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
        QTimer::singleShot(250, this, &TimingOverlay::blinkRaceRecord);
        m_showRaceTime = !m_showRaceTime;
        count++;
    }
    else
    {
        m_showRaceTime = true;
        count = 0;
    }
}

void TimingOverlay::blinkCarStatus()
{
    static int count = 0;
    if (count < 10)
    {
        QTimer::singleShot(250, this, static_cast<void (TimingOverlay::*)()>(&TimingOverlay::blinkCarStatus));
        m_showCarStatus = !m_showCarStatus;
        count++;
    }
    else
    {
        m_showCarStatus = true;
        count = 0;
    }
}

void TimingOverlay::blinkCarStatus(const Car & car)
{
    if (m_car == &car)
    {
        blinkCarStatus();
    }
}

void TimingOverlay::render()
{
    if (m_car && m_race)
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
    const auto leadersLap = m_race->timing().leadersLap() + 1;
    const auto laps = m_race->lapCount();

    m_text.setGlyphSize(GLYPH_W_POS, GLYPH_H_POS);

    // Render the current lap number
    std::wstringstream ss;
    ss << QObject::tr(" LAP:").toStdWString() << (leadersLap <= laps ? leadersLap : laps) << "/" << laps;

    m_text.setText(ss.str());
    m_text.setColor(WHITE);
    m_text.render(0, height() - m_text.height(m_font), nullptr, m_font);
}

void TimingOverlay::renderPosition()
{
    const auto pos = m_race->position(*m_car);
    const auto lap = m_race->timing().lap(m_car->index()) + 1;
    const auto leadersLap = m_race->timing().leadersLap() + 1;

    m_text.setGlyphSize(GLYPH_W_POS, GLYPH_H_POS);

    std::wstringstream ss;
    //: Position/rank
    ss << QObject::tr(" POS:").toStdWString();
    ss << m_posTexts.at(pos);

    const int lapDiff = static_cast<int>(leadersLap) - static_cast<int>(lap);
    if (lapDiff > 0)
    {
        ss << "+"
           << lapDiff
           << (lapDiff == 1 ? QObject::tr("LAP").toStdWString() : QObject::tr("LAPS").toStdWString());
    }

    m_text.setText(ss.str());
    m_text.setColor(YELLOW);
    m_text.render(0, height() - m_text.height(m_font) * 2, nullptr, m_font);
}

void TimingOverlay::renderSpeed()
{
    // Minimaps replace speedometer in the split modes
    if (!Game::instance().hasTwoHumanPlayers())
    {
        int speed = m_car->speedInKmh();
        speed = speed < 0 ? 0 : speed;

        std::wstringstream ss;
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

        const int h = static_cast<int>(m_text.height(m_font));
        m_text.render(0, h, nullptr, m_font);

        m_text.setText(QObject::tr(" KM/H").toStdWString());
        m_text.setGlyphSize(20, 20);
        m_text.setColor(WHITE);
        m_text.render(0, 2 * m_text.height(m_font) + h, nullptr, m_font);
    }
}

void TimingOverlay::renderCurrentLapTime()
{
    const int lastLapTime = m_race->timing().lastLapTime(m_car->index());
    const int currentLapTime = m_race->timing().currentLapTime(m_car->index());

    std::wstringstream ss;
    ss << QObject::tr("LAP:").toStdWString();
    if (m_race->timing().raceCompleted(m_car->index()))
    {
        ss << m_race->timing().msecsToString(lastLapTime);

        m_text.setColor(WHITE);
    }
    else
    {
        ss << m_race->timing().msecsToString(currentLapTime);

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
      width() - m_text.width(m_font),
      height() - m_text.height(m_font) * CURRENT_LAP_TIME_POS,
      nullptr,
      m_font);
}

void TimingOverlay::renderLastLapTime()
{
    const int lastLapTime = m_race->timing().lastLapTime(m_car->index());

    m_text.setGlyphSize(GLYPH_W_TIMES, GLYPH_H_TIMES);
    m_text.setColor(WHITE);

    std::wstringstream ss;
    //: Last lap time
    ss << QObject::tr("L:").toStdWString() << m_race->timing().msecsToString(lastLapTime);
    m_text.setText(ss.str());
    m_text.render(
      width() - m_text.width(m_font),
      height() - m_text.height(m_font) * LAST_LAP_TIME_POS,
      nullptr,
      m_font);
}

void TimingOverlay::renderRecordLapTime()
{
    if (m_showLapRecordTime)
    {
        const int recordLapTime = m_race->timing().lapRecord();

        m_text.setGlyphSize(GLYPH_W_TIMES, GLYPH_H_TIMES);
        m_text.setColor(WHITE);

        std::wstringstream ss;
        //: Lap record time
        ss << QObject::tr("R:").toStdWString() << m_race->timing().msecsToString(recordLapTime);
        m_text.setText(ss.str());
        m_text.render(
          width() - m_text.width(m_font),
          height() - m_text.height(m_font) * RECORD_LAP_TIME_POS,
          nullptr,
          m_font);
    }
}

void TimingOverlay::renderRaceTime()
{
    if (m_showRaceTime)
    {
        const int raceTime = m_race->timing().raceTime(m_car->index());

        m_text.setGlyphSize(GLYPH_W_TIMES, GLYPH_H_TIMES);
        m_text.setColor(WHITE);

        std::wstringstream ss;
        //: Total race time
        ss << QObject::tr("TOT:").toStdWString() << m_race->timing().msecsToString(raceTime);
        m_text.setText(ss.str());
        m_text.render(
          width() - m_text.width(m_font),
          height() - m_text.height(m_font) * RACE_TIME_POS,
          nullptr,
          m_font);
    }
}

void TimingOverlay::renderCarStatusView()
{
    if (m_showCarStatus)
    {
        m_carStatusView.setPos(width() - m_carStatusView.width(), m_carStatusView.height() + 10);
        m_carStatusView.render();
    }
}

bool TimingOverlay::update()
{
    return true;
}
