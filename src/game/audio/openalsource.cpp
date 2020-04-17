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

#include "openalsource.hpp"
#include "openaldata.hpp"

#include <AL/alc.h>

#include <memory>
#include <stdexcept>

static bool checkError()
{
    ALCenum error = alGetError();
    return error == AL_NO_ERROR;
}

OpenALSource::OpenALSource(STFH::DataPtr data)
  : m_handle(0)
{
    alGenSources((ALuint)1, &m_handle);

    alSourcef(m_handle, AL_PITCH, pitch());
    alSourcef(m_handle, AL_GAIN, volume());
    alSource3f(m_handle, AL_POSITION, 0, 0, 0);
    alSource3f(m_handle, AL_VELOCITY, 0, 0, 0);
    alSourcei(m_handle, AL_LOOPING, AL_FALSE);

    setData(data);
}

void OpenALSource::setData(STFH::DataPtr data)
{
    alGetError();

    if (auto soundData = std::dynamic_pointer_cast<OpenALData>(data))
    {
        Source::setData(data);
        alSourcei(m_handle, AL_BUFFER, soundData->buffer());

        if (!checkError())
        {
            throw std::runtime_error("Failed to bind '" + data->path() + "'");
        }
    }
    else
    {
        throw std::runtime_error("Incompatible sound data container for '" + data->path() + "'");
    }
}

void OpenALSource::play(bool loop)
{
    alSourcei(m_handle, AL_LOOPING, loop);
    alSourcePlay(m_handle);
}

void OpenALSource::stop()
{
    alSourceStop(m_handle);
}

void OpenALSource::setVolume(float volume)
{
    STFH::Source::setVolume(volume);
    alSourcef(m_handle, AL_GAIN, volume);
}

void OpenALSource::setPitch(float pitch)
{
    STFH::Source::setPitch(pitch);
    alSourcef(m_handle, AL_PITCH, STFH::Source::pitch());
}

void OpenALSource::setLocation(const STFH::Location & location)
{
    STFH::Source::setLocation(location);
    alSource3f(m_handle, AL_POSITION, location.x(), location.y(), 0);
}

void OpenALSource::setMaxDist(float maxDist)
{
    Source::setMaxDist(maxDist);
    alSourcef(m_handle, AL_MAX_DISTANCE, maxDist);
}

void OpenALSource::setReferenceDist(float refDist)
{
    Source::setReferenceDist(refDist);
    alSourcef(m_handle, AL_REFERENCE_DISTANCE, refDist);
}

OpenALSource::~OpenALSource()
{
    alDeleteSources(1, &m_handle);
}
