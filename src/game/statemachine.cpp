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

#include "menumanager.hpp"
#include "race.hpp"
#include "renderer.hpp"
#include "startlights.hpp"
#include "track.hpp"

#include <cassert>

StateMachine * StateMachine::m_instance = nullptr;

StateMachine::StateMachine()
: m_state(Init)
, m_startlights(nullptr)
, m_race(nullptr)
, m_renderer(nullptr)
, m_track(nullptr)
, m_fadeValue(0.0)
, m_returnToMenu(false)
{
    assert(!StateMachine::m_instance);
    StateMachine::m_instance = this;
}

StateMachine::~StateMachine()
{
    StateMachine::m_instance = nullptr;
}

StateMachine & StateMachine::instance()
{
    assert(StateMachine::m_instance);
    return *StateMachine::m_instance;
}

bool StateMachine::update()
{
    switch (m_state)
    {
    case Init:

        m_state     = Intro;
        m_fadeValue = 0.0;

        break;

    case Intro:

        assert(m_renderer);

        if (m_fadeValue < 2.0)
        {
            m_fadeValue += 0.01;
            m_renderer->setFadeValue(std::min(m_fadeValue, MCFloat(1.0)));

            // Avoid a non-black screen immediately after the game starts.
            if (m_fadeValue > 0.02)
            {
                m_renderer->setEnabled(true);
            }
        }
        else
        {
            m_fadeValue = 1.0;
            m_state     = Menu;
        }

        break;

    case Menu:

        m_returnToMenu = false;
        m_renderer->setFadeValue(1.0);

        if (MenuManager::instance().done())
        {
            m_state = GameTransitionIn;
        }

        break;

    case GameTransitionIn:

        assert(m_track);

        if (!m_track->update())
        {
            m_startlights->reset();
            m_state = DoStartlights;
        }

        break;

    case DoStartlights:

        assert(m_startlights);

        if (!m_startlights->update())
        {
            m_state = Play;
        }

        break;

    case Play:

        assert(m_race);

        if (m_race->finished())
        {
            m_fadeValue = 3.0;
            m_state     = GameTransitionOut;
        }
        else if (m_returnToMenu)
        {
            m_fadeValue = 1.0;
            m_state     = GameTransitionOut;
        }

        break;

    case GameTransitionOut:

        assert(m_renderer);

        if (m_fadeValue >= 0.01)
        {
            m_fadeValue -= 0.01;
            m_renderer->setFadeValue(std::min(m_fadeValue, MCFloat(1.0)));
        }
        else
        {
            m_fadeValue = 1.0;
            m_state     = Menu;

            MenuManager::instance().enterMenu("trackSelection");
        }

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
