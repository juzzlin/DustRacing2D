// This file is part of Dust Racing 2D.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef CARSTATUSVIEW_HPP
#define CARSTATUSVIEW_HPP

#include "renderable.hpp"

class MCSurface;
class Car;

//! Shows the tire and fuel status of a car.
class CarStatusView : public Renderable
{
public:

    //! Constructor.
    CarStatusView();

    //! \reimp
    virtual void render() override;

    void setCarToFollow(const Car & car);

private:

    MCSurface & m_body;
    MCSurface & m_tires;
    const Car * m_car;
};

#endif // CARSTATUSVIEW_HPP
