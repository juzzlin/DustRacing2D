// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#include "mcsoundmanager.hh"
#include "mcsoundparser.hh"
#include "mcsound.hh"
#include "mcmusic.hh"

#include <QDir>

MCSoundManager::MCSoundManager() :
  listSounds(),
  listMusics(),
  mapChunks(),
  mapMusics(),
  nChannel(0)
{}

void MCSoundManager::load(const QString & strFileName, const QString & strBaseDataPath, bool bAllowNullChunks) throw (MCException)
{
  // Instantiate a parser
  MCSoundParser handler;

  // Instantiate a QFile for the texture file
  QFile xmlFile(strFileName);

  // Set QXmlInputSource to the QFile
  QXmlInputSource source(&xmlFile);

  // Instantiate a simple (SAX2) reader
  QXmlSimpleReader reader;

  // Set it to use the MCSoundParser as the handler
  reader.setContentHandler(&handler);
  reader.setErrorHandler(&handler);

  // Parse the sound config file
  if (reader.parse(source))
  {
    // Get data and create SDL sound files
    QList<MCSoundData> soundDataList = handler.soundData();
    Q_FOREACH(MCSoundData data, soundDataList)
    {
      QString strHandle = data.strHandle;
      QString strPath   = strBaseDataPath + QDir::separator().toAscii() + data.strFile;

      if (data.isMusic)
      {
        Mix_Music * p = Mix_LoadMUS(strPath.toStdString().c_str());
        if (p || bAllowNullChunks)
        {
          mapMusics[strHandle] = p;
        }
        else
        {
          throw MCException("Cannot read file '" + strPath + "'");
        }
      }
      else
      {
        Mix_Chunk * p    = Mix_LoadWAV_RW(SDL_RWFromFile(strPath.toStdString().c_str(), "rb"), 0);
        if (p || bAllowNullChunks)
        {
          mapChunks[strHandle] = p;
        }
        else
        {
          throw MCException("Cannot read file '" + strPath + "'");
        }
      }
    }
  }
}

MCSound * MCSoundManager::newSound(const QString & strId) throw (MCException)
{
  // Try to find an existing chunk
  if (!mapChunks.contains(strId))
  {
    // No: throw an exception
    throw MCException("MCSoundManager::getSound(): Cannot find sound object for handle '" +
                      strId + "'");
    return NULL;
  }
  else
  {
    // Yes: create a new Sound using the chunk and return it
    MCSound * pSound = new MCSound(strId, mapChunks.find(strId).value(), nChannel);

    // Store for deletion
    listSounds << shared_ptr<MCSound>(pSound);

    // Return the sound
    return pSound;
  }
}

MCMusic * MCSoundManager::newMusic(const QString & strId) throw (MCException)
{
  // Try to find an existing music
  if (!mapMusics.contains(strId))
  {
    // No: throw an exception
    throw MCException("MCSoundManager::getMusic(): Cannot find music object for handle '" +
                      strId + "'");;
    return NULL;
  }
  else
  {
    // Yes: create a new Music using the chunk and return it
    MCMusic * pMusic = new MCMusic(strId, mapMusics.find(strId).value());

    // Store for deletion
    listMusics << shared_ptr<MCMusic>(pMusic);

    // Return the music 
    return pMusic;
  }
}

MCSoundManager::~MCSoundManager()
{
  // Delete Mix_Chunks
  ChunkHash::iterator iter(mapChunks.begin());
  while (iter != mapChunks.end())
  {
    if (iter.value())
    {
      Mix_FreeChunk(iter.value());
    }
    iter++;
  }
  mapChunks.clear();

  // Delete Mix_Musics
  MusicHash::iterator iter2(mapMusics.begin());
  while (iter2 != mapMusics.end())
  {
    if (iter2.value())
    {
      Mix_FreeMusic(iter2.value());
    }
    iter2++;
  }
  mapMusics.clear();
}
