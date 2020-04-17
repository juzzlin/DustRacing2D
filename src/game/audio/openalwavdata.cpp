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

#define _CRT_SECURE_NO_WARNINGS

// Always enable asserts
#ifdef NDEBUG
#undef NDEBUG
#endif

#include "openalwavdata.hpp"

#include <AL/alc.h>
#include <cassert>
#include <cstdio>
#include <stdexcept>

static bool checkError()
{
    ALCenum error = alGetError();
    return error == AL_NO_ERROR;
}

// WAV load code from http://www.dunsanyinteractive.com/blogs/oliver/?p=72
// by Oliver Plunkett

struct RIFF_Header
{
    char chunkID[4];
    int32_t chunkSize;
    char format[4];
};

struct WAVE_Format
{
    char subChunkID[4];
    int32_t subChunkSize;
    int16_t audioFormat;
    int16_t numChannels;
    int32_t sampleRate;
    int32_t byteRate;
    int16_t blockAlign;
    int16_t bitsPerSample;
};

struct WAVE_Data
{
    char subChunkID[4]; // Should contain the word data
    int32_t subChunk2Size; // Stores the size of the data block
};

static bool loadWavFile(
  const std::string & filename,
  ALuint * buffer,
  ALsizei * size,
  ALsizei * frequency,
  ALenum * format)
{
    FILE * soundFile = nullptr;
    WAVE_Format wave_format;
    RIFF_Header riff_header;
    WAVE_Data wave_data;
    unsigned char * data;

    try
    {
        soundFile = fopen(filename.c_str(), "rb");
        if (!soundFile)
            throw std::runtime_error(filename);

        // Read in the first chunk into the struct
        size_t bytesRead = std::fread(&riff_header, sizeof(RIFF_Header), 1, soundFile);
        assert(bytesRead == sizeof(RIFF_Header));

        //check for RIFF and WAVE tag in memeory
        if ((riff_header.chunkID[0] != 'R' || riff_header.chunkID[1] != 'I' || riff_header.chunkID[2] != 'F' || riff_header.chunkID[3] != 'F') || (riff_header.format[0] != 'W' || riff_header.format[1] != 'A' || riff_header.format[2] != 'V' || riff_header.format[3] != 'E'))
            throw std::runtime_error("Invalid RIFF or WAVE Header");

        //Read in the 2nd chunk for the wave info
        bytesRead = std::fread(&wave_format, sizeof(WAVE_Format), 1, soundFile);
        assert(bytesRead == sizeof(WAVE_Format));

        //check for fmt tag in memory
        if (wave_format.subChunkID[0] != 'f' || wave_format.subChunkID[1] != 'm' || wave_format.subChunkID[2] != 't' || wave_format.subChunkID[3] != ' ')
            throw std::runtime_error("Invalid Wave Format");

        //check for extra parameters;
        if (wave_format.subChunkSize > 16)
            std::fseek(soundFile, sizeof(int16_t), SEEK_CUR);

        //Read in the the last byte of data before the sound file
        bytesRead = std::fread(&wave_data, sizeof(WAVE_Data), 1, soundFile);
        assert(bytesRead == sizeof(WAVE_Data));

        //check for data tag in memory
        if (wave_data.subChunkID[0] != 'd' || wave_data.subChunkID[1] != 'a' || wave_data.subChunkID[2] != 't' || wave_data.subChunkID[3] != 'a')
            throw std::runtime_error("Invalid data header");

        //Allocate memory for data
        data = new unsigned char[wave_data.subChunk2Size];

        // Read in the sound data into the soundData variable
        if (!std::fread(data, static_cast<size_t>(wave_data.subChunk2Size), 1, soundFile))
            throw std::runtime_error("error loading WAVE data into struct!");

        //Now we set the variables that we passed in with the
        //data from the structs
        *size = wave_data.subChunk2Size;
        *frequency = wave_format.sampleRate;
        //The format is worked out by looking at the number of
        //channels and the bits per sample.
        if (wave_format.numChannels == 1)
        {
            if (wave_format.bitsPerSample == 8)
                *format = AL_FORMAT_MONO8;
            else if (wave_format.bitsPerSample == 16)
                *format = AL_FORMAT_MONO16;
        }
        else if (wave_format.numChannels == 2)
        {
            if (wave_format.bitsPerSample == 8)
                *format = AL_FORMAT_STEREO8;
            else if (wave_format.bitsPerSample == 16)
                *format = AL_FORMAT_STEREO16;
        }
        //create our openAL buffer and check for success
        alGenBuffers(1, buffer);
        checkError();
        //now we put our data into the openAL buffer and
        //check for success
        alBufferData(*buffer, *format, reinterpret_cast<void *>(data), *size, *frequency);
        checkError();
        //clean up and return true if successful
        fclose(soundFile);
        return true;
    } catch (std::string error)
    {
        //clean up memory if wave loading fails
        std::fclose(soundFile);
        //return false to indicate the failure to load wave
        return false;
    }
}

OpenALWavData::OpenALWavData(const std::string & path)
  : m_size(0)
  , m_freq(0)
  , m_buffer(0)
{
    load(path);
}

void OpenALWavData::load(const std::string & path)
{
    OpenALData::load(path);

    if (!loadWavFile(path, &m_buffer, &m_size, &m_freq, &m_format))
    {
        throw std::runtime_error("Failed to load '" + path + "'");
    }

    if (!checkError())
    {
        throw std::runtime_error("Failed to set buffer data of '" + path + "'");
    }
}

ALuint OpenALWavData::buffer() const
{
    return m_buffer;
}

OpenALWavData::~OpenALWavData()
{
    alDeleteBuffers(1, &m_buffer);
}
