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

#ifndef MCFONT_HH
#define MCFONT_HH

#include "mctypes.hh"
#include "mcsurfacemanager.hh"

/*!
 * Routines to load and manage a bitmap-based "font".
 * Font class shares the same mapping file format with TextureManager.
 * The question mark (?) must be always defined in the mapping file
 * to represent undefined characters.
 *
 * The MCFont object is then passed to an MCText object
 * so as to render given (ASCII) text using the desired font.
 *
 * Example character-to-texture mapping file:
 *
 * <?xml version="1.0"?>
 *
 * <textures version="1.0">
 *   <default_path value="./data/fonts/verasans/"/>
 *   <x_axis_mirror value=1/>
 *   <colorkey r=0 g=0 b=0/>
 *   <texture handle="A" image="A.png"/>
 *   <texture handle="B" image="B.png"/>
 *   <texture handle="C" image="C.png"/>
 *   ...
 *   <texture handle="Z" image="Z.png"/>
 *   <texture handle="?" image="Q_MARK.png"/>
 * </textures>
 */
class MCFont : public MCTextureManager
{
public:

  //! Constructor
  MCFont();

  //! Destructor
  virtual ~MCFont();

  //! Load font from strBaseDataPath using the given mapping file strFile.
  virtual void load(const QString & strFile, const QString & strBaseDataPath) throw (MCException);

  //! Get nominal width of an empty space/letter
  MCUint nominalWidth() const;

  //! Get nominal height of an empty space/letter
  MCUint nominalHeight() const;

  //! Get actual width of an empty space/letter (with sizeDivider applied)
  MCUint width() const;

  //! Get actual height of an empty space/letter (with sizeDivider applied)
  MCUint height() const;

  //! Get size divider
  MCUint sizeDivider() const;

  //! Set size divider 
  void setSizeDivider(MCUint s);

private:

  //! Hidden assignment
  MCFont & operator= (const MCFont & r);

  //! Hidden copy constructor
  MCFont(const MCFont & r);

  //! Nominal width of a letter
  MCUint m_nominalWidth;

  //! Nominal height of letter 
  MCUint m_nominalHeight;

  //! Size divider
  MCUint m_sizeDivider;
};

#endif // MCFONT_HH
