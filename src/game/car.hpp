// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef CAR_HPP
#define CAR_HPP

#include <MCObject>
#include <MCVector2d>

#include "updateableif.hpp"

class MCSurface;
class MCFrictionGenerator;
class Route;

//! Base class for race cars.
class Car : public MCObject, public UpdateableIf
{
public:

    //! Defines the (default) car properties.
    class Description
    {
    public:

        //! Constructor.
        Description()
        : maxLinearVelocity(50.0)
        , maxAngularVelocity(10.0)
        , accelerationFriction(1.0)
        , brakingFriction(1.5)
        , rollingFrictionOnTrack(0.1)
        , rollingFrictionOffTrack(0.5)
        , rotationFriction(1.0)
        , slideFriction(0.75)
        , turningImpulse(0.3)
        , power(5000.0)
        , mass(1000.0)
        , momentOfInertia(mass * 10.0)
        , restitution(0.1)
        , dragLinear(1.0)
        , dragQuadratic(5.0)
        , numberPos(-7, 0)
        , leftFrontTirePos(15, 11)
        , rightFrontTirePos(15, -11)
        , leftRearTirePos(-15, 11)
        , rightRearTirePos(-15, -11)
        , leftBrakeGlowPos(-27, 11)
        , rightBrakeGlowPos(-27, -11)
        {}

        MCFloat maxLinearVelocity;
        MCFloat maxAngularVelocity;
        MCFloat accelerationFriction;
        MCFloat brakingFriction;
        MCFloat rollingFrictionOnTrack;
        MCFloat rollingFrictionOffTrack;
        MCFloat rotationFriction;
        MCFloat slideFriction;
        MCFloat turningImpulse;
        MCFloat power;
        MCFloat mass;
        MCFloat momentOfInertia;
        MCFloat restitution;
        MCFloat dragLinear;
        MCFloat dragQuadratic;

        MCVector2dF numberPos;
        MCVector2dF leftFrontTirePos;
        MCVector2dF rightFrontTirePos;
        MCVector2dF leftRearTirePos;
        MCVector2dF rightRearTirePos;
        MCVector2dF leftBrakeGlowPos;
        MCVector2dF rightBrakeGlowPos;
    };

    //! Constructor.
    Car(Description & desc, MCSurface & surface, MCUint index, bool isHuman);

    //! Destructor.
    virtual ~Car();

    //! Return the index.
    MCUint index() const;

    //! Clear statuses before setting any states.
    void clearStatuses();

    //! Turn left.
    void turnLeft();

    //! Turn right.
    void turnRight();

    //! Accelerate.
    void accelerate(bool deccelerate = false);

    //! Brake.
    void brake();

    void setBrakeLightState(bool state);

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

    //! \reimp
    virtual bool update();

    //! \reimp
    virtual void reset();

    void setLeftSideOffTrack(bool state);

    void setRightSideOffTrack(bool state);

    void setTurningImpulse(MCFloat impulse);

    void setCurrentTargetNodeIndex(int index);

    int currentTargetNodeIndex() const;

    void setPrevTargetNodeIndex(int index);

    int prevTargetNodeIndex() const;

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

    MCVector3dF numberLocation() const;

    bool isHuman() const;

private:

    MCSurface & generateNumberSurface(MCUint index);

    void setProperties(Description & desc);
    void initForceGenerators(Description & desc);

    Description           m_desc;

    MCFrictionGenerator * m_pBrakingFriction;
    MCFrictionGenerator * m_pOnTrackFriction;
    MCFrictionGenerator * m_pOffTrackFriction;
    MCFrictionGenerator * m_pSlideFriction;
    bool                  m_leftSideOffTrack;
    bool                  m_rightSideOffTrack;
    bool                  m_accelerating;
    bool                  m_braking;
    bool                  m_reverse;
    bool                  m_turnLeft;
    bool                  m_turnRight;
    MCUint                m_index;
    MCFloat               m_tireAngle;
    MCSurface           & m_number;
    MCSurface           & m_frontTire;
    MCSurface           & m_brakeGlow;
    MCFloat               m_length;
    int                   m_speedInKmh;
    MCFloat               m_dx, m_dy;
    int                   m_currentTargetNodeIndex;
    int                   m_prevTargetNodeIndex;
    int                   m_routeProgression;
    bool                  m_isHuman;
    mutable int           m_smokeCounter;
    mutable int           m_sparkleCounter;
    mutable int           m_mudCounter;
};

#endif // CAR_HPP
