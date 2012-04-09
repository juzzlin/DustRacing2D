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

#include "startlights.hpp"
#include "inputhandler.hpp"
#include "race.hpp"

Startlights::Startlights(Race & race)
  : m_state(LightsInit)
  , m_counter(0)
  , m_stepsPerState(60)
  , m_race(race)
{
}

bool Startlights::updateCounter(MCUint limit)
{
    if (++m_counter > limit)
    {
        m_counter = 0;
        return true;
    }
    return false;
}

void Startlights::update()
{
    const MCUint second = m_stepsPerState;
    switch (m_state)
    {
    case LightsInit:
        m_pos = MCVector3dF(m_width / 2, 3 * m_height / 2, 0);
        m_animation.init(
             m_pos,
             m_pos,
             MCVector3dF(m_pos.i(), m_height / 2, 0),
             second / 3);
        m_state = LightsAppear;
        InputHandler::setEnabled(false);
        break;

    case LightsAppear:
        m_animation.update();
        if (updateCounter(second))
        {
            m_state = LightsFirstRow;
        }
        break;

    case LightsFirstRow:
        if (updateCounter(second))
        {
            m_state = LightsSecondRow;
        }
        break;

    case LightsSecondRow:
        if (updateCounter(second))
        {
            m_state = LightsThirdRow;
        }
        break;

    case LightsThirdRow:
        if (updateCounter(second))
        {
            m_state = LightsGo;
            InputHandler::setEnabled(true);
            m_race.start();
        }
        break;

    case LightsGo:
        if (updateCounter(second))
        {
            m_state = LightsDisappear;
            m_animation.init(m_pos,
                             m_pos,
                             MCVector3dF(m_pos.i(), 3 * m_height / 2, 0),
                             second / 3);
        }
        break;

    case LightsDisappear:
        m_animation.update();
        break;

    case LightsEnd:
        break;
    }
}

void Startlights::setDimensions(MCUint width, MCUint height)
{
    m_width  = width;
    m_height = height;
}

Startlights::LightState Startlights::state() const
{
    return m_state;
}

const MCVector3dF & Startlights::pos() const
{
    return m_pos;
}
