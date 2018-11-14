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

#include "carparticleeffectmanager.hpp"
#include "carsoundeffectmanager.hpp"

#include <memory>

class MCSurface;
class MCFrictionGenerator;
class Route;

//! Base class for race cars.
class Car : public MCObject
{
public:

    enum class Steer {
        Neutral,
        Left,
        Right
    };

    //! Defines the (default) car properties.
    struct Description
    {
		Description();
		
        float accelerationFriction; // friction coefficient for tires on street, about 1.1 for best of class tires

        float rollingFrictionOnTrack; // braking friction coefficient for rolling on street, typically below 0.1

        float rotationFriction; 

        float power; // engine's power in Watts

        float mass; // cars mass in kilo gramms

        float restitution; // factor for remaining impulse for rebounce

        float dragLinear; // driving resistance coefficient in N/(m/s)

        float dragQuadratic; // driving resistance proportional to the squared speed in N/(m/s)²
    };

    //! Constructor.
    Car(Description & desc, MCSurface & surface, unsigned int index, bool isHuman);

    //! Destructor.
    virtual ~Car();

    //! Return the index.
    unsigned int index() const;

    //! Clear statuses before setting any states.
    void clearStatuses();

    //! Steer.
    void steer(Steer direction, float control = 1.0f);

    //! Accelerate.
    void accelerate(bool deccelerate = false);

    //! Brake.
    void brake();

    bool isBraking() const;

    bool isSkidding() const;

    bool isSliding();

    void setBrakeLightState(bool state);

    //! Get estimated speed in km/h. This is used e.g. by the speedometer.
    int speedInKmh() const;

    //! Get cached speed.
    float absSpeed() const;

    void addDamage(float damage);

    //! \reimp
    virtual void collisionEvent(MCCollisionEvent & event) override;

    //! \reimp
    virtual void onStepTime(int ms) override;

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

    void setPosition(int value);

    int position() const;

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

    void updateAnimations();

    void updateTireWear(int step);

    void wearOutTires(int step, float factor);

    Description m_desc;

    MCForceGeneratorPtr m_onTrackFriction;

    bool m_leftSideOffTrack;

    bool m_rightSideOffTrack;

    bool m_accelerating;

    bool m_braking;

    bool m_reverse;

    bool m_skidding;

    Steer m_steer;

    unsigned int m_index;

    float m_tireAngle; // -15...+15 degrees

    float m_initDamageCapacity; /// normally 100, in percent

    float m_damageCapacity; /// 0 to m_initDamageCapacity

    float m_initTireWearOutCapacity; /// normally 100, in percent

    float m_tireWearOutCapacity; /// 0 to m_initTireWearOutCapacity

    MCSurface & m_frontTire;

    MCSurface & m_brakeGlow;

    float m_length; // in meters

    int m_speedInKmh;

    float m_absSpeed; // in meters/second

    float m_dx, m_dy;

    int m_nextTargetNodeIndex;

    int m_currentTargetNodeIndex;

    int m_prevTargetNodeIndex;

    int m_routeProgression;

    int m_position;

    bool m_isHuman;

    CarParticleEffectManager m_particleEffectManager;

    CarSoundEffectManagerPtr m_soundEffectManager;

    MCObjectPtr m_leftFrontTire;

    MCObjectPtr m_rightFrontTire;

    MCObjectPtr m_leftRearTire;

    MCObjectPtr m_rightRearTire;

    MCObjectPtr m_leftBrakeGlow;

    MCObjectPtr m_rightBrakeGlow;

    MCVector3dF m_numberPos; // scene units

    MCVector3dF m_leftFrontTirePos; // scene units

    MCVector3dF m_rightFrontTirePos; // scene units

    MCVector3dF m_leftRearTirePos; // scene units

    MCVector3dF m_rightRearTirePos; // scene units

    MCVector3dF m_leftBrakeGlowPos; // scene units

    MCVector3dF m_rightBrakeGlowPos; // scene units

    bool m_hadHardCrash;
};

typedef std::shared_ptr<Car> CarPtr;

#endif // CAR_HPP
