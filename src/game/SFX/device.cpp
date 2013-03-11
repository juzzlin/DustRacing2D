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

#include "device.hpp"
#include "sound.hpp"

#include <SDL/SDL_mixer.h>
#include <cassert>

namespace SFX {

Device * Device::m_instance = nullptr;

Device::Device()
: m_inited(false)
{
    assert(!Device::m_instance);
    Device::m_instance = this;
}

Device & Device::instance()
{
    assert(!Device::m_instance);
    return *Device::m_instance;
}

bool Device::init(int sampleRate, int numChannels)
{
    if (Mix_OpenAudio(sampleRate, MIX_DEFAULT_FORMAT, numChannels, 1024) == -1)
    {
        return false;
    }

    Sound::init();

    m_inited = true;

    return true;
}

bool Device::inited() const
{
    return m_inited;
}

Device::~Device()
{
    Mix_CloseAudio();
}

} // namespace SFX
