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

#ifndef STARTLIGHTS_HPP
#define STARTLIGHTS_HPP

#include <QObject>

#include "MiniCore/Core/MCTypes"
#include "MiniCore/Core/MCVectorAnimation"

#include "updateableif.hpp"

#include <functional>
#include <map>
#include <string>

class MessageOverlay;
class Race;

//! Startlight model that controls the position and animation
//! of the startlights.
class Startlights : public QObject, public UpdateableIf
{
    Q_OBJECT

public:

    //! Startlights animation sequence states.
    enum State
    {
        Init,
        Appear,
        FirstRow,
        SecondRow,
        ThirdRow,
        Go,
        Disappear,
        End
    };

    //! Constructor.
    Startlights(MessageOverlay & messageOverlay);

    //! \reimp
    virtual bool update();

    //! \reimp
    virtual void reset();

    State state() const;

    void setDimensions(MCUint width, MCUint height);

    const MCVector3dF & pos() const;

    MCFloat glowScale() const;

signals:

    void soundRequested(const std::string & handle);
    void raceStarted();

private:

    typedef std::map<State, std::function<bool()> > StateToFunctionMap;
    StateToFunctionMap m_stateToFunctionMap;

    bool stateInit();

    bool stateAppear();

    bool stateFirstRow();

    bool stateSecondRow();

    bool stateThirdRow();

    bool stateGo();

    bool stateDisappear();

    bool stateEnd();

    bool timeElapsed(MCUint limit);

    State             m_state;
    MCUint            m_counter;
    MCUint            m_stepsPerState;
    MCVector3dF       m_pos;
    MCUint            m_width;
    MCUint            m_height;
    MCFloat           m_glowScale;
    MCVectorAnimation m_animation;
    MessageOverlay  & m_messageOverlay;
};

#endif // STARTLIGHTS_HPP
