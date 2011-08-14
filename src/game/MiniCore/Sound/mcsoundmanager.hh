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

#ifndef MCSOUNDMANAGER_HH
#define MCSOUNDMANAGER_HH

#include "mcexception.hh"
#include "mcmacros.hh"

#include <SDL/SDL_mixer.h>
#include <QString>
#include <QHash>
#include <QList>

#include <tr1/memory>

using std::tr1::shared_ptr;

class MCSound;
class MCMusic;

/*! \class MCSoundManager
 *  \brief Sound/music management class operating on (wrapping) SDL's sound stuff.
 */
class MCSoundManager
{
public:

  //! Constructor
  MCSoundManager();
  //! Destructor
  virtual ~MCSoundManager();

  //! Loads sounds and music strBaseDataPath using the given mapping file strFile
  virtual void load(const QString & strFile, const QString & strBaseDataPath,
                    bool bAllowNullChunks) throw (MCException);

  //! Returns a new sound object associated with strId
  MCSound * newSound(const QString & strId) throw (MCException);

  //! Returns a new music object associated with strId
  MCMusic * newMusic(const QString & strId) throw (MCException);

private:

  //! Disable copy constructor
  DISABLE_COPY(MCSoundManager);

  //! Disable assignment
  DISABLE_ASSI(MCSoundManager);

  //! Vector for resulting Sounds
  typedef QList<shared_ptr<MCSound> > SoundList;
  SoundList listSounds;

  //! Vector for resulting Musics
  typedef QList<shared_ptr<MCMusic> > MusicList;
  MusicList listMusics;

  //! Map for SDL's Mix_Chunks
  typedef QHash<QString, Mix_Chunk *> ChunkHash;
  ChunkHash mapChunks;

  //! Map for SDL's Mix_Musics
  typedef QHash<QString, Mix_Music *> MusicHash;
  MusicHash mapMusics;

  //! Channel counter
  int nChannel;
};

#endif // MCSOUNDMANAGER_HH
