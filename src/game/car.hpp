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

#include <MCObject>
#include <MCVector2d>
#include <MCParticle>
#include <deque>
#include <memory>

class MCSurface;
class MCFrictionGenerator;
class Route;

//! Base class for race cars.
class Car : public MCObject
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
        , brakingFriction(0.75)
        , rollingFrictionOnTrack(0.05)
        , rollingFrictionOffTrack(0.5)
        , rotationFriction(1.0)
        , slideFriction(0.5)
        , turningImpulse(0.30)
        , power(5000.0)
        , mass(1000.0)
        , momentOfInertia(mass * 10.0)
        , restitution(0.1)
        , dragLinear(0.5)
        , dragQuadratic(2.5)
        , leftFrontTirePos(15, 11)
        , rightFrontTirePos(15, -11)
        , leftRearTirePos(-15, 11)
        , rightRearTirePos(-15, -11)
        , leftBrakeGlowPos(-27, 11)
        , rightBrakeGlowPos(-27, -11)
        {}

        MCFloat maxLinearVelocity;
        MCFloat maxAngularVelocity;
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

        MCVector2dF leftFrontTirePos;
        MCVector2dF rightFrontTirePos;
        MCVector2dF leftRearTirePos;
        MCVector2dF rightRearTirePos;
        MCVector2dF leftBrakeGlowPos;
        MCVector2dF rightBrakeGlowPos;
    };

    //! Constructor.
    Car(Description desc, MCSurface & surface, MCUint index, bool isHuman);

    //! Return the index.
    MCUint index() const;

    //! Clear statuses before setting any states.
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

    bool isHuman() const;

private:

    void doSmoke(MCVector3dFR location, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const;

    void doSkidMark(MCVector3dFR location, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const;

    void doSparkle(MCVector3dFR location, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const;

    void doMud(MCVector3dFR location, MCFloat r, MCFloat g, MCFloat b, MCFloat a) const;

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
    MCSurface           & m_frontTire;
    MCSurface           & m_brakeGlow;
    MCFloat               m_length;
    int                   m_speedInKmh;
    MCFloat               m_dx, m_dy;
    int                   m_currentTargetNodeIndex;
    int                   m_prevTargetNodeIndex;
    int                   m_routeProgression;
    bool                  m_isHuman;

    mutable MCParticle::ParticleFreeList m_freeList;
    std::vector<std::shared_ptr<MCParticle> > m_delete;
};

#endif // CAR_HPP
