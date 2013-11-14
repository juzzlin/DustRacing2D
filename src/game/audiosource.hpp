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

#include "audiothread.hpp"

//! Class that defines a set of signals used to play sounds.
class AudioSource : public QObject
{
    Q_OBJECT

public:

    //! Constructor.
    AudioSource();

    //! Destructor.
    virtual ~AudioSource();

signals:

    void playRequested(QString handle, bool loop);

    void stopRequested(QString handle);

    void pitchChangeRequested(QString handle, float pitch);

    void volumeChangeRequested(QString handle, float pitch);

    void locationChanged(QString handle, float x, float y);
};

#endif // AUDIOSOURCE_HPP
