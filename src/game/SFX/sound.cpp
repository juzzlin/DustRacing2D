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

#include "sound.hpp"

#include <cmath>

namespace SFX {

std::vector<Sound::SoundListenerIf *> Sound::m_listeners;

bool  Sound::m_enabled     = true;
int   Sound::m_soundAreaX1 = 0;
int   Sound::m_soundAreaY1 = 0;
int   Sound::m_soundAreaX2 = 0;
int   Sound::m_soundAreaY2 = 0;
int   Sound::m_maxDist     = 0;
float Sound::m_volume      = 0.5;

static const float MIN_SDL_VOLUME = 5;

namespace {
// Converts 0.0..1.0 scaled volume to SDL's internal volume.
static int getSDLVolume(float volume)
{
    volume = std::max(volume, 0.0f);
    volume = std::min(volume, 1.0f);

    return static_cast<int>(volume * MIX_MAX_VOLUME);
}
}

Sound::Sound(std::string name, Mix_Chunk * data, int newChannel)
: m_name(name)
, m_channel(newChannel)
, m_data(data)
{}

int Sound::play(int sourceX, int sourceY, int loops)
{
    if (Sound::m_enabled)
    {
        const int scaledVolume = getScaledSDLVolume(sourceX, sourceY);
        if (scaledVolume > MIN_SDL_VOLUME)
        {
            Mix_VolumeChunk(m_data, scaledVolume);
            m_channel = Mix_PlayChannel(-1, m_data, loops);
            return m_channel;
        }
    }

    return -1;
}

int Sound::play(int loops, float volume)
{
    if (Sound::m_enabled)
    {
        Mix_VolumeChunk(m_data, getSDLVolume(volume));
        m_channel = Mix_PlayChannel(-1, m_data, loops);
        return m_channel;
    }

    return -1;
}

int Sound::play(int loops)
{
    if (Sound::m_enabled)
    {
        Mix_VolumeChunk(m_data, getSDLVolume(Sound::m_volume));
        m_channel = Mix_PlayChannel(-1, m_data, loops);
        return m_channel;
    }

    return -1;
}

void Sound::stop()
{
    if (Sound::m_enabled)
    {
        if (m_channel != -1)
        {
            Mix_HaltChannel(m_channel);

            m_channel = -1;
        }
    }
}

void Sound::stop(int channel)
{
    if (Sound::m_enabled)
    {
        if (channel != -1)
        {
            Mix_HaltChannel(channel);
        }
    }
}

void Sound::stopSounds()
{
    if (Sound::m_enabled)
    {
        Mix_HaltChannel(-1);
    }
}

int Sound::channel() const
{
    return m_channel;
}

int Sound::getScaledSDLVolume(int sourceX, int sourceY)
{
    SoundListenerIf * listener = nullptr;
    int refX, refY, d, dX, dY, result;
    result = 0;

    for (unsigned int i = 0; i < Sound::m_listeners.size(); i++)
    {
        listener = Sound::m_listeners[i];
        refX     = listener->getEarX();
        refY     = listener->getEarY();
        dX       = std::abs(refX - sourceX);
        dY       = std::abs(refY - sourceY);
        d        = std::max(dX, dY) + std::min(dX, dY) / 2;
        d        = (Sound::m_maxDist) - d;
        d        = d < 0 ? 0 : d;
        d        = d * getSDLVolume(Sound::m_volume) / Sound::m_maxDist;

        if (!i || d > result)
        {
            result = d;
        }
    }

    return result;
}

void Sound::addListener(Sound::SoundListenerIf & listener)
{
    Sound::m_listeners.push_back(&listener);
}

void Sound::removeListeners()
{
    Sound::m_listeners.clear();
}

void Sound::setSoundAreaBoundaries(int x1, int y1, int x2, int y2)
{
    int dX, dY;

    Sound::m_soundAreaX1 = x1;
    Sound::m_soundAreaY1 = y1;
    Sound::m_soundAreaX2 = x2;
    Sound::m_soundAreaY2 = y2;

    dX = std::abs(Sound::m_soundAreaX2 - Sound::m_soundAreaX1);
    dY = std::abs(Sound::m_soundAreaY2 - Sound::m_soundAreaY1);

    Sound::m_maxDist = std::max(dX, dY) / 2;
}

void Sound::enable(bool bFlag)
{
    Sound::m_enabled = bFlag;
}

void Sound::setVolume(float volume)
{
    if (Sound::m_enabled)
    {
        Sound::m_volume = volume;
        Mix_VolumeChunk(m_data, getSDLVolume(Sound::m_volume));
    }
}

float Sound::volume()
{
    return Sound::m_volume;
}

void Sound::setChannels(int numChannels)
{
    if (Sound::m_enabled)
    {
        Mix_AllocateChannels(numChannels);
    }
}

const std::string & Sound::name() const
{
    return m_name;
}

Sound::~Sound()
{}

} // namespace SFX
