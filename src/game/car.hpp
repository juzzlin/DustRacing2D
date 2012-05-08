// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef CAR_HPP
#define CAR_HPP

#include "MiniCore/Core/MCObject"
#include "MiniCore/Core/MCGLRectParticleGroup"

#include <deque>

class MCSurface;
class MCFrictionGenerator;

//! Base class for race cars.
class Car : public MCObject
{
public:

    //! Constructor.
    Car(MCSurface & surface, MCUint index);

    //! Return the index.
    MCUint index() const;

    void clearStatuses();

    //! Turn left.
    void turnLeft();

    //! Turn right.
    void turnRight();

    //! Accelerate.
    void accelerate();

    //! Brake.
    void brake();

    void setBrakeLightState(bool state);

    //! User isn't doing anything.
    void noAction();

    //! User isn't steering.
    void noSteering();

    //! Get estimated speed in km/h. This is used by the speedometer.
    int speedInKmh() const;

    //! \reimp
    virtual void render(MCCamera *p);

    //! \reimp
    virtual void collisionEvent(MCCollisionEvent & event);

    //! \reimp
    virtual void stepTime();

    void setPower(MCFloat power);

    void setLeftSideOffTrack(bool state);

    void setRightSideOffTrack(bool state);

    void setTurningImpulse(MCFloat impulse);

    void setCurrentRouteIndex(int index);

    int currentRouteIndex() const;

    void setRouteProgression(int value);

    int routeProgression() const;

    //! Get location of the left front tire.
    MCVector3dF leftFrontTireLocation() const;

    //! Get location of the right front tire.
    MCVector3dF rightFrontTireLocation() const;

    //! Get location of the left rear tire.
    MCVector3dF leftRearTireLocation() const;

    //! Get location of the right rear tire.
    MCVector3dF rightRearTireLocation() const;

private:

    void doSmoke(MCVector3dFR location, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const;

    void doSkidMark(MCVector3dFR location, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const;

    void doSparkle(MCVector3dFR location, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const;

    void doMud(MCVector3dFR location, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const;

    MCFrictionGenerator * m_pBrakingFriction;
    MCFrictionGenerator * m_pOffTrackFriction;
    bool                  m_leftSideOffTrack;
    bool                  m_rightSideOffTrack;
    bool                  m_accelerating;
    bool                  m_braking;
    bool                  m_reverse;
    bool                  m_turnLeft;
    bool                  m_turnRight;
    MCUint                m_index;
    MCFloat               m_tireAngle;
    MCSurface           & m_frontTire;
    MCSurface           & m_brakeGlow;
    MCFloat               m_power;
    MCFloat               m_turningImpulse;
    MCFloat               m_length;
    int                   m_speedInKmh;
    MCFloat               m_dx, m_dy;
    int                   m_currentRouteIndex;
    int                   m_routeProgression;

    mutable MCGLRectParticleGroup m_particleGroup;
};

#endif // CAR_HPP
