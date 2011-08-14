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

#include "mcsound.hh"
#include "mcobject.hh"

#include <cmath>

QVector<MCObject *> MCSound::m_referenceObjects;

bool MCSound::m_enable      = true;

int  MCSound::m_soundAreaX1 = 0;
int  MCSound::m_soundAreaY1 = 0;
int  MCSound::m_soundAreaX2 = 0;
int  MCSound::m_soundAreaY2 = 0;
int  MCSound::m_maxDist     = 0;

int  MCSound::m_volume      = MIX_MAX_VOLUME;

namespace
{
  static const int MIN_VOLUME = 5;
}

MCSound::MCSound(QString strName, Mix_Chunk * pData, int newChannel):
  m_strName(strName),
  m_channel(newChannel),
  m_data(pData)
{}

int MCSound::play(int sourceX, int sourceY, int loops) throw (MCException)
{
  if (MCSound::m_enable)
  {
    int scaledVolume = getScaledVolume(sourceX, sourceY);
    if (scaledVolume > MIN_VOLUME)
    {
      Mix_VolumeChunk(m_data, scaledVolume);
      m_channel = Mix_PlayChannel(-1, m_data, loops);
      return m_channel;
    }
  }

  return -1;
}

int MCSound::play(int loops, int newVolume) throw (MCException)
{
  if (MCSound::m_enable)
  {
    Mix_VolumeChunk(m_data, newVolume);
    m_channel = Mix_PlayChannel(-1, m_data, loops);
    return m_channel;
  }

  return -1;
}

int MCSound::play(int loops) throw (MCException)
{
  if (MCSound::m_enable)
  {
    Mix_VolumeChunk(m_data, MCSound::m_volume);
    m_channel = Mix_PlayChannel(-1, m_data, loops);
    return m_channel;
  }

  return -1;
}

void MCSound::stop()
{
  if (MCSound::m_enable)
  {
    if (m_channel != -1)
    {
      Mix_HaltChannel(m_channel);

      m_channel = -1;
    }
  }
}

void MCSound::stop(int newChannel)
{
  if (MCSound::m_enable)
  {
    if (newChannel != -1)
    {
      Mix_HaltChannel(newChannel);
    }
  }
}

void MCSound::stopSounds()
{
  if (MCSound::m_enable)
  {
    Mix_HaltChannel(-1);
  }
}

int MCSound::channel() const
{
  return m_channel;
}

int MCSound::getScaledVolume(int sourceX, int sourceY)
{
  MCObject * p = NULL;
  int refX, refY, d, dX, dY, result;
  result = 0;

  UINT i2 = MCSound::m_referenceObjects.size();
  for (UINT i = 0; i < i2; i++)
  {
    p    = MCSound::m_referenceObjects[i];
    refX = p->getX();
    refY = p->getY();
    dX   = std::abs(refX - sourceX);
    dY   = std::abs(refY - sourceY);
    d    = std::max(dX, dY) + std::min(dX, dY) / 2;
    d    = (MCSound::m_maxDist) - d;
    d    = d < 0 ? 0 : d;
    d    = d * MCSound::m_volume / MCSound::m_maxDist;
    
    if (!i || d > result)
    {
      result = d;
    }
  }

  return result;
}

void MCSound::addReferenceObject(MCObject * pObject)
{
  MCSound::m_referenceObjects << pObject;
}

void MCSound::setSoundAreaBoundaries(int x1, int y1, int x2, int y2)
{
  int dX, dY;

  MCSound::m_soundAreaX1 = x1;
  MCSound::m_soundAreaY1 = y1;
  MCSound::m_soundAreaX2 = x2;
  MCSound::m_soundAreaY2 = y2;

  dX = std::abs(MCSound::m_soundAreaX2 - MCSound::m_soundAreaX1);
  dY = std::abs(MCSound::m_soundAreaY2 - MCSound::m_soundAreaY1);

  MCSound::m_maxDist = std::max(dX, dY) / 2;
}

void MCSound::enable(bool bFlag)
{
  MCSound::m_enable = bFlag;
}

void MCSound::setVolume(int newVolume)
{
  MCSound::m_volume = newVolume;
}

int MCSound::volume()
{
  return MCSound::m_volume;
}

void MCSound::setChannels(int numChannels)
{
  if (MCSound::m_enable)
  {
    Mix_AllocateChannels(numChannels);
  }
}

MCSound::~MCSound()
{}
