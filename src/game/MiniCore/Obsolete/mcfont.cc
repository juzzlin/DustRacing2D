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

#include "mcfont.hh"
#include "mcsurface.hh"
#include <iostream>

using std::cerr;
using std::endl;

MCFont::MCFont() :
  m_nominalWidth(0),
  m_nominalHeight(0),
  m_sizeDivider(1)
{}

void MCFont::load(const QString & strFile, const QString & strBaseDataPath) throw (MCException)
{
  // Load textures using method in base class
  MCTextureManager::load(strFile, strBaseDataPath);

  // Check that "?" is present
  try
  {
    MCSurface * p = surface("?");

    // Note!: getTexture already has thrown as error if not found
    if (p)
    {
      m_nominalWidth  = p->width();
      m_nominalHeight = p->height();
    }
  }
  catch (MCException & e)
  {
    cerr << "ERROR!!: Incorrect font " << strFile.toStdString() << "!" << endl;
    throw e;
  }
}

MCUint MCFont::nominalWidth() const
{
  return m_nominalWidth;
}

MCUint MCFont::nominalHeight() const
{
  return m_nominalHeight;
}

MCUint MCFont::width() const
{
  return m_nominalWidth / m_sizeDivider;
}

MCUint MCFont::height() const
{
  return m_nominalHeight / m_sizeDivider;
}

MCUint MCFont::sizeDivider() const
{
  return m_sizeDivider;
}

void MCFont::setSizeDivider(MCUint s)
{
  if (s > 0)
  {
    m_sizeDivider = s;
  }
  else
  {
    m_sizeDivider = 1;
  }
}

MCFont::~MCFont()
{}

