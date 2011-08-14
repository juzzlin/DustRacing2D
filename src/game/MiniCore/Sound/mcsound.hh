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

#ifndef MCSOUND_HH
#define MCSOUND_HH

#include <SDL/SDL_mixer.h>
#include <QString>
#include <QVector>

#include "mcobject.hh"
#include "mcexception.hh"

/*! \class MCSound
 *  \brief A wrapper for SDL's Mix_Chunk with automatic panning feature.
 */
class MCSound
{
public:

  /*! Constructor
   * \param strName Name / handle for the sound
   * \param pData   Actual sound data
   * \param channel Default channel for the sound
   */
  MCSound(QString strName, Mix_Chunk * pData, int channel);

  //! Destructor
  ~MCSound();

  /*! Play sound for nTimes times using given location for the sound.
   *  addReferenceObject() must have been called with a legal object.
   * \param sourceX X-coordinate for the sound.
   * \param sourceY Y-coordinate for the sound.
   * \param loops Number of loops to be played.
   * \return channel playing, -1 on failure.
   */
  int play(int sourceX, int sourceY, int loops) throw (MCException);

  /*! Play sound
   * \param loops Number of loops to be played.
   * \param volume Volume 0..MIX_MAX_VOLUME.
   * \return channel playing, -1 on failure.
   */
  int play(int loops, int volume) throw (MCException);

  /*! Play sound for given times using MCSound::volume() as the volume.
   * \param loops Number of loops to be played
   * \return channel playing, -1 on failure.
   */
  int play(int loops) throw (MCException);

  //! Stop sound
  void stop();

  //! Stop sound (all sounds in the given channel)
  void stop(int channel);

  //! Get channel of the sound
  int channel() const;

  /*! Add reference object (listener) to scale volume with. Maximum value is
   *  selected if multiple reference objects are defined.
   */
  static void addReferenceObject(MCObject * pObject);

  //! Set area boundaries.
  static void setSoundAreaBoundaries(int x1, int y1, int x2, int y2);

  //! Stop all currently playing sounds.
  static void stopSounds();

  /*! Enable / disable sound system.
   *  No calls to underlying SDL_mixer are done when disabled.
   */
  static void enable(bool flag);

  /*! Set global default volume.
   * \param volume Volume 0..MIX_MAX_VOLUME.
   */
  static void setVolume(int volume);

  //! Get global volume
  static int volume();

  //! Set/allocate channels
  static void setChannels(int numChannels);
  
private:

  //! Hidden assignment
  MCSound & operator= (const MCSound & r);

  //! Hidden copy constructor
  MCSound(const MCSound & r);

  //! Enable / disable sound system
  static bool m_enable;

  //! Global max sound level
  static int m_volume;

  //! Vector of reference objects (sound sources)
  static QVector<MCObject *> m_referenceObjects;

  //! Area boundaries
  static int m_soundAreaX1, m_soundAreaY1, m_soundAreaX2, m_soundAreaY2;

  //! Maximum distance within the sound is played
  static int m_maxDist;

  //! Sound volume scaler logic
  int getScaledVolume(int sourceX, int sourceY);

  //! Name of the sound
  QString m_strName;

  //! Current channel
  int m_channel;

  //! SDL sound data
  Mix_Chunk * m_data;
};

#endif // MCSOUND_HH
