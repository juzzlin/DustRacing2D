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

#include "mcglcolor.hh"
#include "mctypes.hh"
#include "mcmacros.hh"

#include <string>

class MCCamera;
class MCTextureFont;

/*! MCTextureText is a renderable texture text object.
 *  A monospace font is assumed (MCTextureFont). */
class MCTextureText
{
public:

    //! Constructor.
    explicit MCTextureText(const std::wstring & text);

    //! Destructor.
    ~MCTextureText();

    //! Set the text.
    void setText(const std::wstring & text);

    //! Get the text.
    const std::wstring & text() const;

    //! Set size of one glyph.
    void setGlyphSize(int width, int height);

    //! Set color.
    void setColor(const MCGLColor & color);

    //! Get color.
    const MCGLColor & color() const;

    //! Set shadow offset. Effective if renderWithShadow() is called.
    void setShadowOffset(MCFloat xOffset, MCFloat yOffset);

    //! Get glyph width.
    int glyphWidth() const;

    //! Get glyph height.
    int glyphHeight() const;

    //! Get text width.
    int width() const;

    //! Get text height.
    int height() const;

    //! Render at (x,y) as seen thru the
    //! given camera window (can be nullptr) using the
    //! given font.
    void render(MCFloat x, MCFloat y, MCCamera * camera,
        MCTextureFont & font, bool shadow = true);

private:

    DISABLE_ASSI(MCTextureText);
    DISABLE_COPY(MCTextureText);

    void updateTextDimensions();

    std::wstring m_text;
    int          m_glyphWidth;
    int          m_glyphHeight;
    int          m_textWidth;
    int          m_textHeight;
    MCGLColor    m_color;
    MCFloat      m_xOffset;
    MCFloat      m_yOffset;
};

#endif // MCTEXTUREGLYPH_HH
