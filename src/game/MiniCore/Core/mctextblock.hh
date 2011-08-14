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

#ifndef MCTEXTBLOCK_HH
#define MCTEXTBLOCK_HH

#include "mctext.hh"
#include "mctypes.hh"

#include <QString>
#include <QList>

//! A collection/block of Text's.
class MCTextBlock
{
public:

  //! Alignment of text lines inside the block
  enum ALIGNMENT {LEFT_JUSTIFIED=0, CENTERED};
  
  //! Constructor
  MCTextBlock();

  //! Copy constructor
  MCTextBlock(const MCTextBlock & r);

  //! Substitution
  MCTextBlock & operator= (const MCTextBlock & r);
  
  //! Adds a QString to the block
  void addText(QString text);

  //! Adds a Text line to the block
  void addText(MCText text);

  //! Renders the block
  void render(int bottomLeftX, int bottomLeftY, MCTextBlock::ALIGNMENT align, const MCFont * pFont);
  
  //! Get line count
  UINT lineCount() const;

  //! Get width of the block
  UINT width(const MCFont * pFont) const;

  //! Get height of the block
  UINT height(const MCFont * pFont) const;

  //! Makes the line order reversed
  void reverseLines();

private:

  //! The text lines
  QList<MCText> m_text;
};

#endif // MCTEXTBLOCK_HH
