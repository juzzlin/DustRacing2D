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

#include <MCGLEW>

#include <map>
#include <vector>

MCTextureText::MCTextureText(const std::wstring & text)
: m_text(text)
, m_glyphWidth(32)
, m_glyphHeight(32)
, m_color(1.0, 1.0, 1.0, 1.0)
, m_xOffset(2.0)
, m_yOffset(-2.0)
{
    updateTextDimensions();
}

MCTextureText::~MCTextureText()
{
}

void MCTextureText::updateTextDimensions()
{
    int maxLength = 0;
    int length    = 0;
    m_textHeight  = m_glyphHeight;
    for (int i = 0; i < static_cast<int>(m_text.size()); i++)
    {
        if (m_text.at(i) == '\n')
        {
            m_textHeight += m_glyphHeight;
            length        = 0;
        }
        else
        {
            length++;
            if (length > maxLength)
            {
                maxLength = length;
            }
        }
    }

    m_textWidth = maxLength * m_glyphWidth;
}

void MCTextureText::setText(const std::wstring & text)
{
    m_text = text;
    updateTextDimensions();
}

const std::wstring & MCTextureText::text() const
{
    return m_text;
}

void MCTextureText::setGlyphSize(int width, int height)
{
    m_glyphWidth  = width;
    m_glyphHeight = height;
    updateTextDimensions();
}

int MCTextureText::glyphWidth() const
{
    return m_glyphWidth;
}

int MCTextureText::glyphHeight() const
{
    return m_glyphHeight;
}

void MCTextureText::setColor(const MCGLColor & color)
{
    m_color = color;
}

const MCGLColor & MCTextureText::color() const
{
    return m_color;
}

void MCTextureText::setShadowOffset(MCFloat xOffset, MCFloat yOffset)
{
    m_xOffset = xOffset;
    m_yOffset = yOffset;
}

int MCTextureText::width() const
{
    return m_textWidth;
}

int MCTextureText::height() const
{
    return m_textHeight;
}

void MCTextureText::render(MCFloat x, MCFloat y, MCCamera * camera,
    MCTextureFont & font, bool shadow)
{
    glDisable(GL_DEPTH_TEST);

    wchar_t prevGlyph = 0;
    wchar_t glyph     = 0;
    int     glyphXPos = x;
    int     glyphYPos = y;

    if (shadow)
    {
        font.surface().bindShadow();

        for (int i = 0; i < static_cast<int>(m_text.size()); i++)
        {
            prevGlyph = glyph;
            glyph     = m_text[i];

            if (glyph == '\n')
            {
                glyphXPos  = x;
                glyphYPos -= m_glyphHeight;
            }
            else if (glyph == ' ')
            {
                glyphXPos += m_glyphWidth;
            }
            else
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

                font.surface().setSize(m_glyphWidth, m_glyphHeight);
                font.surface().renderShadow(camera,
                    MCVector3dF(glyphXPos + m_xOffset, glyphYPos + m_yOffset, 0), 0, false);

                glyphXPos += m_glyphWidth;
            }
        }
    }

    font.surface().bind();
    font.surface().setColor(m_color);

    prevGlyph = 0;
    glyph     = 0;
    glyphXPos = x;
    glyphYPos = y;

    for (int i = 0; i < static_cast<int>(m_text.size()); i++)
    {
        prevGlyph = glyph;
        glyph     = m_text[i];

        if (glyph == '\n')
        {
            glyphXPos  = x;
            glyphYPos -= m_glyphHeight;
        }
        else if (glyph == ' ')
        {
            glyphXPos += m_glyphWidth;
        }
        else
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

            font.surface().setSize(m_glyphWidth, m_glyphHeight);
            font.surface().render(camera,
                MCVector3dF(glyphXPos, glyphYPos, 0), 0, false);

            glyphXPos += m_glyphWidth;
        }
    }
}
