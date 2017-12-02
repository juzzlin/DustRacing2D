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

#ifndef OPENALDEVICE_HPP
#define OPENALDEVICE_HPP

#include <Device>

#include <AL/al.h>
#include <AL/alc.h>

//! OpenAL sound device wrapper class.
class OpenALDevice : public STFH::Device
{
public:

    //! Constructor.
    OpenALDevice();

    //! \reimp
    virtual void initialize() override;

    //! \reimp
    virtual void shutDown() override;

    //! Destructor.
    virtual ~OpenALDevice();

private:

    ALCdevice  * m_device;
    ALCcontext * m_context;
};

#endif // OPENALDEVICE_HPP
