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

#include "mctexturetext.hh"
#include "mctexturefont.hh"
#include "mctextureglyph.hh"
#include "mccamera.hh"
#include "mcsurface.hh"
#include "mcgltexcoord.hh"

#include <GL/gl.h>

#include <map>
#include <vector>

MCTextureText::MCTextureText(const std::string & text)
: m_text(text)
, m_glyphWidth(32)
, m_glyphHeight(32)
, m_r(1.0)
, m_g(1.0)
, m_b(1.0)
, m_a(1.0)
, m_xOffset(2.0)
, m_yOffset(2.0)
{
    updateTextDimensions();
}

MCTextureText::~MCTextureText()
{
}

void MCTextureText::updateTextDimensions()
{
    m_textWidth  = m_text.size() * m_glyphWidth;
    m_textHeight = m_glyphHeight;
    for (MCUint i = 0; i < m_text.size(); i++)
    {
        if (m_text.at(i) == '\n')
        {
            m_textHeight += m_glyphHeight;
        }
    }
}

void MCTextureText::setText(const std::string & text)
{
    m_text = text;
    updateTextDimensions();
}

const std::string & MCTextureText::text() const
{
    return m_text;
}

void MCTextureText::setGlyphSize(MCUint width, MCUint height)
{
    m_glyphWidth  = width;
    m_glyphHeight = height;
    updateTextDimensions();
}

MCUint MCTextureText::glyphWidth() const
{
    return m_glyphWidth;
}

MCUint MCTextureText::glyphHeight() const
{
    return m_glyphHeight;
}

void MCTextureText::setColor(MCFloat r, MCFloat g, MCFloat b, MCFloat a)
{
    m_r = r;
    m_g = g;
    m_b = b;
    m_a = a;
}

void MCTextureText::getColor(MCFloat & r, MCFloat & g, MCFloat & b, MCFloat & a) const
{
    r = m_r;
    g = m_g;
    b = m_b;
    a = m_a;
}

void MCTextureText::setShadowOffset(MCFloat xOffset, MCFloat yOffset)
{
    m_xOffset = xOffset;
    m_yOffset = yOffset;
}

MCUint MCTextureText::textWidth() const
{
    return m_textWidth;
}

MCUint MCTextureText::textHeight() const
{
    return m_textHeight;
}

void MCTextureText::render(MCFloat x, MCFloat y, MCCamera * pCamera,
    MCTextureFont & font, bool shadow)
{
    const MCUint w2 = m_glyphWidth  / 2;
    const MCUint h2 = m_glyphHeight / 2;

    font.surface().enableClientState(true);
    font.surface().setColor(m_r, m_g, m_b, m_a);

    glDisable(GL_DEPTH_TEST);

    // Generate a sorted batch in order to
    // improve VBO performance. The idea is
    // to render glyphs out-of-order so that
    // the same glyphs are grouped together.
    static std::map<int, std::vector<int> > batch;
    batch.clear();
    for (MCUint i = 0; i < m_text.size(); i++)
    {
        const int glyph = m_text[i];
        if (glyph != '\n' && glyph != ' ')
        {
            batch[glyph].push_back(i);
        }
    }

    int prevGlyph = 0;
    int glyph     = 0;

    auto iter = batch.begin();
    while (iter != batch.end())
    {
        prevGlyph = glyph;
        glyph     = iter->first;
        for (MCUint i = 0; i < iter->second.size(); i++)
        {
            if (glyph != prevGlyph)
            {
                MCTextureGlyph & texGlyph = font.glyph(glyph);
                const MCGLTexCoord uv[4] =
                {
                    {texGlyph.uv(3).m_u, texGlyph.uv(3).m_v},
                    {texGlyph.uv(0).m_u, texGlyph.uv(0).m_v},
                    {texGlyph.uv(1).m_u, texGlyph.uv(1).m_v},
                    {texGlyph.uv(2).m_u, texGlyph.uv(2).m_v}
                };

                font.surface().setTexCoords(uv);
            }

            MCUint glyphXPos = m_glyphWidth * iter->second[i];

            if (shadow)
            {
                font.surface().renderShadowScaled(pCamera,
                    MCVector3dF(x + glyphXPos + m_xOffset, y + m_yOffset, 0),
                    w2, h2, 0, false);
            }

            font.surface().renderScaled(pCamera,
                MCVector3dF(x + glyphXPos, y, 0),
                w2, h2, 0, false);
        }

        iter++;
    }

    font.surface().enableClientState(false);
}
