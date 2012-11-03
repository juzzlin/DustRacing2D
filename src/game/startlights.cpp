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
#include "messageoverlay.hpp"

Startlights::Startlights(MessageOverlay & messageOverlay)
: m_state(LightsInit)
, m_counter(0)
, m_stepsPerState(60)
, m_messageOverlay(messageOverlay)
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

bool Startlights::update()
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
        return true;

    case LightsAppear:
        m_animation.update();
        if (updateCounter(second))
        {
            m_state = LightsFirstRow;
            m_messageOverlay.addMessage("3");
        }
        return true;

    case LightsFirstRow:
        if (updateCounter(second))
        {
            m_state = LightsSecondRow;
            m_messageOverlay.addMessage("2");
        }
        return true;

    case LightsSecondRow:
        if (updateCounter(second))
        {
            m_state = LightsThirdRow;
            m_messageOverlay.addMessage("1");
        }
        return true;

    case LightsThirdRow:
        if (updateCounter(second))
        {
            m_state = LightsGo;
            m_messageOverlay.addMessage("GO!!!");
            InputHandler::setEnabled(true);
            emit raceStarted();
        }
        return true;

    case LightsGo:
        if (updateCounter(second))
        {
            m_state = LightsDisappear;
            m_animation.init(
                m_pos,
                m_pos,
                MCVector3dF(m_pos.i(), 3 * m_height / 2, 0),
                second / 3);
        }
        return true;

    case LightsDisappear:
        if (m_animation.update())
        {
            m_state = LightsEnd;
        }
        return true;

    case LightsEnd:
        return false;
    }

    return true;
}

void Startlights::setDimensions(MCUint width, MCUint height)
{
    m_width  = width;
    m_height = height;
}

void Startlights::reset()
{
    m_state = LightsInit;
}

Startlights::LightState Startlights::state() const
{
    return m_state;
}

const MCVector3dF & Startlights::pos() const
{
    return m_pos;
}
