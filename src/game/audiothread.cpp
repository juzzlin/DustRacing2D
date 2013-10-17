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

AudioThread::AudioThread(QObject * parent)
    : QThread(parent)
    , m_openALDevice(new OpenALDevice)
{
}

void AudioThread::init()
{
    m_openALDevice->initialize(); // Throws on failure

    loadSounds();
}

void AudioThread::loadSounds()
{
    m_menuClick.reset(new OpenALSource(
        STFH::DataPtr(new OpenALWavData("data/sounds/146721__fins__menu-click.wav"))));
}

void AudioThread::playSound(const std::string & handle)
{
    if (handle == "menuClick")
    {
        m_menuClick->play();
    }
}

void AudioThread::run()
{
    QThread::run();
}

AudioThread::~AudioThread()
{
}
