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

#ifndef SOUNDMANAGER_HPP
#define SOUNDMANAGER_HPP

#include <MCException>
#include <MCMacros>

#include <list>
#include <memory>
#include <string>
#include <unordered_map>

#include <SDL/SDL_mixer.h>

namespace SFX {

class Sound;
class Music;

/*! \class SoundManager
 *  \brief Sound/music management class operating on (wrapping) SDL's sound stuff. */
class SoundManager
{
public:

    //! Constructor
    SoundManager();

    //! Destructor
    virtual ~SoundManager();

    //! Loads sounds from strBasePath using the given mapping file.
    //! \param configFilePath Path to the XML-based input file.
    //! \param baseDataPath The absolute search path for an image is
    //! baseDataPath + baseSoundPath + fileName. baseSoundPath and the fileName are
    //! defined in the input file.
    virtual void load(
        const std::string & configFilePath,
        const std::string & baseDataPath) throw (MCException);

    //! Returns a new sound object using the sound data associated with strId.
    Sound & newSound(const std::string & strId) throw (MCException);

    //! Returns a new music object using the music data associated with strId.
    Music & newMusic(const std::string & strId) throw (MCException);

private:

  //! Disable copy constructor
  DISABLE_COPY(SoundManager);

  //! Disable assignment
  DISABLE_ASSI(SoundManager);

  //! Vector for resulting Sounds
  typedef std::shared_ptr<Sound> SoundPtr;
  typedef std::list<SoundPtr> SoundList;
  SoundList m_listSounds;

  //! Vector for resulting Musics
  typedef std::shared_ptr<Music> MusicPtr;
  typedef std::list<MusicPtr> MusicList;
  MusicList m_listMusics;

  //! Map for SDL's Mix_Chunks
  typedef std::unordered_map<std::string, Mix_Chunk *> ChunkHash;
  ChunkHash m_mapChunks;

  //! Map for SDL's Mix_Musics
  typedef std::unordered_map<std::string, Mix_Music *> MusicHash;
  MusicHash m_mapMusics;

  //! Channel counter
  int m_channel;
};

} // namespace SFX

#endif // SOUNDMANAGER_HPP
