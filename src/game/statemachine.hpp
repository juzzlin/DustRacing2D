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

#ifndef STATEMACHINE_HPP
#define STATEMACHINE_HPP

#include "renderer.hpp"
#include "updateableif.hpp"

#include <MCTypes>
#include <QApplication>

#include <functional>
#include <map>

class Intro;
class Race;
class Renderer;
class Startlights;
class Track;

//! The main state machine of the game.
class StateMachine : public UpdateableIf
{
public:

    enum State
    {
        Init,
        DoIntro,
        Menu,
        MenuTransitionIn,
        MenuTransitionOut,
        GameTransitionIn, // Active when a race starts
        GameTransitionOut, // Active when a race is over
        DoStartlights,
        Play
    };

    //! Constructor.
    StateMachine();

    //! Destructor.
    virtual ~StateMachine();

    //! Return the singleton instance.
    static StateMachine & instance();

    void setIntro(Intro & intro);

    void setTrack(Track & track);

    void setRenderer(Renderer & renderer);

    void setRace(Race & race);

    void setStartlights(Startlights & startlights);

    void setThemeSong(SFX::Music & themeSong);

    void quit();

    StateMachine::State state() const;

    bool isFading() const;

    //! \reimp
    virtual bool update();

    //! \reimp
    virtual void reset();

private:

    void stateInit();

    void stateDoIntro();

    void stateMenu();

    void stateMenuTransitionIn();

    void stateMenuTransitionOut();

    void stateGameTransitionIn();

    void stateGameTransitionOut();

    void stateDoStartlights();

    void statePlay();

    static StateMachine * m_instance;

    typedef std::map<State, std::function<void()> > StateToFunctionMap;

    StateToFunctionMap m_stateToFunctionMap;
    State              m_state;
    bool               m_isFading;
    Intro            * m_intro;
    Startlights      * m_startlights;
    Race             * m_race;
    Renderer         * m_renderer;
    Track            * m_track;
    MCFloat            m_fadeValue;
    bool               m_returnToMenu;
};

#endif // STATEMACHINE_HPP
