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

#ifndef MCTEXTUREFONT_HH
#define MCTEXTUREFONT_HH

#include "mctextureglyph.hh"
#include <unordered_map>

//! Textured monospace font.
class MCTextureFont
{
public:

    //! Constructor.
    //! \param textureHandle Handle to the OpenGL texture containing
    //! all the monospace glyphs.
    MCTextureFont(MCUint textureHandle);

    //! Add a mapping from given glyph id to given MCTextureGlyph.
    //! MCTextureGlyph includes e.g. uv-coordinates.
    void addGlyphMapping(int glyphId, MCTextureGlyph textureGlyph);

    //! Return an MCTextureGlyph for the given id. Default containing
    //! the whole texture is returned if not found.
    MCTextureGlyph & glyph(int glyphId);

private:

    typedef std::unordered_map<int, MCTextureGlyph> GlyphHash;
    GlyphHash m_glyphs;
    MCTextureGlyph m_default;
    MCUint m_textureHandle;
};

#endif // MCTEXTUREFONT_HH
