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

#ifndef OPENALWAVDATA_HPP
#define OPENALWAVDATA_HPP

#include <AL/al.h>
#include <Data>

//! OpenAL WAV data loader using ALUT.
class OpenALWavData : public STFH::Data
{
public:

    //! Constructor.
    OpenALWavData();

    //! Destructor.
    virtual ~OpenALWavData();

    //! \reimp
    virtual void load(const std::string & path) throw ();

    //! \return the data chunk.
    virtual ALvoid * data();

    //! \return size.
    virtual ALsizei size();

    //! \return frequency.
    virtual ALsizei freq();

    //! \return format.
    virtual ALenum format();

    //! \return OpenAL buffer handle.
    virtual ALuint buffer();

private:

    ALvoid *  m_data;
    ALsizei   m_size, m_freq;
    ALenum    m_format;
    ALuint    m_buffer;
    ALboolean m_loop;
};

#endif // OPENALWAVDATA_HPP
