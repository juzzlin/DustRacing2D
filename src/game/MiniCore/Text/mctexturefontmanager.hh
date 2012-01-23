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

#include "../Core/mcexception.hh"
#include "../Core/mcmacros.hh"

class MCTextureFont;
class MCTextureFontData;
class MCTextureManager;

/*! Manager for textured monospace font.
 *
 *  Example config file for a MCTextureFont containing glyphs
 *  of a monospace font. Surface "monospace" refers to a
 *  MCSurface handle loaded by MCTextureManager:
 *
 *  <?xml version="1.0"?>
 *  <!-- Config file for texture mapped fonts. -->
 *  <fonts>
 *      <font name="default" surface="monospace" maxGlyphsPerRow="15">
 *          <row y="0"   h="80" glyphs="abcdefghijklmn"/>
 *          <row y="86"  h="80" glyphs="opqrstuvwxyzåäö"/>
 *          <row y="168" h="80" glyphs="ABCDEFGHIJKLMN"/>
 *          <row y="258" h="80" glyphs="OPQRSTUVWXYZÅÄÖ"/>
 *          <row y="342" h="80" glyphs="0123456789"/>
 *          <row y="424" h="80" glyphs=" !&quot;#$%&amp;&apos;()*+,-."/>
 *          <row y="508" h="80" glyphs="/:;&lt;=&gt;?"/>
 *      </font>
 *  </fonts>
 */
class MCTextureFontManager
{
public:

    //! Constructor.
    MCTextureFontManager(const MCTextureManager & textureManager);

    //! Destructor.
    virtual ~MCTextureFontManager();

    //! Return the singleton.
    static MCTextureFontManager & instance();

    //! Loads texture config from the given config file.
    //! \param filePath Path to the XML-based input file.
    virtual void load(const std::string & filePath) throw (MCException);

    //! Returns a font object associated with given name.
    //! MCTextureFontManager will keep the ownership.
    //! \param name Name defined in the fonts XML file.
    //! \return Reference to the corresponding MCTextureFont.
    //! \throws MCException on failure.
    MCTextureFont & font(const std::string & name) const throw (MCException);

private:

    //! Disable copy constructor
    DISABLE_COPY(MCTextureFontManager);

    //! Disable assignment
    DISABLE_ASSI(MCTextureFontManager);

    //! Creates a new font object from the given data and adds it
    //! to the font hash.
    void createFontFromData(const MCTextureFontData & data);

    //! Map for resulting font objects.
    typedef std::unordered_map<std::string, MCTextureFont *> FontHash;
    FontHash m_fontHash;

    const MCTextureManager & m_textureManager;
    static MCTextureFontManager * m_pInstance;
};

#endif // MCTEXTUREFONTMANAGER_HH
