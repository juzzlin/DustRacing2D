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
#include <QString>
#include <memory>
#include <Location>
#include <MCVector3d>

class Car;

/*! Manages sound effects, like the engine sound. These are connected
 *  to the AudioThread via Qt signals. */
class CarSoundEffectManager : public QObject
{
    Q_OBJECT

public:

    //! Constructor.
    CarSoundEffectManager(Car & car, QString engineSoundHandle);

    //! Destructor.
    virtual ~CarSoundEffectManager();

    void update();

public slots:

    void startEngineSound();

    void stopEngineSound();

signals:

    void playRequested(QString handle, bool loop);

    void stopRequested(QString handle);

    void pitchChangeRequested(QString handle, float pitch);

    void volumeChangeRequested(QString handle, float pitch);

    void locationChanged(QString handle, float x, float y);

private:

    Car & m_car;
    QString m_engineSoundHandle;
    int m_gear;
    int m_prevSpeed;
    MCVector3dF m_prevLocation;
};

typedef std::shared_ptr<CarSoundEffectManager> CarSoundEffectManagerPtr;

#endif // CARSOUNDEFFECTMANAGER_HPP
