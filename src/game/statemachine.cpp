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

#include "statemachine.hpp"

#include <MenuManager>
#include <cassert>

StateMachine * StateMachine::m_instance = nullptr;

StateMachine::StateMachine()
: m_state(Init)
, m_oldState(Init)
, m_raceFinished(false)
{
    assert(!StateMachine::m_instance);
    StateMachine::m_instance = this;

    m_stateToFunctionMap[Init]              = std::bind(&StateMachine::stateInit,              this);
    m_stateToFunctionMap[DoIntro]           = std::bind(&StateMachine::stateDoIntro,           this);
    m_stateToFunctionMap[Menu]              = std::bind(&StateMachine::stateMenu,              this);
    m_stateToFunctionMap[MenuTransitionIn]  = std::bind(&StateMachine::stateMenuTransitionIn,  this);
    m_stateToFunctionMap[MenuTransitionOut] = std::bind(&StateMachine::stateMenuTransitionOut, this);
    m_stateToFunctionMap[GameTransitionIn]  = std::bind(&StateMachine::stateGameTransitionIn,  this);
    m_stateToFunctionMap[GameTransitionOut] = std::bind(&StateMachine::stateGameTransitionOut, this);
    m_stateToFunctionMap[DoStartlights]     = std::bind(&StateMachine::stateDoStartlights,     this);
    m_stateToFunctionMap[Play]              = std::bind(&StateMachine::statePlay,              this);
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

void StateMachine::quit()
{
    if (m_state == StateMachine::Play)
    {
        m_state = GameTransitionOut;
    }
    else if (m_state == StateMachine::Menu)
    {
        emit exitGameRequested();
    }
}

bool StateMachine::update()
{
    // Run the state function on transition
    if (m_state == Init || m_oldState != m_state)
    {
        m_oldState = m_state;
        m_stateToFunctionMap[m_state]();
    }

    // Transition logic that needs to be constantly updated
    if (m_state == Menu)
    {
        if (MTFH::MenuManager::instance().isDone())
        {
            m_state = MenuTransitionOut;
        }
    }

    return true;
}

void StateMachine::endFadeIn()
{
    if (m_state == DoIntro)
    {
        m_state = Menu;
    }
    else if (m_state == GameTransitionIn)
    {
        m_state = DoStartlights;
    }
    else if (m_state == MenuTransitionIn)
    {
        m_state = Menu;
    }
}

void StateMachine::endFadeOut()
{
    if (m_state == MenuTransitionOut)
    {
        m_state = GameTransitionIn;
    }
    else if (m_state == GameTransitionOut)
    {
        m_state = MenuTransitionIn;
    }
}

void StateMachine::endStartlightAnimation()
{
    m_state = Play;
}

void StateMachine::stateInit()
{
    m_state = DoIntro;
}

void StateMachine::stateDoIntro()
{
    emit fadeInRequested(0, 5000, 5000);
    emit renderingEnabled(true);
}

void StateMachine::stateMenu()
{
    // Re-init the current menu
    MTFH::MenuManager::instance().enterCurrentMenu();
}

void StateMachine::stateMenuTransitionIn()
{
    emit fadeInRequested(0, 2000, 0);
    emit soundsStopped();
}

void StateMachine::stateMenuTransitionOut()
{
    emit fadeOutRequested(0, 2000, 0);
}

void StateMachine::stateGameTransitionIn()
{
    emit fadeInRequested(0, 2000, 0);
}

void StateMachine::stateGameTransitionOut()
{
    if (m_raceFinished)
    {
        emit fadeOutRequested(5000, 5000, 0);
    }
    else
    {
        emit fadeOutRequested(0, 2000, 0);
    }
}

void StateMachine::stateDoStartlights()
{
    emit startlightAnimationRequested();
}

void StateMachine::statePlay()
{
    m_raceFinished = false;
}

void StateMachine::finishRace()
{
    m_state = GameTransitionOut;
    m_raceFinished = true;
}

StateMachine::State StateMachine::state() const
{
    return m_state;
}

void StateMachine::reset()
{
    m_state = Init;
}
