// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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
#include "inputhandler.hpp"

#include <MenuManager>
#include <cassert>

StateMachine * StateMachine::m_instance = nullptr;

StateMachine::StateMachine(InputHandler & inputHandler)
  : m_state(State::Init)
  , m_oldState(State::Init)
  , m_raceFinished(false)
  , m_inputHandler(inputHandler)
{
    assert(!StateMachine::m_instance);
    StateMachine::m_instance = this;

    m_stateToFunctionMap[State::Init] = std::bind(&StateMachine::stateInit, this);
    m_stateToFunctionMap[State::DoIntro] = std::bind(&StateMachine::stateDoIntro, this);
    m_stateToFunctionMap[State::Menu] = std::bind(&StateMachine::stateMenu, this);
    m_stateToFunctionMap[State::MenuTransitionIn] = std::bind(&StateMachine::stateMenuTransitionIn, this);
    m_stateToFunctionMap[State::MenuTransitionOut] = std::bind(&StateMachine::stateMenuTransitionOut, this);
    m_stateToFunctionMap[State::GameTransitionIn] = std::bind(&StateMachine::stateGameTransitionIn, this);
    m_stateToFunctionMap[State::GameTransitionOut] = std::bind(&StateMachine::stateGameTransitionOut, this);
    m_stateToFunctionMap[State::DoStartlights] = std::bind(&StateMachine::stateDoStartlights, this);
    m_stateToFunctionMap[State::Play] = std::bind(&StateMachine::statePlay, this);
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
    if (m_state == StateMachine::State::Play)
    {
        m_state = State::GameTransitionOut;
    }
    else if (m_state == StateMachine::State::Menu)
    {
        emit exitGameRequested();
    }
}

bool StateMachine::update()
{
    // Run the state function on transition
    if (m_state == State::Init || m_oldState != m_state)
    {
        m_oldState = m_state;
        m_stateToFunctionMap[m_state]();
    }

    // Transition logic that needs to be constantly updated
    if (m_state == State::Menu)
    {
        if (MTFH::MenuManager::instance().isDone())
        {
            m_state = State::MenuTransitionOut;
        }
    }

    return true;
}

void StateMachine::endFadeIn()
{
    switch (m_state)
    {
    case State::GameTransitionIn:
        m_state = State::DoStartlights;
        break;
    case State::MenuTransitionIn:
        m_state = State::Menu;
        break;
    default:
        break;
    }
}

void StateMachine::endFadeOut()
{
    switch (m_state)
    {
    case State::MenuTransitionOut:
        m_state = State::GameTransitionIn;
        break;
    case State::GameTransitionOut:
        m_state = State::MenuTransitionIn;
        break;
    default:
        break;
    }
}

void StateMachine::endStartlightAnimation()
{
    m_state = State::Play;
}

void StateMachine::stateInit()
{
    m_state = State::DoIntro;
}

void StateMachine::stateDoIntro()
{
    m_timer.singleShot(3000, [&]() {
        if (m_state == State::DoIntro)
        {
            m_state = State::Menu;
        }
    });

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
    emit fadeOutFlashRequested(0, 2000, 0);
}

void StateMachine::stateGameTransitionIn()
{
    emit fadeInRequested(0, 2000, 0);

    m_inputHandler.reset();
}

void StateMachine::stateGameTransitionOut()
{
    if (m_raceFinished)
    {
        emit fadeOutRequested(10000, 10000, 0);
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
    m_state = State::GameTransitionOut;
    m_raceFinished = true;
}

StateMachine::State StateMachine::state() const
{
    return m_state;
}

void StateMachine::reset()
{
    m_state = State::Init;
}
