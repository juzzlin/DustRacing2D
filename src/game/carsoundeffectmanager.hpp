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

#ifndef CARSOUNDEFFECTMANAGER_HPP
#define CARSOUNDEFFECTMANAGER_HPP

#include <QObject>

class Car;

/*! Manages sound effects, like the engine sound. These are connected
 *  to the AudioThread via Qt signals. */
class CarSoundEffectManager : public QObject
{
    Q_OBJECT

public:

    //! Constructor.
    CarSoundEffectManager(Car & car);

    //! Destructor.
    virtual ~CarSoundEffectManager();

    void update();

private:

    Car & m_car;
};

#endif // CARSOUNDEFFECTMANAGER_HPP
