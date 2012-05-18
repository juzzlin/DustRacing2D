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
#include "startlights.hpp"
#include "track.hpp"

StateMachine::StateMachine(Startlights & startlights)
: m_state(Init)
, m_startlights(startlights)
, m_pTrack(nullptr)
{
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
        m_state = GameTransitionIn;
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
