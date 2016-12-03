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

#ifndef PIT_HPP
#define PIT_HPP

#include <MCObject>
#include <QObject>

#include <map>

class MCCollisionEvent;
class MCSurface;
class Car;

//! The pit object.
class Pit : public QObject, public MCObject
{
    Q_OBJECT

public:

    //! Constructor.
    Pit(MCSurface & surface);

    //! \reimp
    virtual void collisionEvent(MCCollisionEvent & event) override;

    //! \reimp
    virtual void onStepTime(int step) override;

signals:

    void pitStop(Car & car);

private:

    std::map<Car *, int> m_pittingCars;
    int                  m_tag;
};

#endif // PIT_HPP
