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

#include "openaloggdata.hpp"

#include <AL/alc.h>

#include <cstdio>
#include <vector>
#include <stdexcept>

#include <vorbis/vorbisfile.h>

static bool checkError()
{
    ALCenum error = alGetError();
    return error == AL_NO_ERROR;
}

static const int BUFFER_SIZE = 32768; // 32 KB buffers

// This routine is originally from
// http://www.gamedev.net/page/resources/_/technical/game-programming/introduction-to-ogg-vorbis-r2031
static void loadOgg(const char * fileName, std::vector<char> & buffer, ALenum & format, ALsizei & freq)
{
    int endian = 0; // 0 for Little-Endian, 1 for Big-Endian
    int bitStream;
    long bytes;
    char array[BUFFER_SIZE]; // Local fixed size array
    FILE * f;

    // Open for binary reading
    f = std::fopen(fileName, "rb");

    vorbis_info * pInfo;
    OggVorbis_File oggFile;
    ov_open(f, &oggFile, NULL, 0);

    // Get some information about the OGG file
    pInfo = ov_info(&oggFile, -1);

    // Check the number of channels... always use 16-bit samples
    if (pInfo->channels == 1)
    {
        format = AL_FORMAT_MONO16;
    }
    else
    {
        format = AL_FORMAT_STEREO16;
    }

    freq = pInfo->rate;

    do
    {
        // Read up to a buffer's worth of decoded sound data
        bytes = ov_read(&oggFile, array, BUFFER_SIZE, endian, 2, 1, &bitStream);

        // Append to end of buffer
        buffer.insert(buffer.end(), array, array + bytes);

    } while (bytes > 0);

    ov_clear(&oggFile);
}

OpenALOggData::OpenALOggData(const std::string & path)
  : m_freq(0)
  , m_buffer(0)
{
    load(path);
}

void OpenALOggData::load(const std::string & path)
{
    Data::load(path);

    alGetError();

    if (!m_buffer)
    {
        alGenBuffers(1, &m_buffer);
    }

    std::vector<char> oggBuffer;
    loadOgg(path.c_str(), oggBuffer, m_format, m_freq);

    alBufferData(m_buffer, m_format, &oggBuffer[0], static_cast<ALsizei>(oggBuffer.size()), m_freq);

    if (!checkError())
    {
        throw std::runtime_error("Failed to set buffer data of '" + path + "'");
    }
}

ALuint OpenALOggData::buffer() const
{
    return m_buffer;
}

OpenALOggData::~OpenALOggData()
{
    alDeleteBuffers(1, &m_buffer);
}
