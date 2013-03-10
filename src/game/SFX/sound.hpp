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

#ifndef SOUND_HPP
#define SOUND_HPP

#include <SDL/SDL_mixer.h>

#include <string>
#include <vector>

namespace SFX {

/*! \class Sound
 *  \brief A wrapper for SDL's Mix_Chunk with automatic panning feature.
 */
class Sound
{
public:

    //! Interface for listener objects.
    class SoundListenerIf
    {
    public:

        //! Get ear x in World coordinates.
        virtual int getEarX() = 0;

        //! Get ear y in World coordinates.
        virtual int getEarY() = 0;
    };

    /*! Constructor
     * \param name    Name / handle of the sound.
     * \param data    The actual sound data.
     * \param channel Default channel for the sound. */
    Sound(std::string name, Mix_Chunk * data, int channel);

    //! Destructor
    virtual ~Sound();

    /*! Play sound for nTimes times using given location for the sound.
     *  addListener() must have been called with a legal object, otherwise
     *  a zero volume is used. See also setSoundAreaBoundaries().
     * \param sourceX X-coordinate for the sound.
     * \param sourceY Y-coordinate for the sound.
     * \param loops Number of loops to be played.
     * \return channel playing, -1 on failure. */
    int play(int sourceX, int sourceY, int loops);

    /*! Play sound
     * \param loops Number of loops to be played.
     * \param volume Volume 0.0..1.0
     * \return channel playing, -1 on failure. */
    int play(int loops, float volume);

    /*! Play sound for given times using Sound::volume() as the volume.
     * \param loops Number of loops to be played
     * \return channel playing, -1 on failure. */
    int play(int loops = 0);

    //! Stop sound
    void stop();

    //! Stop sound (all sounds in the given channel)
    void stop(int channel);

    //! Get channel of the sound
    int channel() const;

    /*! Set volume.
     * \param volume Volume: 0.0..1.0 */
    void setVolume(float volume);

    //! Get volume.
    float volume();

    //! Add listener object to scale volume with.
    static void addListener(Sound::SoundListenerIf & listener);

    //! Remove all listeners.
    static void removeListeners();

    //! Set area boundaries.
    static void setSoundAreaBoundaries(int x1, int y1, int x2, int y2);

    //! Stop all currently playing sounds.
    static void stopSounds();

    /*! Enable / disable sound system.
     *  No calls to underlying SDL_mixer are done when disabled. */
    static void enable(bool flag);

    //! Set/allocate channels
    static void setChannels(int numChannels);
  
    //! \return The name/handle of the sound object.
    const std::string & name() const;

private:

    //! Hidden assignment
    Sound & operator= (const Sound & r);

    //! Hidden copy constructor
    Sound(const Sound & r);

    //! Enable / disable sound system
    static bool m_enabled;

    //! Global max sound level
    static float m_volume;

    //! Vector of reference objects (sound sources)
    static std::vector<SoundListenerIf *> m_listeners;

    //! Area boundaries
    static int m_soundAreaX1, m_soundAreaY1, m_soundAreaX2, m_soundAreaY2;

    //! Maximum distance within the sound is played
    static int m_maxDist;

    //! Sound volume scaler logic
    int getScaledSDLVolume(int sourceX, int sourceY);

    //! Name of the sound
    std::string m_name;

    //! Current channel
    int m_channel;

    //! SDL sound data
    Mix_Chunk * m_data;
};

} // namespace SFX

#endif // SOUND_HPP
