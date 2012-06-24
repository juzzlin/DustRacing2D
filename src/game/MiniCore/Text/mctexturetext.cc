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

class MCTextureTextImpl
{
    explicit MCTextureTextImpl(const std::string & text);

    void updateTextDimensions();

    void render(MCFloat x, MCFloat y, MCCamera * pCamera,
        MCTextureFont & font, bool newLineIncreasesY = false);

    void renderShadow(MCFloat x, MCFloat y, MCCamera * pCamera,
        MCTextureFont & font, bool newLineIncreasesY = false);

    void renderWithShadow(MCFloat x, MCFloat y, MCCamera * pCamera,
        MCTextureFont & font, bool newLineIncreasesY = false);

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

void MCTextureText::renderWithShadow(MCFloat x, MCFloat y, MCCamera * pCamera,
    MCTextureFont & font, bool newLineIncreasesY)
{
    m_pImpl->renderWithShadow(x, y, pCamera, font, newLineIncreasesY);
}

void MCTextureTextImpl::renderWithShadow(MCFloat x, MCFloat y, MCCamera * pCamera,
    MCTextureFont & font, bool newLineIncreasesY)
{
    renderShadow(x + xOffset, y + yOffset, pCamera, font, newLineIncreasesY);
    render(x, y, pCamera, font, newLineIncreasesY);
}

void MCTextureText::render(MCFloat x, MCFloat y, MCCamera * pCamera,
    MCTextureFont & font, bool newLineIncreasesY)
{
    m_pImpl->render(x, y, pCamera, font, newLineIncreasesY);
}

void MCTextureTextImpl::render(MCFloat x, MCFloat y, MCCamera * pCamera,
    MCTextureFont & font, bool newLineIncreasesY)
{
    const MCFloat w2 = glyphWidth / 2;
    const MCFloat h2 = glyphHeight / 2;

    MCFloat glyphXPos = 0;
    MCFloat glyphYPos = 0;

    font.surface().enableClientState(true);
    font.surface().setColor(r, g, b, a);

    glDisable(GL_DEPTH_TEST);

    int prevGlyph = 0;
    int glyph     = 0;
    const MCUint textSize = text.size();
    for (MCUint i = 0; i < textSize; i++)
    {
        prevGlyph = glyph;
        glyph     = text.at(i);
        if (glyph == '\n')
        {
            if (newLineIncreasesY)
            {
                glyphYPos += glyphHeight;
            }
            else
            {
                glyphYPos -= glyphHeight;
            }

            glyphXPos  = 0;
        }
        else if (glyph == ' ')
        {
            glyphXPos += glyphWidth;
        }
        else
        {
            if (glyph != prevGlyph)
            {
                MCTextureGlyph & glyph = font.glyph(text.at(i));
                const MCGLTexCoord uv[4] =
                {
                    {glyph.uv(3).m_u, glyph.uv(3).m_v},
                    {glyph.uv(0).m_u, glyph.uv(0).m_v},
                    {glyph.uv(1).m_u, glyph.uv(1).m_v},
                    {glyph.uv(2).m_u, glyph.uv(2).m_v}
                };

                font.surface().setTexCoords(uv);
            }

            font.surface().renderScaled(
                pCamera,
                MCVector3dF(x + glyphXPos, y + glyphYPos, 0),
                w2, h2, 0, false);

            glyphXPos += glyphWidth;
        }
    }

    font.surface().enableClientState(false);
}

void MCTextureText::renderShadow(MCFloat x, MCFloat y, MCCamera * pCamera,
    MCTextureFont & font, bool newLineIncreasesY)
{
    m_pImpl->renderShadow(x, y, pCamera, font, newLineIncreasesY);
}

void MCTextureTextImpl::renderShadow(MCFloat x, MCFloat y, MCCamera * pCamera,
    MCTextureFont & font, bool newLineIncreasesY)
{
    const MCFloat w2 = glyphWidth / 2;
    const MCFloat h2 = glyphHeight / 2;

    MCFloat glyphXPos = 0;
    MCFloat glyphYPos = 0;

    font.surface().enableClientState(true);
    glDisable(GL_DEPTH_TEST);

    int prevGlyph = 0;
    int glyph     = 0;
    const MCUint textSize = text.size();
    for (MCUint i = 0; i < textSize; i++)
    {
        prevGlyph = glyph;
        glyph     = text.at(i);
        if (glyph == '\n')
        {
            if (newLineIncreasesY)
            {
                glyphYPos += glyphHeight;
            }
            else
            {
                glyphYPos -= glyphHeight;
            }

            glyphXPos  = 0;
        }
        else if (glyph == ' ')
        {
            glyphXPos += glyphWidth;
        }
        else
        {
            if (glyph != prevGlyph)
            {
                MCTextureGlyph & glyph = font.glyph(text.at(i));
                const MCGLTexCoord uv[4] =
                {
                    {glyph.uv(3).m_u, glyph.uv(3).m_v},
                    {glyph.uv(0).m_u, glyph.uv(0).m_v},
                    {glyph.uv(1).m_u, glyph.uv(1).m_v},
                    {glyph.uv(2).m_u, glyph.uv(2).m_v}
                };

                font.surface().setTexCoords(uv);
            }

            font.surface().renderShadowScaled(
                pCamera,
                MCVector3dF(x + glyphXPos, y + glyphYPos, 0),
                w2, h2, 0, false);

            glyphXPos += glyphWidth;
        }
    }

    font.surface().enableClientState(false);
}
