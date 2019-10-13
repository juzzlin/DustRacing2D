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
    void setGlyphSize(float width, float height);

    //! Set color.
    void setColor(const MCGLColor & color);

    //! Get color.
    const MCGLColor & color() const;

    //! Set shadow offset. Effective if renderWithShadow() is called.
    void setShadowOffset(float xOffset, float yOffset);

    //! Get glyph width.
    float glyphWidth() const;

    //! Get glyph height.
    float glyphHeight() const;

    //! Get text width.
    float width(MCTextureFont & font) const;

    //! Get text height.
    float height(MCTextureFont & font) const;

    //! Render at (x,y) as seen through the given camera window (can be nullptr) using the given font.
    void render(float x, float y, MCCamera * camera, MCTextureFont & font, bool shadow = true);

private:
    void updateTextDimensions();

    std::wstring m_text;

    float m_glyphWidth;

    float m_glyphHeight;

    float m_textWidth;

    float m_textHeight;

    MCGLColor m_color;

    float m_xOffset;

    float m_yOffset;
};

#endif // MCTEXTUREGLYPH_HH
