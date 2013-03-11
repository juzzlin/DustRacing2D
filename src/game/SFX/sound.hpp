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

#include <map>
#include <string>
#include <vector>

#include "device.hpp"

namespace SFX {

/*! \class Sound
 *  \brief A wrapper for SDL's Mix_Chunk with automatic positional volume control. */
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
     * \param name Name / handle of the sound.
     * \param data The actual sound data.
     * \param defaultChannel Default channel for the sound. */
    Sound(std::string name, Mix_Chunk * data, int defaultChannel = -1);

    //! Destructor
    virtual ~Sound();

    /*! Play sound for nTimes times using given location for the sound.
     *  addListener() must have been called with a legal object, otherwise
     *  a zero volume is used. See also setSoundAreaBoundaries().
     * \param sourceX X-coordinate for the sound.
     * \param sourceY Y-coordinate for the sound.
     * \param loops Number of loops to be played.
     * \return channel playing, -1 on failure. */
    int play(int sourceX, int sourceY, int loops = 0);

    //! Get x-coordinate
    int sourceX() const;

    //! Get y-coordinate
    int sourceY() const;

    /*! Play sound
     * \param loops Number of loops to be played.
     * \return channel playing, -1 on failure. */
    int play(int loops = 0);

    //! Stop sound
    void stop();

    //! Stop sound (all sounds in the given channel)
    void stop(int channel);

    //! Set channel.
    void setDefaultChannel(int channel);

    //! Get channel of the sound
    int currentChannel() const;

    /*! Set volume.
     * \param volume Volume: 0.0..1.0 */
    void setVolume(float volume);

    //! Get volume.
    float volume();

    /*! \return True, if the sound is playing. */
    bool isPlaying();

    //! \return The name/handle of the sound object.
    const std::string & name() const;

    //! Add listener object to scale volume with.
    static void addListener(Sound::SoundListenerIf & listener);

    //! Remove all listeners.
    static void removeListeners();

    //! Set maximum distance that equals zero volume.
    static void setMaximumDistance(int distance);

    //! Stop all currently playing sounds.
    static void stopSounds();

    /*! Enable / disable sound system.
     *  No calls to underlying SDL_mixer are done when disabled. */
    static void enable(bool flag);

    /*! Allocate channels.
     * \return The number of channels allocated */
    static int allocateChannels(int numChannels);

    /*! Update volume. Must be periodically called if dynamic changes in volume
     *  due to moving listeners is wanted. */
    static void updateVolumes();

private:

    //! Hidden assignment
    Sound & operator= (const Sound & r);

    //! Hidden copy constructor
    Sound(const Sound & r);

    static void init();

    //! Called, when a channel finishes.
    static void channelCallback(int channel);

    static void calculateAverageListenerPosition(int & listenerX, int & listenerY);

    //! Sound volume scaler logic
    static float getScaledVolume(int sourceX, int sourceY);

    //! Enable / disable sound system
    static bool m_enabled;

    //! Maximum distance within which the sound is played
    static int m_maxDist;

    //! Name of the sound
    std::string m_name;

    //! Current volume
    float m_volume;

    //! default channel
    int m_defaultChannel;

    //! Current channel
    int m_currentChannel;

    //! X-coordinate of the source.
    int m_sourceX;

    //! Y-coordinate of the source.
    int m_sourceY;

    //! SDL sound data
    Mix_Chunk * m_data;

    bool m_positional;

    //! Vector of sound listeners.
    typedef std::vector<SoundListenerIf *> ListenerVector;
    static ListenerVector m_listeners;

    //! Channel to sound object mapping.
    typedef std::map<int, Sound *> ChannelToSoundMap;
    static ChannelToSoundMap m_channelToSoundMap;

    friend class Device;
};

} // namespace SFX

#endif // SOUND_HPP
