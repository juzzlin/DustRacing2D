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

#include "mctextblock.hh"
#include "mctypes.hh"

MCTextBlock::MCTextBlock() :
  m_text()
{}

MCTextBlock::MCTextBlock(const MCTextBlock & r) :
  m_text(r.m_text)
{}

MCTextBlock & MCTextBlock::operator= (const MCTextBlock & r)
{
  if (&r != this)
  {
    m_text = r.m_text;
  }
  return *this; 
}

void MCTextBlock::addText(QString text)
{
  m_text << MCText(text);
}

void MCTextBlock::addText(MCText text)
{
  m_text << text;
}

void MCTextBlock::reverseLines()
{
  MCText t;
  UINT j;
  const UINT i2 = m_text.length();
  for (UINT i = 0; i < i2 / 2; i++)
  {
    j = i2 - i - 1;
    t = m_text[i];
    m_text[i] = m_text[j];
    m_text[j] = t;
  }
}

UINT MCTextBlock::width(const MCFont * pFont) const
{
  UINT maxw = 0;
  UINT w    = 0;

  const UINT i2 = m_text.length();
  for (UINT i = 0; i < i2; i++)
  {
    w = m_text[i].width(pFont);
    if (w > maxw || !i)
    {
      maxw = w;
    }
  }

  return maxw;
}

UINT MCTextBlock::height(const MCFont * pFont) const
{
  return pFont->height() * m_text.length();
}

void MCTextBlock::render(int bottomLeftX, int bottomLeftY,
                         MCTextBlock::ALIGNMENT align, const MCFont * pFont)
{
  const UINT i2 = m_text.length();
  
  int x  = bottomLeftX;
  int y  = bottomLeftY;
  int dy = pFont->height();

  switch (align)
  {
  default:
  case MCTextBlock::LEFT_JUSTIFIED:

    for (UINT i = 0; i < i2; i++)
    {
      m_text[i].render(pFont, x, y);
      y += dy;
    }

    break;

  case MCTextBlock::CENTERED:

    for (UINT i = 0; i < i2; i++)
    {
      m_text[i].render(pFont, x - (m_text[i].width(pFont)>>1), y);
      y += dy;
    }

    break;
  }
}

UINT MCTextBlock::lineCount() const
{
  return m_text.length();
}
