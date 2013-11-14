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

#include "audiothread.hpp"
#include "audiosource.hpp"
#include "openalwavdata.hpp"
#include "openaloggdata.hpp"

#include <QDir>
#include <QString>

#include <sstream>

#include <AL/al.h>

AudioThread::AudioThread(int numCars, QObject * parent)
    : QThread(parent)
    , m_openALDevice(new OpenALDevice)
    , m_inited(false)
    , m_masterVolume(1.0)
    , m_numCars(numCars)
{
}

void AudioThread::init()
{
    m_openALDevice->initialize(); // Throws on failure

    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
    alSpeedOfSound(1000.0);
}

void AudioThread::connectAudioSource(AudioSource & source)
{
    connect(&source, SIGNAL(playRequested(QString, bool)),
        this, SLOT(playSound(QString, bool)));
    connect(&source, SIGNAL(stopRequested(QString)),
        this, SLOT(stopSound(QString)));
    connect(&source, SIGNAL(pitchChangeRequested(QString, float)),
        this, SLOT(setPitch(QString, float)));
    connect(&source, SIGNAL(locationChanged(QString, float, float)),
        this, SLOT(setLocation(QString, float, float)));
}

void AudioThread::disconnectAudioSource(AudioSource & source)
{
    disconnect(&source, SIGNAL(playRequested(QString, bool)),
        this, SLOT(playSound(QString, bool)));
    disconnect(&source, SIGNAL(stopRequested(QString)),
        this, SLOT(stopSound(QString)));
    disconnect(&source, SIGNAL(pitchChangeRequested(QString, float)),
        this, SLOT(setPitch(QString, float)));
    disconnect(&source, SIGNAL(locationChanged(QString, float, float)),
        this, SLOT(setLocation(QString, float, float)));
}

void AudioThread::loadSounds()
{
    loadSound("menuClick", "menuClick.ogg");
    loadSound("pit", "pit.ogg");

    loadEngineSounds("carEngine.ogg");
}

void AudioThread::loadSound(QString handle, QString path)
{
    const QString soundPath =
        QString(DATA_PATH) + QDir::separator() + "sounds" + QDir::separator();

    m_soundMap[handle] =
        STFH::SourcePtr(new OpenALSource(
            STFH::DataPtr(new OpenALOggData((soundPath + path).toStdString()))));
}

void AudioThread::loadEngineSounds(QString path)
{
    const QString soundPath =
        QString(DATA_PATH) + QDir::separator() + "sounds" + QDir::separator();

    STFH::DataPtr sharedData(new OpenALOggData((soundPath + path).toStdString()));

    for (int i = 0; i < m_numCars; i++)
    {
        std::stringstream ss;
        ss << "carEngine" << i;
        STFH::SourcePtr source(new OpenALSource(sharedData));
        m_soundMap[ss.str().c_str()] = source;
        source->setMaxDist(250);
        source->setReferenceDist(50);
    }
}

void AudioThread::playSound(QString handle, bool loop)
{
    if (m_soundMap.count(handle))
        m_soundMap[handle]->play(loop);
}

void AudioThread::stopSound(QString handle)
{
    if (m_soundMap.count(handle))
        m_soundMap[handle]->stop();
}

void AudioThread::setPitch(QString handle, float pitch)
{
    if (m_soundMap.count(handle))
        m_soundMap[handle]->setPitch(pitch);
}

void AudioThread::setVolume(QString handle, float volume)
{
    if (m_soundMap.count(handle))
        m_soundMap[handle]->setVolume(volume);
}

void AudioThread::setMasterVolume(float volume)
{
    m_masterVolume = volume;
}

void AudioThread::setLocation(QString handle, float x, float y)
{
    if (m_soundMap.count(handle))
        m_soundMap[handle]->setLocation(STFH::Location(x, y));
}

void AudioThread::setListenerLocation(float x, float y)
{
    alListener3f(AL_POSITION, x, y, 1);
}

void AudioThread::run()
{
    if (!m_inited)
    {
        init();
        loadSounds();

        m_inited = true;
    }

    QThread::run();
}

AudioThread::~AudioThread()
{
}
