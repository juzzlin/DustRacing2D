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

#include <MCCollisionEvent>
#include <MCForceGenerator>
#include <MCObject>
#include <MCVector2d>

#include <deque>
#include <memory>

class CarParticleEffectManager;
class CarSoundEffectManager;
class Gearbox;
class MCSurface;
class MCFrictionGenerator;
class Route;

//! Base class for race cars.
class Car : public MCObject
{
public:
    enum class Steer
    {
        Neutral,
        Left,
        Right
    };

    //! Defines the (default) car properties.
    struct Description
    {
        float accelerationFriction = 0.75f;

        float rollingFrictionOnTrack = 0.1f;

        float rotationFriction = 1.0f;

        float power = 5000.0f;

        float mass = 1500.0f;

        float restitution = 0.05f;

        float dragLinear = 1.0f;

        float dragQuadratic = 5.0f;
    };

    //! Constructor.
    Car(Description & desc, std::shared_ptr<MCSurface> surface, size_t index, bool isHuman);

    //! Destructor.
    virtual ~Car() override;

    //! Return the index.
    size_t index() const;

    void steer(Steer direction, float control = 1.0f);

    bool isAccelerating() const;

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

    using CarSoundEffectManagerPtr = std::shared_ptr<CarSoundEffectManager>;
    void setSoundEffectManager(CarSoundEffectManagerPtr soundEffectManager);

    CarSoundEffectManagerPtr soundEffectManager() const;

    bool hasDamage() const;

    bool hasTireWear() const;

    void setAcceleratorEnabled(bool acceleratorEnabled);

    void setBrakeEnabled(bool brakeEnabled);

private:
    void accelerate(bool deccelerate = false);

    void createChildObjects(float maxZ, size_t index);

    void createTires();

    void doTireSpinEffect();

    void initForceGenerators(Description & desc);

    void setProperties(Description & desc);

    void updateAnimations();

    void updateTireWear(int step);

    void wearOutTires(int step, float factor);

    Description m_desc;

    MCForceGeneratorPtr m_onTrackFriction;

    bool m_leftSideOffTrack;

    bool m_rightSideOffTrack;

    bool m_skidding;

    Steer m_steer;

    size_t m_index;

    float m_tireAngle;

    float m_initDamageCapacity;

    float m_damageCapacity;

    float m_initTireWearOutCapacity;

    float m_tireWearOutCapacity;

    std::shared_ptr<MCSurface> m_frontTire;

    std::shared_ptr<MCSurface> m_brakeGlow;

    float m_length;

    int m_speedInKmh;

    float m_absSpeed;

    float m_dx, m_dy;

    bool m_isHuman;

    std::unique_ptr<CarParticleEffectManager> m_particleEffectManager;

    CarSoundEffectManagerPtr m_soundEffectManager;

    MCObjectPtr m_leftFrontTire;

    MCObjectPtr m_rightFrontTire;

    MCObjectPtr m_leftRearTire;

    MCObjectPtr m_rightRearTire;

    MCObjectPtr m_leftBrakeGlow;

    MCObjectPtr m_rightBrakeGlow;

    MCVector3dF m_numberPos;

    MCVector3dF m_leftFrontTirePos;

    MCVector3dF m_rightFrontTirePos;

    MCVector3dF m_leftRearTirePos;

    MCVector3dF m_rightRearTirePos;

    MCVector3dF m_leftBrakeGlowPos;

    MCVector3dF m_rightBrakeGlowPos;

    bool m_hadHardCrash;

    std::unique_ptr<Gearbox> m_gearbox;

    bool m_acceleratorEnabled = false;

    bool m_brakeEnabled = false;

    std::deque<std::pair<MCObject *, MCVector3dF>> m_collisionEffectStack;
};

typedef std::shared_ptr<Car> CarPtr;

#endif // CAR_HPP
