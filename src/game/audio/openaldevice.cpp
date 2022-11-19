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

#include "openaldevice.hpp"
#include "simple_logger.hpp"

OpenALDevice::OpenALDevice()
  : m_device(nullptr)
  , m_context(nullptr)
{
}

void OpenALDevice::initialize()
{
    m_device = alcOpenDevice(alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER));
    if (!m_device)
    {
        throw std::runtime_error("Failed to open default sound device");
    }

    juzzlin::L().info() << "Sound device: " << alcGetString(m_device, ALC_DEVICE_SPECIFIER);

    m_context = alcCreateContext(m_device, nullptr);
    if (!alcMakeContextCurrent(m_context))
    {
        throw std::runtime_error("Failed to create default sound context");
    }
}

void OpenALDevice::shutDown()
{
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_context);
    alcCloseDevice(m_device);
}

OpenALDevice::~OpenALDevice()
{
    shutDown();
}
