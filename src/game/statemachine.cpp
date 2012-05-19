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

#include "statemachine.hpp"

#include "renderer.hpp"
#include "startlights.hpp"
#include "track.hpp"

StateMachine::StateMachine(
    Renderer & renderer, Startlights & startlights)
: m_state(Init)
, m_startlights(startlights)
, m_renderer(renderer)
, m_pTrack(nullptr)
, m_fadeValue(0.0f)
{
    m_renderer.setEnabled(false);
}

void StateMachine::setTrack(Track & track)
{
    m_pTrack = &track;
}

bool StateMachine::update()
{
    switch (m_state)
    {
    case Init:
        m_state = Intro;
        break;

    case Intro:

        if (m_fadeValue < 2.0f)
        {
            m_fadeValue += 0.01f;
            m_renderer.setFadeShaderEnabled(true);
            m_renderer.setFadeValue(std::min(m_fadeValue, 1.0f));

            // Avoid a non-black screen immediately after the game starts.
            if (m_fadeValue > 0.02f)
            {
                m_renderer.setEnabled(true);
            }
        }
        else
        {
            m_fadeValue = 1.0f;
            m_state = GameTransitionIn;
            m_renderer.setFadeShaderEnabled(false);
        }

        break;

    case GameTransitionIn:
        if (m_pTrack)
        {
            if (!m_pTrack->update())
            {
                m_state = DoStartlights;
            }
        }
        break;

    case DoStartlights:
        if (!m_startlights.update())
        {
            m_state = Play;
        }
        break;

    case Play:
        break;

    default:
        break;
    }

    return true;
}

StateMachine::State StateMachine::state() const
{
    return m_state;
}

void StateMachine::reset()
{
}
