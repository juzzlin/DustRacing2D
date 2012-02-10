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

class MCSurface;
class MCFrictionGenerator;

//! Base class for race cars.
class Car : public MCObject
{
public:

    //! Constructor.
    Car(MCSurface & surface, MCUint index);

    //! Destructor.
    virtual ~Car();

    //! Return the index.
    MCUint index() const;

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
    MCUint speedInKmh() const;

    //! \reimp
    virtual void render(MCCamera *p);

private:

    void oversteer(MCUint newBodyAngle);

    MCFrictionGenerator * m_pDeccelerationFriction;
    bool                  m_frictionGeneratorAdded;
    bool                  m_accelerating;
    bool                  m_braking;
    MCUint                m_index;
    MCUint                m_leftTireAngle;
    MCUint                m_rightTireAngle;
    MCSurface           & m_frontTire;
    MCSurface           & m_brakeGlow;
};

#endif // CAR_HPP
