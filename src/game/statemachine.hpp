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

#ifndef STATEMACHINE_HPP
#define STATEMACHINE_HPP

#include "updateableif.hpp"
#include "renderer.hpp"
#include <MCTypes>

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
        Intro,
        Menu,
        GameTransitionIn, // Active when a race starts
        DoStartlights,
        Play,
        GameTransitionOut // Active when a race is over
    };

    //! Constructor.
    StateMachine();

    void setTrack(Track & track)
    {
        m_track = &track;
    }

    void setRenderer(Renderer & renderer)
    {
        m_renderer = &renderer;
        m_renderer->setEnabled(false);
    }

    void setRace(Race & race)
    {
        m_race = &race;
    }

    void setStartlights(Startlights & startlights)
    {
        m_startlights = &startlights;
    }

    StateMachine::State state() const;

    //! \reimp
    virtual bool update();

    //! \reimp
    virtual void reset();

private:

    State         m_state;
    Startlights * m_startlights;
    Race        * m_race;
    Renderer    * m_renderer;
    Track       * m_track;
    MCFloat       m_fadeValue;
};

#endif // STATEMACHINE_HPP
