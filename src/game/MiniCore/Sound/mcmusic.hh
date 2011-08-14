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

#ifndef MCMUSIC_HH
#define MCMUSIC_HH

#include <SDL/SDL_mixer.h>
#include <QString>

#include "mcexception.hh"

/*! \class MCMusic
 *  \brief Wrapper for SDL's music data.
 */
class MCMusic
{
public:

  /*! Constructor
   * \param strName Name / handle of the music track
   * \param pData The actual music data
   */
  MCMusic(QString strName, Mix_Music * pData);

  //! Destructor
  virtual ~MCMusic();

  /*! Play music for given times using given volume.
   * \param loops Play loops times, -1 for play "forever".
   * \param volume Volume 0..MIX_MAX_VOLUME.
   * \return channel playing, -1 on failure.
   */
  int play(int loops, int volume) throw (MCException);

  /*! Play music for nTimes using default volume set by MCMusic::setVolume().
   * \param loops Play loops times, -1 for play "forever".
   * \return channel playing, -1 on failure.
   */
  int play(int loops) throw (MCException);

  //! Stop music
  static void stop();

  /*! Set default volume
   * \param volume Volume 0..MIX_MAX_VOLUME.
   */
  static void setVolume(int volume);

  //! Get current global volume.
  static int volume();

  /*! Enable / disable MCMusic.
   *  If disabled, no calls to underlying SDL_mixer are performed.
   */
  static void enable(bool flag);

private:

  //! Hidden assignment
  MCMusic & operator= (const MCMusic & r);

  //! Hidden copy constructor
  MCMusic(const MCMusic & r);

  //! Enable / disable music system
  static bool m_enable;

  //! Default volume
  static int m_volume;

  //! Name of the music
  QString m_strName;

  //! SDL music data
  Mix_Music * m_data;
};

#endif // GAME_MUSIC_HH
