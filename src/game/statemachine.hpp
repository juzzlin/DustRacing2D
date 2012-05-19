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
        GameTransitionIn,
        DoStartlights,
        Play
    };

    //! Constructor.
    StateMachine(Renderer & renderer, Startlights & startLights);

    void setTrack(Track & track);

    StateMachine::State state() const;

    //! \reimp
    virtual bool update();

    //! \reimp
    virtual void reset();

private:

    State         m_state;
    Startlights & m_startlights;
    Renderer    & m_renderer;
    Track       * m_pTrack;
    float         m_fadeValue;
};

#endif // STATEMACHINE_HPP
