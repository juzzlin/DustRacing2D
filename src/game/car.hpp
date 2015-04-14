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

#include <MCForceGenerator>
#include <MCObject>
#include <MCVector2d>

#include "updateableif.hpp"
#include "carparticleeffectmanager.hpp"
#include "carsoundeffectmanager.hpp"

#include <memory>

class MCSurface;
class MCFrictionGenerator;
class Route;

//! Base class for race cars.
class Car : public MCObject, public UpdateableIf
{
public:

    enum class Steer {
        Neutral,
        Left,
        Right
    };

    //! Defines the (default) car properties.
    class Description
    {
    public:

        //! Constructor.
        Description()
        : accelerationFriction(0.75f)
        , brakingFriction(1.5f)
        , rollingFrictionOnTrack(0.1f)
        , rotationFriction(1.0f)
        , power(5000.0f)
        , mass(1500.0f)
        , restitution(0.05f)
        , dragLinear(1.0f)
        , dragQuadratic(5.0f)
        , tireWearOutCapacity(100)
        , numberPos(-5, 0, 0)
        , leftFrontTirePos(14, 9, 0)
        , rightFrontTirePos(14, -9, 0)
        , leftRearTirePos(-14, 9, 0)
        , rightRearTirePos(-14, -9, 0)
        , leftBrakeGlowPos(-21, 8, 0)
        , rightBrakeGlowPos(-21, -8, 0)
        {}

        float accelerationFriction;
        float brakingFriction;
        float rollingFrictionOnTrack;
        float rotationFriction;
        float power;
        float mass;
        float restitution;
        float dragLinear;
        float dragQuadratic;
        float tireWearOutCapacity;

        MCVector3dF numberPos;
        MCVector3dF leftFrontTirePos;
        MCVector3dF rightFrontTirePos;
        MCVector3dF leftRearTirePos;
        MCVector3dF rightRearTirePos;
        MCVector3dF leftBrakeGlowPos;
        MCVector3dF rightBrakeGlowPos;
    };

    //! Constructor.
    Car(Description & desc, MCSurface & surface, MCUint index, bool isHuman);

    //! Destructor.
    virtual ~Car();

    //! Return the index.
    MCUint index() const;

    //! Clear statuses before setting any states.
    void clearStatuses();

    //! Steer.
    void steer(Steer direction, MCFloat control = 1.0f);

    //! Accelerate.
    void accelerate(bool deccelerate = false);

    //! Brake.
    void brake();

    bool isBraking() const;

    bool isSkidding() const;

    void setBrakeLightState(bool state);

    //! Get estimated speed in km/h. This is used e.g. by the speedometer.
    int speedInKmh() const;

    //! Get cached speed.
    MCFloat absSpeed() const;

    //! \reimp
    virtual void render(MCCamera *p);

    //! \reimp
    virtual void collisionEvent(MCCollisionEvent & event);

    //! \reimp
    virtual void stepTime(MCFloat step);

    //! \reimp
    virtual bool update();

    //! \reimp
    virtual void reset();

    void setLeftSideOffTrack(bool state);

    bool leftSideOffTrack() const;

    void setRightSideOffTrack(bool state);

    bool rightSideOffTrack() const;

    bool isOffTrack() const;

    void setTurningImpulse(float impulse);

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

    //! \return Tire wear level. 1.0 means no wear, 0.0 means totally worn-out tires.
    float tireWearLevel() const;

    void resetTireWear();

    void setSoundEffectManager(CarSoundEffectManagerPtr soundEffectManager);

    CarSoundEffectManagerPtr soundEffectManager() const;

private:

    void initForceGenerators(Description & desc);

    void setProperties(Description & desc);

    void wearOutTires(MCFloat step, MCFloat factor);

    Description              m_desc;
    MCForceGeneratorPtr      m_pBrakingFriction;
    MCForceGeneratorPtr      m_pOnTrackFriction;
    bool                     m_leftSideOffTrack;
    bool                     m_rightSideOffTrack;
    bool                     m_accelerating;
    bool                     m_braking;
    bool                     m_reverse;
    bool                     m_skidding;
    Steer                    m_steer;
    MCUint                   m_index;
    float                    m_tireAngle;
    float                    m_tireWearOutCapacity;
    MCSurface              & m_frontTire;
    MCSurface              & m_brakeGlow;
    MCFloat                  m_length;
    int                      m_speedInKmh;
    MCFloat                  m_absSpeed;
    MCFloat                  m_dx, m_dy;
    int                      m_currentTargetNodeIndex;
    int                      m_prevTargetNodeIndex;
    int                      m_routeProgression;
    bool                     m_isHuman;
    CarParticleEffectManager m_particleEffectManager;
    CarSoundEffectManagerPtr m_soundEffectManager;
    MCObjectPtr              m_leftFrontTire;
    MCObjectPtr              m_rightFrontTire;
    MCObjectPtr              m_leftRearTire;
    MCObjectPtr              m_rightRearTire;
};

typedef std::shared_ptr<Car> CarPtr;

#endif // CAR_HPP
