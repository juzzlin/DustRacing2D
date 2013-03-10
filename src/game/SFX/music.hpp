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

#ifndef MUSIC_HPP
#define MUSIC_HPP

#include <SDL/SDL_mixer.h>
#include <string>

namespace SFX {

/*! \class Music
 *  \brief Wrapper for SDL's music data.
 */
class Music
{
public:

    /*! Constructor
     * \param name Name / handle of the music track
     * \param pData The actual music data */
    Music(std::string name, Mix_Music * pData);

    //! Destructor
    virtual ~Music();

    /*! Play music for given times using given volume.
     * \param loops Play loops times, -1 for play "forever".
     * \param volume Volume 0.0..1.0.
     * \return channel playing, -1 on failure. */
    int play(int loops, float volume);

    /*! Play music for nTimes using default volume set by Music::setVolume().
     * \param loops Play loops times, -1 for play "forever".
     * \return channel playing, -1 on failure. */
    int play(int loops = 0);

    //! Stop music
    static void stop();

    /*! Set default volume
     * \param volume Volume 0.0..1.0. */
    static void setVolume(float volume);

    //! Get current global volume.
    static float volume();

    /*! Enable / disable Music.
     *  If disabled, no calls to underlying SDL_mixer are performed. */
    static void enable(bool flag);

    /*! Fade out the current music.
     *  \param ms Fade time in ms. */
    static void fadeOut(int ms);

    //! \return The name/handle of the music object.
    const std::string & name() const;

private:

    //! Hidden assignment
    Music & operator= (const Music & r);

    //! Hidden copy constructor
    Music(const Music & r);

    //! Enable / disable music system
    static bool m_enable;

    //! Default volume
    static float m_volume;

    //! Name of the music
    std::string m_name;

    //! SDL music data
    Mix_Music * m_data;
};

} // namespace SFX

#endif // MUSIC_HPP
