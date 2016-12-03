// This file is part of Dust Racing 2D.
// Copyright (C) 2014 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef TIRE_HPP
#define TIRE_HPP

#include <MCObject>

class Car;

class Tire : public MCObject
{
public:

    Tire(Car & car, MCFloat friction, MCFloat offTrackFriction);

    virtual void onStepTime(int step) override;

    void setIsOffTrack(bool flag);

private:

    bool m_isOffTrack;

    MCFloat m_friction;

    MCFloat m_offTrackFriction;

    Car & m_car;
};

#endif // TIRE_HPP
