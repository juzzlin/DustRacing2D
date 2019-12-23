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

#include <QString>
#include <QTimer>

#include "audiosource.hpp"
#include <Location>
#include <MCVector3d>
#include <memory>

class Car;
class MCObject;

/*! Manages sound effects, like the engine sound. These are connected
 *  to the AudioThread via Qt signals. */
class CarSoundEffectManager : public AudioSource
{
    Q_OBJECT

public:
    /*! Handles to special sounds that can be played simultaneously by each car */
    struct MultiSoundHandles
    {
        QString engineSoundHandle;
        QString hitSoundHandle;
        QString skidSoundHandle;
    };

    //! Constructor.
    CarSoundEffectManager(Car & car, const MultiSoundHandles & handles);

    //! Destructor.
    virtual ~CarSoundEffectManager();

    void update();

    void collision(MCObject & collidingObject);

public slots:

    void startEngineSound();

    void stopEngineSound();

private:
    void processEngineSound();

    void processSkidSound();

    Car & m_car;

    int m_gear;

    int m_prevSpeed;

    MCVector3dF m_prevLocation;

    QTimer m_hitTimer;

    QTimer m_skidTimer;

    MultiSoundHandles m_handles;

    bool m_skidPlaying;
};

typedef std::shared_ptr<CarSoundEffectManager> CarSoundEffectManagerPtr;

#endif // CARSOUNDEFFECTMANAGER_HPP
