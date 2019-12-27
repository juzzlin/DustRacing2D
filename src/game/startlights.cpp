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

#include "startlights.hpp"
#include "inputhandler.hpp"

Startlights::Startlights()
  : m_state(State::Init)
  , m_counter(0)
  , m_stepsPerState(60)
{
    m_stateToFunctionMap[State::Init] = std::bind(&Startlights::stateInit, this);
    m_stateToFunctionMap[State::Appear] = std::bind(&Startlights::stateAppear, this);
    m_stateToFunctionMap[State::FirstRow] = std::bind(&Startlights::stateFirstRow, this);
    m_stateToFunctionMap[State::SecondRow] = std::bind(&Startlights::stateSecondRow, this);
    m_stateToFunctionMap[State::ThirdRow] = std::bind(&Startlights::stateThirdRow, this);
    m_stateToFunctionMap[State::Go] = std::bind(&Startlights::stateGo, this);
    m_stateToFunctionMap[State::Disappear] = std::bind(&Startlights::stateDisappear, this);
    m_stateToFunctionMap[State::End] = std::bind(&Startlights::stateEnd, this);

    connect(&m_timer, &QTimer::timeout, this, &Startlights::updateAnimation);
    m_timer.setInterval(1000 / m_stepsPerState);
}

bool Startlights::timeElapsed(size_t limit)
{
    if (++m_counter > limit)
    {
        m_counter = 0;
        return true;
    }
    return false;
}

void Startlights::updateAnimation()
{
    m_stateToFunctionMap[m_state]();
}

void Startlights::beginAnimation()
{
    m_state = State::Init;
    m_timer.start();
}

void Startlights::setDimensions(size_t width, size_t height)
{
    m_width = width;
    m_height = height;
}

Startlights::State Startlights::state() const
{
    return m_state;
}

const MCVector3dF & Startlights::pos() const
{
    return m_pos;
}

float Startlights::glowScale() const
{
    return m_glowScale;
}

void Startlights::stateInit()
{
    const size_t second = m_stepsPerState;

    m_pos = MCVector3dF(m_width / 2, 3 * m_height / 2, 0);
    m_animation.init(
      m_pos,
      m_pos,
      MCVector3dF(m_pos.i(), m_height / 2, 0),
      second / 3);
    m_state = State::Appear;
    m_glowScale = 1.0;
    InputHandler::setEnabled(false);
}

void Startlights::stateAppear()
{
    const size_t second = m_stepsPerState;

    m_animation.update();
    if (timeElapsed(second))
    {
        m_state = State::FirstRow;
        emit messageRequested("3");
    }
}

void Startlights::stateFirstRow()
{
    const size_t second = m_stepsPerState;

    if (timeElapsed(second))
    {
        m_state = State::SecondRow;
        emit messageRequested("2");
    }
}

void Startlights::stateSecondRow()
{
    const size_t second = m_stepsPerState;

    if (timeElapsed(second))
    {
        m_state = State::ThirdRow;
        emit messageRequested("1");
    }
}

void Startlights::stateThirdRow()
{
    const size_t second = m_stepsPerState;

    if (timeElapsed(second))
    {
        m_state = State::Go;
        emit messageRequested(QObject::tr("GO!!!"));
        emit raceStarted();
        InputHandler::setEnabled(true);
    }
}

void Startlights::stateGo()
{
    const size_t second = m_stepsPerState;

    if (timeElapsed(second))
    {
        m_state = State::Disappear;
        m_animation.init(
          m_pos,
          m_pos,
          MCVector3dF(m_pos.i(), 3 * m_height / 2, 0),
          second / 3);
    }

    m_glowScale *= 0.75f;
}

void Startlights::stateDisappear()
{
    if (m_animation.update())
    {
        m_state = State::End;
    }
}

void Startlights::stateEnd()
{
    m_timer.stop();
    emit animationEnded();
}
