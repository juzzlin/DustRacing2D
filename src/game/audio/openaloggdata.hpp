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

#ifndef OPENALOGGDATA_HPP
#define OPENALOGGDATA_HPP

#include "openaldata.hpp"

//! OpenAL Ogg data loader.
class OpenALOggData : public OpenALData
{
public:

    //! Constructor.
    OpenALOggData(const std::string & path);

    //! Destructor.
    virtual ~OpenALOggData();

    //! \reimp
    virtual void load(const std::string & path) override;

    //! \reimp
    virtual ALuint buffer() const override;

private:

    ALsizei m_freq;
    ALenum  m_format;
    ALuint  m_buffer;
};

#endif // OPENALOGGDATA_HPP
