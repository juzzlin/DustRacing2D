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

#include "../Core/mccamera.hh"
#include "../Core/mcsurface.hh"
#include "../Core/mcgltexcoord.hh"

#include <GL/gl.h>

#include <map>
#include <vector>

class MCTextureTextImpl
{
    explicit MCTextureTextImpl(const std::string & text);

    void updateTextDimensions();

    void render(MCFloat x, MCFloat y, MCCamera * pCamera,
        MCTextureFont & font, bool shadow = true);

    std::string text;
    MCUint glyphWidth, glyphHeight;
    MCUint textWidth, textHeight;
    MCFloat r, g, b, a;
    MCFloat xOffset, yOffset;

    friend class MCTextureText;
};

MCTextureTextImpl::MCTextureTextImpl(const std::string & text_)
: text(text_)
, glyphWidth(32)
, glyphHeight(32)
, r(1.0)
, g(1.0)
, b(1.0)
, a(1.0)
, xOffset(2.0)
, yOffset(2.0)
{
    updateTextDimensions();
}

MCTextureText::MCTextureText(const std::string & text)
: m_pImpl(new MCTextureTextImpl(text))
{
}

MCTextureText::~MCTextureText()
{
    delete m_pImpl;
}

void MCTextureTextImpl::updateTextDimensions()
{
    textWidth  = text.size() * glyphWidth;
    textHeight = glyphHeight;
    for (MCUint i = 0; i < text.size(); i++)
    {
        if (text.at(i) == '\n')
        {
            textHeight += glyphHeight;
        }
    }
}

void MCTextureText::setText(const std::string & text)
{
    m_pImpl->text = text;
    m_pImpl->updateTextDimensions();
}

const std::string & MCTextureText::text() const
{
    return m_pImpl->text;
}

void MCTextureText::setGlyphSize(MCUint width, MCUint height)
{
    m_pImpl->glyphWidth  = width;
    m_pImpl->glyphHeight = height;
    m_pImpl->updateTextDimensions();
}

MCUint MCTextureText::glyphWidth() const
{
    return m_pImpl->glyphWidth;
}

MCUint MCTextureText::glyphHeight() const
{
    return m_pImpl->glyphHeight;
}

void MCTextureText::setColor(MCFloat r, MCFloat g, MCFloat b, MCFloat a)
{
    m_pImpl->r = r;
    m_pImpl->g = g;
    m_pImpl->b = b;
    m_pImpl->a = a;
}

void MCTextureText::getColor(MCFloat & r, MCFloat & g, MCFloat & b, MCFloat & a) const
{
    r = m_pImpl->r;
    g = m_pImpl->g;
    b = m_pImpl->b;
    a = m_pImpl->a;
}

void MCTextureText::setShadowOffset(MCFloat xOffset, MCFloat yOffset)
{
    m_pImpl->xOffset = xOffset;
    m_pImpl->yOffset = yOffset;
}

MCUint MCTextureText::textWidth() const
{
    return m_pImpl->textWidth;
}

MCUint MCTextureText::textHeight() const
{
    return m_pImpl->textHeight;
}

void MCTextureText::render(MCFloat x, MCFloat y, MCCamera * pCamera,
    MCTextureFont & font, bool shadow)
{
    m_pImpl->render(x, y, pCamera, font, shadow);
}

void MCTextureTextImpl::render(MCFloat x, MCFloat y, MCCamera * pCamera,
    MCTextureFont & font, bool shadow)
{
    const MCUint w2 = glyphWidth / 2;
    const MCUint h2 = glyphHeight / 2;

    font.surface().enableClientState(true);
    font.surface().setColor(r, g, b, a);

    glDisable(GL_DEPTH_TEST);

    // Generate a sorted batch in order to
    // improve VBO performance. The idea is
    // to render glyphs out-of-order so that
    // the same glyphs are grouped together.
    static std::map<int, std::vector<int> > batch;
    batch.clear();
    for (MCUint i = 0; i < text.size(); i++)
    {
        const int glyph = text[i];
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

            MCUint glyphXPos = glyphWidth * iter->second[i];

            if (shadow)
            {
                font.surface().renderShadowScaled(pCamera,
                    MCVector3dF(x + glyphXPos + xOffset, y + yOffset, 0),
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
