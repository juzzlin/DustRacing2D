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

#include "mcmusic.hh"

bool MCMusic::m_enable = true;
int  MCMusic::m_volume = MIX_MAX_VOLUME;

MCMusic::MCMusic(QString strName, Mix_Music * data):
  m_strName(strName),
  m_data(data)
{}

int MCMusic::play(int loops, int newVolume) throw (MCException)
{
  if (MCMusic::m_enable)
  {
    Mix_VolumeMusic(newVolume);
    return Mix_PlayMusic(m_data, loops);
  }
  return -1;
}

int MCMusic::play(int loops) throw (MCException)
{
  if (MCMusic::m_enable)
  {
    Mix_VolumeMusic(MCMusic::m_volume);
    return Mix_PlayMusic(m_data, loops);
  }
  return -1;
}

void MCMusic::stop()
{
  if (MCMusic::m_enable)
  {
    Mix_HaltMusic();
  }
}

void MCMusic::enable(bool flag)
{
  MCMusic::m_enable = flag;
}

void MCMusic::setVolume(int newVolume)
{
  if (MCMusic::m_enable)
  {
    MCMusic::m_volume = newVolume;
    Mix_VolumeMusic(newVolume);
  }
}

int MCMusic::volume()
{
  return MCMusic::m_volume;
}

MCMusic::~MCMusic()
{}
