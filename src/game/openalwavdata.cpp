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

#include "openalwavdata.hpp"
#include <MCException>

#include <AL/alc.h>
#include <AL/alut.h>

static bool checkError()
{
    ALCenum error = alGetError();
    return error == AL_NO_ERROR;
}

OpenALWavData::OpenALWavData(const std::string & path)
    : m_data(nullptr)
    , m_buffer(0)
    , m_loop(AL_FALSE)
{
    load(path);
}

void OpenALWavData::load(const std::string & path) throw ()
{
    Data::load(path);

    alGetError();

    if (!m_buffer)
    {
        alGenBuffers(1, &m_buffer);
    }

    alutLoadWAVFile(
        reinterpret_cast<ALbyte *>(const_cast<char *>(path.c_str())),
        &m_format, &m_data, &m_size, &m_freq, &m_loop);
    if (!checkError())
    {
        throw MCException("Failed to load '" + path + "'");
    }

    alBufferData(m_buffer, m_format, m_data, m_size, m_freq);
    if (!checkError())
    {
        throw MCException("Failed to set buffer data of '" + path + "'");
    }
}

ALvoid * OpenALWavData::data()
{
    return m_data;
}

ALsizei OpenALWavData::size()
{
    return m_size;
}

ALsizei OpenALWavData::freq()
{
    return m_freq;
}

ALenum OpenALWavData::format()
{
    return m_format;
}

ALuint OpenALWavData::buffer()
{
    return m_buffer;
}

OpenALWavData::~OpenALWavData()
{
    alDeleteBuffers(1, &m_buffer);
}
