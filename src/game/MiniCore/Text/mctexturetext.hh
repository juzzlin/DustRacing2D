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

#ifndef MCTEXTURETEXT_HH
#define MCTEXTURETEXT_HH

#include "mctypes.hh"
#include "mcmacros.hh"
#include <string>

class MCCamera;
class MCTextureFont;

//! MCTextureText is a renderable text object
//! using a textured monospace font (MCTextureFont).
class MCTextureText
{
public:

    //! Constructor.
    explicit MCTextureText(const std::string & text);

    //! Destructor.
    ~MCTextureText();

    //! Set the text.
    void setText(const std::string & text);

    //! Get the text.
    const std::string & text() const;

    //! Set size of one glyph.
    void setGlyphSize(MCUint width, MCUint height);

    //! Set color.
    void setColor(MCFloat r, MCFloat g, MCFloat b, MCFloat a = 1.0);

    //! Get color.
    void getColor(MCFloat & r, MCFloat & g, MCFloat & b, MCFloat & a) const;

    //! Set shadow offset. Effective if renderWithShadow() is called.
    void setShadowOffset(MCFloat xOffset, MCFloat yOffset);

    //! Get glyph width.
    MCUint glyphWidth() const;

    //! Get glyph height.
    MCUint glyphHeight() const;

    //! Get text width.
    MCUint textWidth() const;

    //! Get text height.
    MCUint textHeight() const;

    //! Render at (x,y) as seen thru the
    //! given camera window (can be nullptr) using the
    //! given font.
    void render(MCFloat x, MCFloat y, MCCamera * pCamera,
        MCTextureFont & font, bool shadow = true);

private:

    DISABLE_ASSI(MCTextureText);
    DISABLE_COPY(MCTextureText);

    void updateTextDimensions();

    std::string m_text;
    MCUint m_glyphWidth, m_glyphHeight;
    MCUint m_textWidth, m_textHeight;
    MCFloat m_r, m_g, m_b, m_a;
    MCFloat m_xOffset, m_yOffset;
};

#endif // MCTEXTUREGLYPH_HH
