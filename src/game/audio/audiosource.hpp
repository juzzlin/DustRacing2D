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

#ifndef AUDIOSOURCE_HPP
#define AUDIOSOURCE_HPP

#include <QObject>

/*! Class that defines a set of signals used to play sounds.
 *  AudioWorker can connect to a class inherited from AudioSource. */
class AudioSource : public QObject
{
    Q_OBJECT

public:
    //! Constructor.
    AudioSource();

    //! Destructor.
    virtual ~AudioSource();

signals:

    void playRequested(const QString & handle, bool loop);

    void stopRequested(const QString & handle);

    void pitchChanged(const QString & handle, float pitch);

    void volumeChanged(const QString & handle, float pitch);

    void locationChanged(const QString & handle, float x, float y);
};

#endif // AUDIOSOURCE_HPP
