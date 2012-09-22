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

#ifndef STARTLIGHTS_HPP
#define STARTLIGHTS_HPP

#include <QObject>

#include "MiniCore/Core/MCTypes"
#include "MiniCore/Core/MCVectorAnimation"

#include "updateableif.hpp"

class MessageOverlay;
class Race;

//! Startlight model that controls the position and animation
//! of the startlights.
class Startlights : public QObject, public UpdateableIf
{
    Q_OBJECT

public:

    //! Startlights animation sequence states.
    enum LightState
    {
        LightsInit,
        LightsAppear,
        LightsFirstRow,
        LightsSecondRow,
        LightsThirdRow,
        LightsGo,
        LightsDisappear,
        LightsEnd
    };

    //! Constructor.
    Startlights(MessageOverlay & messageOverlay);

    //! \reimp
    virtual bool update();

    //! \reimp
    virtual void reset();

    LightState state() const;

    void setDimensions(MCUint width, MCUint height);

    const MCVector3dF & pos() const;

signals:

    void raceStarted();

private:

    bool updateCounter(MCUint limit);

    LightState        m_state;
    MCUint            m_counter;
    MCUint            m_stepsPerState;
    MCVector3dF       m_pos;
    MCUint            m_width;
    MCUint            m_height;
    MCVectorAnimation m_animation;
    MessageOverlay  & m_messageOverlay;
};

#endif // STARTLIGHTS_HPP
