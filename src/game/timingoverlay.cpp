// This file is part of Dust Racing 2D.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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
, m_showRecordTime(true)
{
    assert(Scene::NUM_CARS == static_cast<int>(m_posTexts.size()) - 1);
    m_text.setShadowOffset(2, -2);
}

void TimingOverlay::setCarToFollow(const Car & car)
{
    m_car = &car;
    m_carStatusView.setCarToFollow(car);
}

void TimingOverlay::setTiming(Timing & timing)
{
    m_timing = &timing;
}

void TimingOverlay::setRace(Race & race)
{
    m_race = &race;
}

void TimingOverlay::render()
{
    if (m_car && m_timing && m_race)
    {
        renderCurrentLap();

        renderPosition();

        renderSpeed();

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

    m_text.setGlyphSize(20, 20);

    // Render the current lap number
    std::stringstream ss;
    ss << QObject::tr(" LAP:").toStdString() << (leadersLap <= laps ? leadersLap : laps) << "/" << laps;

    const MCGLColor white(1.0, 1.0, 1.0);

    m_text.setText(ss.str());
    m_text.setColor(white);
    m_text.render(0, height() - m_text.height(), nullptr, m_font);
}

void TimingOverlay::renderPosition()
{
    const int pos        = m_race->getPositionOfCar(*m_car);
    const int lap        = m_timing->lap(m_car->index()) + 1;
    const int leadersLap = m_timing->leadersLap() + 1;

    std::stringstream ss;
    ss << QObject::tr(" POS:").toStdString() << m_posTexts.at(pos);

    const int lapDiff = leadersLap - lap;
    if (lapDiff > 0)
    {
        ss << "+"
           << lapDiff
           << (lapDiff == 1 ? QObject::tr("LAP").toStdString() : QObject::tr("LAPS").toStdString());
    }

    const MCGLColor yellow(1.0, 1.0, 0.0);

    m_text.setText(ss.str());
    m_text.setColor(yellow);
    m_text.render(0, height() - m_text.height() * 2, nullptr, m_font);
}

void TimingOverlay::renderSpeed()
{
    const MCGLColor white(1.0, 1.0, 1.0);
    const MCGLColor yellow(1.0, 1.0, 0.0);
    const MCGLColor red(1.0, 0.0, 0.0);

    int speed = m_car->speedInKmh();
    speed = speed < 0 ? 0 : speed;

    std::stringstream ss;
    ss << " " << speed;
    if (speed < 100)
    {
        m_text.setColor(white);
    }
    else if (speed < 200)
    {
        m_text.setColor(yellow);
    }
    else
    {
        m_text.setColor(red);
    }

    m_text.setText(ss.str());
    m_text.setGlyphSize(40, 40);
    const int h = m_text.height();
    m_text.render(0, h, nullptr, m_font);

    m_text.setText(QObject::tr(" KM/H").toStdString());
    m_text.setGlyphSize(20, 20);
    m_text.setColor(white);
    m_text.render(0, 2 * m_text.height() + h, nullptr, m_font);
}

void TimingOverlay::renderCurrentLapTime()
{
    const int lastLapTime = m_timing->lastLapTime(m_car->index());
    const int currentLapTime = m_timing->currentTime(m_car->index());
    const MCGLColor red(1.0, 0.0, 0.0);
    const MCGLColor white(1.0, 1.0, 1.0);

    std::stringstream ss;
    if (m_timing->raceCompleted(m_car->index()))
    {
        ss << "  " << m_timing->msecsToString(lastLapTime);

        m_text.setColor(white);
    }
    else
    {
        ss << "  " << m_timing->msecsToString(currentLapTime);

        // Set color to white, if lastLapTime is not set.
        if (lastLapTime == -1 || currentLapTime == lastLapTime)
        {
            m_text.setColor(white);
        }
        // Set color to green, if current time is ahead of the last lap time.
        else if (currentLapTime < lastLapTime)
        {
            const MCGLColor green(0.0, 1.0, 0.0);
            m_text.setColor(green);
        }
        // Set color to red (current time is slower than the last lap time).
        else
        {
            m_text.setColor(red);
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

void TimingOverlay::renderLastLapTime()
{
    const int lastLapTime = m_timing->lastLapTime(m_car->index());
    const MCGLColor white(1.0, 1.0, 1.0);

    m_text.setGlyphSize(20, 20);
    m_text.setColor(white);

    std::stringstream ss;
    ss << QObject::tr("L:").toStdString() << m_timing->msecsToString(lastLapTime);
    m_text.setText(ss.str());
    m_text.render(
        width() - m_text.width(),
        height() - m_text.height() * 2,
        nullptr,
        m_font);
}

void TimingOverlay::renderRecordLapTime()
{
    if (m_showRecordTime)
    {
        const int recordLapTime = m_timing->lapRecord();
        const MCGLColor white(1.0, 1.0, 1.0);

        m_text.setGlyphSize(20, 20);
        m_text.setColor(white);

        std::stringstream ss;
        ss << QObject::tr("R:").toStdString() << m_timing->msecsToString(recordLapTime);
        m_text.setText(ss.str());
        m_text.render(
            width()  - m_text.width(),
            height() - m_text.height() * 3,
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
    // Blink the record time a couple of times if a new record time set.
    // 60 Hz update rate is assumed here.

    static int blink   = 0;
    static int blinked = 0;

    if (m_timing->newLapRecordAchieved())
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
            m_timing->setNewLapRecordAchieved(false);
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
