// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#ifndef MCTEXTUREFONTMANAGER_HH
#define MCTEXTUREFONTMANAGER_HH

#include <string>
#include <unordered_map>

#include "mcmacros.hh"

class  MCTextureFont;
struct MCTextureFontData;
class  MCSurfaceManager;

/*! A manager class for textured monospace fonts.
 *
 *  Example XML config file for a MCTextureFont containing glyphs
 *  of a monospace font. Surface "monospace" refers to a
 *  MCSurface handle loaded by MCSurfaceManager, containing all the glyphs:
 *
 *  \code
 *
 *  <?xml version="1.0"?>
 *  <!-- Config file for texture mapped fonts. -->
 *  <fonts>
 *      <font name="default" surface="monospace">
 *          <glyph name="A" x0="0" y0="581" x1="34" y1="517"/>
 *          <glyph name="B" x0="34" y0="581" x1="68" y1="517"/>
 *          <glyph name="C" x0="68" y0="581" x1="102" y1="517"/>
 *          <glyph name="D" x0="102" y0="581" x1="136" y1="517"/>
 *          <glyph name="E" x0="136" y0="581" x1="170" y1="517"/>
 *          <glyph name="F" x0="170" y0="581" x1="204" y1="517"/>
 *          <glyph name="G" x0="204" y0="581" x1="238" y1="517"/>
 *          <glyph name="H" x0="238" y0="581" x1="273" y1="517"/>
 *          <glyph name="I" x0="273" y0="581" x1="307" y1="517"/>
 *          <glyph name="J" x0="307" y0="581" x1="341" y1="517"/>
 *          <glyph name="K" x0="341" y0="581" x1="375" y1="517"/>
 *          <glyph name="L" x0="375" y0="581" x1="409" y1="517"/>
 *          <glyph name="M" x0="409" y0="581" x1="443" y1="517"/>
 *          <glyph name="N" x0="443" y0="581" x1="477" y1="517"/>
 *          <glyph name="O" x0="0" y0="494" x1="34" y1="430"/>
 *          <glyph name="P" x0="34" y0="494" x1="68" y1="430"/>
 *          <glyph name="Q" x0="68" y0="494" x1="102" y1="430"/>
 *          <glyph name="R" x0="102" y0="494" x1="136" y1="430"/>
 *          <glyph name="S" x0="136" y0="494" x1="170" y1="430"/>
 *          <glyph name="T" x0="170" y0="494" x1="204" y1="430"/>
 *          <glyph name="U" x0="204" y0="494" x1="238" y1="430"/>
 *          <glyph name="V" x0="238" y0="494" x1="273" y1="430"/>
 *          <glyph name="W" x0="273" y0="494" x1="307" y1="430"/>
 *          <glyph name="X" x0="307" y0="494" x1="341" y1="430"/>
 *          <glyph name="Y" x0="341" y0="494" x1="375" y1="430"/>
 *          <glyph name="Z" x0="375" y0="494" x1="409" y1="430"/>
 *      </font>
 *  </fonts>
 *
 *  \endcode
 *
 *  (x0, y0) is the upper left coordinate of the glyph.
 *  (x1, y1) is the lower right coordinate of the glyph.
 */
class MCTextureFontManager
{
public:

    //! Constructor.
    MCTextureFontManager(const MCSurfaceManager & surfaceManager);

    //! Destructor.
    virtual ~MCTextureFontManager();

    /*! Loads texture config from the given config file.
     *  \param filePath Path to the XML-based input file. */
    virtual void load(const std::string & filePath);

    /*! Returns a font object associated with given name.
     *  MCTextureFontManager will keep the ownership.
     *  \param name Name defined in the fonts XML file.
     *  \return Reference to the corresponding MCTextureFont.
     *  \throws std::runtime_error on failure. */
    MCTextureFont & font(const std::string & name) const;

    /*! Creates a new font object from the given data and adds it
     *  to the font hash. */
    void createFontFromData(const MCTextureFontData & data);

private:

    //! Disable copy constructor
    DISABLE_COPY(MCTextureFontManager);

    //! Disable assignment
    DISABLE_ASSI(MCTextureFontManager);

    //! Map for resulting font objects.
    typedef std::unordered_map<std::string, MCTextureFont *> FontHash;
    FontHash m_fontHash;

    const MCSurfaceManager & m_surfaceManager;
};

#endif // MCTEXTUREFONTMANAGER_HH
