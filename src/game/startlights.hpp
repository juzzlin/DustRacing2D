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
#include <QString>
#include <QTimer>

#include <MCVectorAnimation>

#include <functional>
#include <map>
#include <string>

class Race;

//! Startlight model that controls the position and animation
//! of the startlights.
class Startlights : public QObject
{
    Q_OBJECT

public:
    //! Startlights animation sequence states.
    enum class State
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
    Startlights();

    State state() const;

    void setDimensions(size_t width, size_t height);

    const MCVector3dF & pos() const;

    float glowScale() const;

signals:

    void messageRequested(QString message);

    void raceStarted();

    void animationEnded();

public slots:

    void beginAnimation();

private slots:

    void updateAnimation();

private:
    typedef std::map<State, std::function<void()>> StateToFunctionMap;
    StateToFunctionMap m_stateToFunctionMap;

    void stateInit();

    void stateAppear();

    void stateFirstRow();

    void stateSecondRow();

    void stateThirdRow();

    void stateGo();

    void stateDisappear();

    void stateEnd();

    bool timeElapsed(size_t limit);

    State m_state;

    size_t m_counter;

    size_t m_stepsPerState;

    MCVector3dF m_pos;

    size_t m_width;

    size_t m_height;

    float m_glowScale;

    MCVectorAnimation m_animation;

    QTimer m_timer;
};

#endif // STARTLIGHTS_HPP
