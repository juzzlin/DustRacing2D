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
        , rollingFrictionOnTrack(0.1f)
        , rotationFriction(1.0f)
        , power(5000.0f)
        , mass(1500.0f)
        , restitution(0.05f)
        , dragLinear(1.0f)
        , dragQuadratic(5.0f)
        {}

        float accelerationFriction;
        float rollingFrictionOnTrack;
        float rotationFriction;
        float power;
        float mass;
        float restitution;
        float dragLinear;
        float dragQuadratic;
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

    void addDamage(float damage);

    //! \reimp
    virtual void render(MCCamera *p) override;

    //! \reimp
    virtual void collisionEvent(MCCollisionEvent & event) override;

    //! \reimp
    virtual void onStepTime(MCFloat step) override;

    //! \reimp
    virtual bool update() override;

    //! \reimp
    virtual void reset() override;

    void setLeftSideOffTrack(bool state);

    bool leftSideOffTrack() const;

    void setRightSideOffTrack(bool state);

    bool rightSideOffTrack() const;

    bool isOffTrack() const;

    void setTurningImpulse(float impulse);
    
    void setNextTargetNodeIndex(int index);

    int nextTargetNodeIndex() const;

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

    float tireWearFactor() const;

    float tireWearLevel() const;

    void resetTireWear();

    float damageFactor() const;

    float damageLevel() const;

    void resetDamage();

    bool hadHardCrash();

    void setSoundEffectManager(CarSoundEffectManagerPtr soundEffectManager);

    CarSoundEffectManagerPtr soundEffectManager() const;

private:

    void initForceGenerators(Description & desc);

    void setProperties(Description & desc);

    void wearOutTires(MCFloat step, MCFloat factor);

    Description              m_desc;
    MCForceGeneratorPtr      m_onTrackFriction;
    bool                     m_leftSideOffTrack;
    bool                     m_rightSideOffTrack;
    bool                     m_accelerating;
    bool                     m_braking;
    bool                     m_reverse;
    bool                     m_skidding;
    Steer                    m_steer;
    MCUint                   m_index;
    float                    m_tireAngle;
    float                    m_initDamageCapacity;
    float                    m_damageCapacity;
    float                    m_initTireWearOutCapacity;
    float                    m_tireWearOutCapacity;
    MCSurface              & m_frontTire;
    MCSurface              & m_brakeGlow;
    MCFloat                  m_length;
    int                      m_speedInKmh;
    MCFloat                  m_absSpeed;
    MCFloat                  m_dx, m_dy;
    int                      m_nextTargetNodeIndex;
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
    MCVector3dF              m_numberPos;
    MCVector3dF              m_leftFrontTirePos;
    MCVector3dF              m_rightFrontTirePos;
    MCVector3dF              m_leftRearTirePos;
    MCVector3dF              m_rightRearTirePos;
    MCVector3dF              m_leftBrakeGlowPos;
    MCVector3dF              m_rightBrakeGlowPos;
    bool                     m_hadHardCrash;
};

typedef std::shared_ptr<Car> CarPtr;

#endif // CAR_HPP
