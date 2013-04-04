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

#include "game.hpp"
#include "intro.hpp"
#include "race.hpp"
#include "renderer.hpp"
#include "startlights.hpp"
#include "track.hpp"

#include <MenuManager>
#include <cassert>

StateMachine * StateMachine::m_instance = nullptr;

static const float FADE_SPEED_MENU = 0.02;
static const float FADE_SPEED_GAME = 0.01;

StateMachine::StateMachine()
: m_state(Init)
, m_isFading(false)
, m_game(nullptr)
, m_intro(nullptr)
, m_startlights(nullptr)
, m_race(nullptr)
, m_renderer(nullptr)
, m_track(nullptr)
, m_fadeValue(0.0)
, m_returnToMenu(false)
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

void StateMachine::setGame(Game & game)
{
    m_game = &game;
}

void StateMachine::setIntro(Intro & intro)
{
    m_intro = &intro;
}

void StateMachine::setTrack(Track & track)
{
    m_track = &track;
}

void StateMachine::setRenderer(Renderer & renderer)
{
    m_renderer = &renderer;
    m_renderer->setEnabled(false);
}

void StateMachine::setRace(Race & race)
{
    m_race = &race;
}

void StateMachine::setStartlights(Startlights & startlights)
{
    m_startlights = &startlights;
}

void StateMachine::quit()
{
    if (m_state == StateMachine::Play)
    {
        m_returnToMenu = true;
    }
    else if (m_state == StateMachine::Menu)
    {
        QApplication::quit();
    }
}

bool StateMachine::update()
{
    m_stateToFunctionMap[m_state]();

    return true;
}

void StateMachine::stateInit()
{
    m_state     = DoIntro;
    m_fadeValue = 0.0;
}

void StateMachine::stateDoIntro()
{
    assert(m_renderer);

    if (m_intro->update()) // Intro return true when done.
    {
        m_renderer->setFadeValue(1.0);
        m_state = Menu;
    }
}

void StateMachine::stateMenu()
{
    m_returnToMenu = false;
    m_fadeValue    = 1.0;
    m_isFading     = true; // Must be set to apply the initial fade value is set in Scene.
    m_renderer->setFadeValue(m_fadeValue);

    if (MTFH::MenuManager::instance().done())
    {
        m_state = MenuTransitionOut;
    }
}

void StateMachine::stateMenuTransitionIn()
{
    assert(m_renderer);

    if (m_fadeValue >= 1.0)
    {
        m_fadeValue = 1.0;
        m_isFading  = false;
        m_state     = Menu;

        // Re-init the track selection menu
        MTFH::MenuManager::instance().enterCurrentMenu();
    }
    else
    {
        m_fadeValue += FADE_SPEED_MENU;
        m_isFading   = true;
    }

    m_renderer->setFadeValue(m_fadeValue);
}

void StateMachine::stateMenuTransitionOut()
{
    if (m_fadeValue <= 0.0)
    {
        m_state     = GameTransitionIn;
        m_fadeValue = 0.0;
        m_isFading  = false;
    }
    else
    {
        m_fadeValue -= FADE_SPEED_MENU;
        m_isFading   = true;
    }

    m_renderer->setFadeValue(m_fadeValue);
}

void StateMachine::stateGameTransitionIn()
{
    assert(m_track);

    if (m_fadeValue >= 1.0)
    {
        m_fadeValue = 1.0;
        m_isFading  = false;
        m_state     = DoStartlights;

        m_startlights->reset();
    }
    else
    {
        m_fadeValue += FADE_SPEED_MENU;
        m_isFading   = true;
    }

    m_renderer->setFadeValue(m_fadeValue);
}

void StateMachine::stateGameTransitionOut()
{
    assert(m_renderer);

    if (m_fadeValue >= FADE_SPEED_GAME)
    {
        m_fadeValue -= FADE_SPEED_GAME;
        m_isFading   = true;
    }
    else
    {
        m_fadeValue = 0.0;
        m_isFading  = false;
        m_state     = MenuTransitionIn;
    }

    m_renderer->setFadeValue(std::fmin(m_fadeValue, 1.0));
}

void StateMachine::stateDoStartlights()
{
    assert(m_startlights);

    if (!m_startlights->update())
    {
        m_state = Play;
    }
}

void StateMachine::statePlay()
{
    assert(m_race);

    if (m_race->finished())
    {
        m_fadeValue = 3.0; // Add some delay with a value greater than 1.0
        m_state     = GameTransitionOut;
    }
    else if (m_returnToMenu)
    {
        m_fadeValue = 1.0;
        m_state     = GameTransitionOut;
    }
}

StateMachine::State StateMachine::state() const
{
    return m_state;
}

bool StateMachine::isFading() const
{
    return m_isFading;
}

void StateMachine::reset()
{
    m_state = Init;
}
