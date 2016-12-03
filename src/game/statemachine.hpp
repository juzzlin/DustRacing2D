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

#ifndef STATEMACHINE_HPP
#define STATEMACHINE_HPP

#include "renderer.hpp"
#include "updateableif.hpp"

#include <MCTypes>
#include <QObject>

#include <functional>
#include <map>

class InputHandler;

//! The main state machine of the game.
class StateMachine : public QObject, public UpdateableIf
{
    Q_OBJECT

public:

    enum class State
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
    StateMachine(InputHandler & inputHandler);

    //! Destructor.
    virtual ~StateMachine();

    //! Return the singleton instance.
    static StateMachine & instance();

    void quit();

    StateMachine::State state() const;

    //! \reimp
    virtual bool update() override;

    //! \reimp
    virtual void reset() override;

public slots:

    void endFadeIn();

    void endFadeOut();

    void endStartlightAnimation();

    void finishRace();

signals:

    void fadeInRequested(int, int, int);

    void fadeOutRequested(int, int, int);

    void fadeOutFlashRequested(int, int, int);

    void soundsStopped();

    void startlightAnimationRequested();

    void renderingEnabled(bool);

    void exitGameRequested();

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
    State              m_oldState;
    bool               m_raceFinished;
    InputHandler     & m_inputHandler;
};

#endif // STATEMACHINE_HPP
