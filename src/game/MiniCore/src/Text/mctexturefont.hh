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
#include "mcglshaderprogram.hh"

#include <unordered_map>
#include <vector>

class MCSurface;

//! Textured monospace font.
class MCTextureFont
{
public:

    /*! Constructor.
     * \param surface Reference to the surface object containing
     * all the monospace glyphs. */
    MCTextureFont(MCSurface & surface);

    /*! Add a mapping from given glyp to given MCTextureGlyph.
     *  MCTextureGlyph includes e.g. uv-coordinates. */
    void addGlyphMapping(wchar_t glyph, MCTextureGlyph textureGlyph);

    /*! Return an MCTextureGlyph for the given glyph. Default containing
     *  the whole texture is returned if not found. */
    MCTextureGlyph & glyph(wchar_t glyph);

    /*! Set a fallback glyph for the given glyph. Must be call AFTER
     *  all other glyphs are added. */
    void setGlyphFallback(wchar_t glyph, wchar_t fallback);

    //! Return the associated surface.
    MCSurface & surface() const;

    //! Set the shader program to be used.
    void setShaderProgram(MCGLShaderProgramPtr program);

    //! Set the shader program to be used for 2d shadows.
    void setShadowShaderProgram(MCGLShaderProgramPtr program);

private:

    MCTextureGlyph m_default;

    typedef std::unordered_map<wchar_t, MCTextureGlyph> GlyphHash;
    GlyphHash m_glyphs;

    typedef std::vector<MCTextureGlyph> GlyphLookUp;
    GlyphLookUp m_glyphLookUp;

    MCSurface & m_surface;
};

#endif // MCTEXTUREFONT_HH
