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

#include "mctexturefontconfigloader.hh"
#include "mctexturefont.hh"
#include "mctexturefontdata.hh"
#include "mctexturefontmanager.hh"
#include "mctypes.hh"
#include "mcsurface.hh"
#include "mcsurfacemanager.hh"

#include <cassert>

MCTextureFontManager * MCTextureFontManager::m_pInstance = nullptr;

MCTextureFontManager::MCTextureFontManager(
    const MCSurfaceManager & surfaceManager)
  : m_fontHash()
  , m_surfaceManager(surfaceManager)
{
    assert(!MCTextureFontManager::m_pInstance);
    MCTextureFontManager::m_pInstance = this;
}

MCTextureFontManager & MCTextureFontManager::instance()
{
    assert(MCTextureFontManager::m_pInstance);
    return *MCTextureFontManager::m_pInstance;
}

void MCTextureFontManager::load(
    const std::string & fileName) throw (MCException)
{
    MCTextureFontConfigLoader loader;
    loader.setConfigPath(fileName);

    // Parse the texture config file
    if (loader.loadFonts())
    {
        const int numFonts = loader.fonts();
        for (int i = 0; i < numFonts; i++)
        {
            const MCTextureFontData & data = loader.font(i);
            createFontFromData(data);
        }
    }
    else
    {
        // Throw an exception
        throw MCException("Parsing '" + fileName + "' failed!");
    }
}

MCFloat clamp(MCFloat val)
{
    val = val < 0.0 ? 0.0 : val;
    val = val > 1.0 ? 1.0 : val;
    return val;
}

void MCTextureFontManager::createFontFromData(const MCTextureFontData & data)
{
    // Fetch the corresponding surface.
    MCSurface & surface = m_surfaceManager.surface(data.surface.c_str());

    // Create the new font using the GL texture handle given by the
    // corresponding surface.
    MCTextureFont * newFont = new MCTextureFont(surface);

    // Generate glyph structures from the loaded data.
    // Loop thru glyph rows.
    for (MCUint j = 0; j < data.rows.size(); j++)
    {
        // Loop through glyphs in the row.
        const MCTextureFontData::Row & row       = data.rows.at(j);
        const std::string            & rowGlyphs = row.glyphs;
        for (MCUint i = 0; i < rowGlyphs.length(); i++)
        {
            const GLfloat fi = i;
            const GLfloat fy = row.y;
            const GLfloat fh = row.h;
            const GLfloat fH = surface.height();

            // Calculate the uv-coordinates for the glyph.
            MCTextureGlyph newGlyph(
                rowGlyphs.at(i),
                MCTextureGlyph::UV(
                    clamp(fi / data.maxGlyphsPerRow),       clamp((fH - fy) / fH)),
                MCTextureGlyph::UV(
                    clamp((fi + 1) / data.maxGlyphsPerRow), clamp((fH - fy) / fH)),
                MCTextureGlyph::UV(
                    clamp((fi + 1) / data.maxGlyphsPerRow), clamp((fH - (fy + fh)) / fH)),
                MCTextureGlyph::UV(
                    clamp(fi / data.maxGlyphsPerRow),       clamp((fH - (fy + fh)) / fH)));

            // Add glyph mapping to the font.
            newFont->addGlyphMapping(rowGlyphs.at(i), newGlyph);
        }
    }

    // Store the font.
    m_fontHash[data.name] = newFont;
}

MCTextureFont & MCTextureFontManager::font(
    const std::string & name) const throw (MCException)
{
    // Try to find existing texture for the surface
    if (m_fontHash.find(name) == m_fontHash.end())
    {
        // No:
        throw MCException("Cannot find font object called '" + name + "'");
    }

    MCTextureFont * font = m_fontHash.find(name)->second;
    assert(font);
    return *font;
}

MCTextureFontManager::~MCTextureFontManager()
{
    // Delete fonts
    auto iter(m_fontHash.begin());
    while (iter != m_fontHash.end())
    {
        delete iter->second;
        iter++;
    }
}
