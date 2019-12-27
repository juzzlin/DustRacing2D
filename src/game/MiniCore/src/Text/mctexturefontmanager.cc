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

#include "mctexturefontmanager.hh"
#include "mctexturefont.hh"
#include "mctexturefontconfigloader.hh"
#include "mctexturefontdata.hh"

#include "mcsurface.hh"
#include "mcsurfacemanager.hh"

#include <cassert>
#include <exception>

MCTextureFontManager::MCTextureFontManager(
  const MCSurfaceManager & surfaceManager)
  : m_fontHash()
  , m_surfaceManager(surfaceManager)
{
}

void MCTextureFontManager::load(
  const std::string & fileName)
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
        throw std::runtime_error("Parsing '" + fileName + "' failed!");
    }
}

float clamp(float val)
{
    val = val < 0.0f ? 0.0f : val;
    val = val > 1.0f ? 1.0f : val;
    return val;
}

void MCTextureFontManager::createFontFromData(const MCTextureFontData & data)
{
    // Fetch the corresponding surface.
    const auto surface = m_surfaceManager.surface(data.surface.c_str());

    // Create the new font using the GL texture handle given by the
    // corresponding surface.
    const auto newFont = new MCTextureFont(surface);
    newFont->setDensities(data.xDensity, data.yDensity);

    // Generate glyph structures from the loaded data.
    // Loop thru glyphs.
    for (size_t j = 0; j < data.glyphs.size(); j++)
    {
        // Loop through glyphs in the row.
        const MCTextureFontData::Glyph & glyph = data.glyphs.at(j);

        // Calculate the uv-coordinates for the glyph.
        MCTextureGlyph newGlyph(
          MCTextureGlyph::UV(
            static_cast<float>(glyph.x0) / surface->width(),
            static_cast<float>(glyph.y0) / surface->height()),
          MCTextureGlyph::UV(
            static_cast<float>(glyph.x1) / surface->width(),
            static_cast<float>(glyph.y1) / surface->height()));

        // Add glyph mapping to the font.
        newFont->addGlyphMapping(glyph.name, newGlyph);
    }

    for (auto && i = data.fallback.begin(); i != data.fallback.end(); i++)
    {
        newFont->setGlyphFallback(i->first, i->second);
    }

    // Store the font.
    m_fontHash[data.name] = newFont;
}

MCTextureFont & MCTextureFontManager::font(const std::string & name) const
{
    if (!m_fontHash.count(name))
    {
        throw std::runtime_error("Cannot find font object called '" + name + "'");
    }

    const auto font = m_fontHash.find(name)->second;
    assert(font);
    return *font;
}

MCTextureFontManager::~MCTextureFontManager()
{
    // Delete fonts
    for (auto && iter = m_fontHash.begin(); iter != m_fontHash.end(); iter++)
    {
        delete iter->second;
    }
}
