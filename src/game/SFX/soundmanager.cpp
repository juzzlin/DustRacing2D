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

#include "soundmanager.hpp"
#include "soundconfigloader.hpp"
#include "sound.hpp"
#include "music.hpp"

#include <QDir>
#include <cassert>

namespace SFX {

SoundManager::SoundManager()
: m_listSounds()
, m_listMusics()
, m_mapChunks()
, m_mapMusics()
, m_channel(0)
{}

void SoundManager::load(
    const std::string & configFilePath,
    const std::string & baseDataPath) throw (MCException)
{
    SoundConfigLoader loader;
    if (loader.load(configFilePath))
    {
        // Get data and create SDL sound files
        for (unsigned int i = 0; i < loader.soundCount(); i++)
        {
            const SoundMetaData & data = loader.sound(i);
            const std::string path = baseDataPath + QDir::separator().toAscii() + data.filePath;

            if (data.isMusic)
            {
                if (Mix_Music * p = Mix_LoadMUS(path.c_str()))
                {
                    m_mapMusics[data.handle] = p;
                }
                else
                {
                    throw MCException("Cannot read file '" + path + "'");
                }
            }
            else
            {
                if (Mix_Chunk * p = Mix_LoadWAV_RW(SDL_RWFromFile(path.c_str(), "rb"), 0))
                {
                    m_mapChunks[data.handle] = p;
                }
                else
                {
                    throw MCException("Cannot read file '" + path + "'");
                }
            }
        }
    }
    else
    {
        throw MCException("Parsing '" + configFilePath + "' failed!");
    }
}

Sound & SoundManager::newSound(const std::string & strId) throw (MCException)
{
    const auto chunkIter = m_mapChunks.find(strId);
    if (chunkIter == m_mapChunks.end())
    {
        throw MCException("Cannot find sound data for handle '" + strId + "'");
    }

    Sound * sound = new Sound(strId, chunkIter->second, m_channel);

    // Store for deletion
    m_listSounds.push_back(SoundPtr(sound));

    assert(sound);
    return *sound;
}

Music & SoundManager::newMusic(const std::string & strId) throw (MCException)
{
    const auto musicIter = m_mapMusics.find(strId);
    if (musicIter == m_mapMusics.end())
    {
        throw MCException("Cannot find music data for handle '" + strId + "'");
    }

    Music * music = new Music(strId, musicIter->second);

    // Store for deletion
    m_listMusics.push_back(MusicPtr(music));

    assert(music);
    return *music;
}

SoundManager::~SoundManager()
{
    auto chunk(m_mapChunks.begin());
    while (chunk != m_mapChunks.end())
    {
        if (chunk->second)
        {
            Mix_FreeChunk(chunk->second);
        }
        chunk++;
    }

    m_mapChunks.clear();

    auto music(m_mapMusics.begin());
    while (music != m_mapMusics.end())
    {
        if (music->second)
        {
            Mix_FreeMusic(music->second);
        }
        music++;
    }

    m_mapMusics.clear();
}

} // namespace SFX

