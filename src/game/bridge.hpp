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

#ifndef BRIDGE_HPP
#define BRIDGE_HPP

#include <MCObject>
#include <QObject>

#include <map>

class MCCollisionEvent;
class MCSurface;
class Car;

class Bridge : public QObject, public MCObject
{
public:

    Bridge(MCSurface & surface, MCSurface & rail);

    //! \reimp
    virtual void collisionEvent(MCCollisionEvent & event);

    //! \reimp
    virtual void stepTime(MCFloat step);

signals:

    void onBridge(Car & car);

    std::map<Car *, int> m_carsOnBridge;
    int                  m_tag;

    MCObjectPtr          m_rail0;
    MCObjectPtr          m_rail1;
    MCObjectPtr          m_entry0;
};

#endif // BRIDGE_HPP
