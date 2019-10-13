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

#ifndef OPENALDATA_HPP
#define OPENALDATA_HPP

#include <AL/al.h>
#include <Data>

#include <memory>

//! Abstract base class for ogg, wav etc. data loader classes.
class OpenALData : public STFH::Data
{
public:
    //! Constructor.
    OpenALData();

    //! Destructor.
    virtual ~OpenALData();

    //! \return OpenAL buffer handle.
    virtual ALuint buffer() const = 0;
};

using OpenALDataPtr = std::shared_ptr<OpenALData>;

#endif // OPENALDATA_HPP
