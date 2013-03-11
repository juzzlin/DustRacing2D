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
Sound::ChannelToSoundMap Sound::m_channelToSoundMap;

bool  Sound::m_enabled = true;
int   Sound::m_maxDist = 0;

static const float MIN_SDL_VOLUME = 5;
static const float MIN_VOLUME     = 0.1;

namespace {
// Converts 0.0..1.0 scaled volume to SDL's internal volume.
static int getSDLVolume(float volume)
{
    volume = std::max(volume, 0.0f);
    volume = std::min(volume, 1.0f);

    return static_cast<int>(volume * MIX_MAX_VOLUME);
}
}

Sound::Sound(std::string name, Mix_Chunk * data, int channel)
: m_name(name)
, m_volume(0.5)
, m_defaultChannel(channel)
, m_currentChannel(-1)
, m_sourceX(0)
, m_sourceY(0)
, m_data(data)
, m_positional(false)
{}

void Sound::channelCallback(int channel)
{
    // The sound playing on this channel is not active anymore.
    Sound::m_channelToSoundMap[channel] = nullptr;
}

void Sound::init()
{
    // Set the channel callback.
    Mix_ChannelFinished(Sound::channelCallback);
}

int Sound::play(int sourceX, int sourceY, int loops)
{
    if (Sound::m_enabled)
    {
        m_sourceX = sourceX;
        m_sourceY = sourceY;

        const float scaledVolume = Sound::getScaledVolume(m_sourceX, m_sourceY);
        if (scaledVolume > MIN_VOLUME)
        {
            setVolume(scaledVolume);
            m_currentChannel = Mix_PlayChannel(m_defaultChannel, m_data, loops);
            m_positional = true;
            Sound::m_channelToSoundMap[m_currentChannel] = this;
            return m_currentChannel;
        }
    }

    return -1;
}

int Sound::play(int loops)
{
    if (Sound::m_enabled)
    {
        Mix_VolumeChunk(m_data, getSDLVolume(m_volume));
        m_currentChannel = Mix_PlayChannel(m_defaultChannel, m_data, loops);
        m_positional = false;
        Sound::m_channelToSoundMap[m_currentChannel] = this;
        return m_currentChannel;
    }

    return -1;
}

int Sound::sourceX() const
{
    return m_sourceX;
}

int Sound::sourceY() const
{
    return m_sourceY;
}

void Sound::stop()
{
    if (Sound::m_enabled)
    {
        if (m_currentChannel != -1)
        {
            Mix_HaltChannel(m_currentChannel);
            Sound::m_channelToSoundMap[m_currentChannel] = nullptr;
            m_currentChannel = -1;
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
            Sound::m_channelToSoundMap[m_currentChannel] = nullptr;
        }
    }
}

void Sound::stopSounds()
{
    if (Sound::m_enabled)
    {
        Mix_HaltChannel(-1);
        Sound::m_channelToSoundMap.clear();
    }
}

void Sound::setDefaultChannel(int channel)
{
    m_defaultChannel = channel;
}

int Sound::currentChannel() const
{
    return m_currentChannel;
}

float Sound::getScaledVolume(int sourceX, int sourceY)
{
    int listenerX, listenerY;
    Sound::calculateAverageListenerPosition(listenerX, listenerY);

    const int dx = std::abs(sourceX - listenerX);
    const int dy = std::abs(sourceY - listenerY);
    const int da = std::max(dx, dy) + std::min(dx, dy) / 2;

    return float(m_maxDist - da) / m_maxDist;
}

void Sound::calculateAverageListenerPosition(int & listenerX, int & listenerY)
{
    listenerX = 0;
    listenerY = 0;

    if (Sound::m_listeners.size())
    {
        for (SoundListenerIf * listener : Sound::m_listeners)
        {
            listenerX += listener->getEarX();
            listenerY += listener->getEarY();
        }

        listenerX /= Sound::m_listeners.size();
        listenerY /= Sound::m_listeners.size();
    }
}

void Sound::addListener(Sound::SoundListenerIf & listener)
{
    Sound::m_listeners.push_back(&listener);
}

void Sound::removeListeners()
{
    Sound::m_listeners.clear();
}

void Sound::setMaximumDistance(int distance)
{
    Sound::m_maxDist = distance;
}

void Sound::enable(bool bFlag)
{
    Sound::m_enabled = bFlag;
}

void Sound::setVolume(float volume)
{
    if (Sound::m_enabled)
    {
        m_volume = volume;
        Mix_VolumeChunk(m_data, getSDLVolume(Sound::m_volume));
    }
}

float Sound::volume()
{
    return m_volume;
}

void Sound::updateVolumes()
{
    auto iter = Sound::m_channelToSoundMap.begin();
    while (iter != Sound::m_channelToSoundMap.end())
    {
        if (iter->second && iter->second->m_positional)
        {
            iter->second->setVolume(
                getScaledVolume(iter->second->m_sourceX, iter->second->m_sourceY));
        }

        iter++;
    }
}

int Sound::allocateChannels(int numChannels)
{
    if (Sound::m_enabled)
    {
        return Mix_AllocateChannels(numChannels);
    }

    return 0;
}

const std::string & Sound::name() const
{
    return m_name;
}

bool Sound::isPlaying()
{
    return Sound::m_channelToSoundMap[m_currentChannel] == this;
}

Sound::~Sound()
{}

} // namespace SFX
