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
#include "openalwavdata.hpp"

#include <QDir>
#include <QString>

AudioThread::AudioThread(QObject * parent)
    : QThread(parent)
    , m_openALDevice(new OpenALDevice)
    , m_inited(false)
{
}

void AudioThread::init()
{
    m_openALDevice->initialize(); // Throws on failure
}

void AudioThread::loadSounds()
{
    loadSound("menuClick", "146721__fins__menu-click.wav");
    loadSound("carEngine", "147242__qubodup__car-engine-loop.wav");
}

void AudioThread::loadSound(const std::string & handle, const std::string & path)
{
    const std::string soundPath =
        (QString(DATA_PATH) + QDir::separator() + "sounds" + QDir::separator()).toStdString();

    m_soundMap[handle] =
        STFH::SourcePtr(new OpenALSource(
            STFH::DataPtr(new OpenALWavData(soundPath + path))));
}

void AudioThread::playSound(const std::string & handle)
{
    if (m_inited)
    {
        if (m_soundMap.count(handle))
        {
            m_soundMap[handle]->play();
        }
    }
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
