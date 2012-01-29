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

#include <GL/gl.h>

MCTextureText::MCTextureText(const std::string & text)
  : m_text(text)
  , m_glyphWidth(32)
  , m_glyphHeight(32)
  , m_r(1.0f)
  , m_g(1.0f)
  , m_b(1.0f)
  , m_a(1.0f)
{
    updateTextDimensions();
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

MCUint MCTextureText::textWidth() const
{
    return m_textWidth;
}

MCUint MCTextureText::textHeight() const
{
    return m_textHeight;
}

void MCTextureText::render(MCFloat x, MCFloat y, MCCamera * pCamera,
    MCTextureFont & font, bool newLineIncreasesY) const
{
    if (pCamera)
    {
        pCamera->mapToCamera(x, y);
    }

    glPushAttrib(GL_ENABLE_BIT);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glNormal3i(0, 0, 1);

    font.surface().doAlphaTest();
    font.surface().doAlphaBlend();

    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor4f(m_r, m_g, m_b, m_a);

    glBindTexture(GL_TEXTURE_2D, font.surface().handle());

    glPushMatrix();
    glTranslated(x, y, 0);

    const MCFloat w2 = m_glyphWidth / 2;
    const MCFloat h2 = m_glyphHeight / 2;

    MCFloat glyphXPos = 0;
    MCFloat glyphYPos = 0;

    glBegin(GL_QUADS);

    const MCUint textSize = m_text.size();
    for (MCUint i = 0; i < textSize; i++)
    {
        if (m_text.at(i) == '\n')
        {
            if (newLineIncreasesY)
            {
                glyphYPos += m_glyphHeight;
            }
            else
            {
                glyphYPos -= m_glyphHeight;
            }

            glyphXPos  = 0;
        }
        else if (m_text.at(i) == ' ')
        {
            glyphXPos += m_glyphWidth;
        }
        else
        {
            MCTextureGlyph & glyph = font.glyph(m_text.at(i));

            glTexCoord2f(glyph.uv(0).m_u, glyph.uv(0).m_v);
            glVertex3f(glyphXPos - w2, glyphYPos + h2, 0);
            glTexCoord2f(glyph.uv(1).m_u, glyph.uv(1).m_v);
            glVertex3f(glyphXPos + w2, glyphYPos + h2, 0);
            glTexCoord2f(glyph.uv(2).m_u, glyph.uv(2).m_v);
            glVertex3f(glyphXPos + w2, glyphYPos - h2, 0);
            glTexCoord2f(glyph.uv(3).m_u, glyph.uv(3).m_v);
            glVertex3f(glyphXPos - w2, glyphYPos - h2, 0);

            glyphXPos += m_glyphWidth;
        }
    }

    glEnd();

    glPopMatrix();
    glPopAttrib();
}

void MCTextureText::renderShadow(MCFloat x, MCFloat y, MCCamera * pCamera,
    MCTextureFont & font, bool newLineIncreasesY) const
{
    // TODO: Refactor and optimize this..

    if (pCamera)
    {
        pCamera->mapToCamera(x, y);
    }

    glPushAttrib(GL_ENABLE_BIT);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glNormal3i(0, 0, 1);

    font.surface().doAlphaTest();
    glEnable(GL_BLEND);
    glBlendFunc(GL_ZERO, GL_ZERO);

    glNormal3f(0.0f, 0.0f, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);

    glBindTexture(GL_TEXTURE_2D, font.surface().handle());

    glPushMatrix();
    glTranslated(x, y, 0);

    const MCFloat w2 = m_glyphWidth / 2;
    const MCFloat h2 = m_glyphHeight / 2;

    MCFloat glyphXPos = 0;
    MCFloat glyphYPos = 0;

    glBegin(GL_QUADS);

    const MCUint textSize = m_text.size();
    for (MCUint i = 0; i < textSize; i++)
    {
        if (m_text.at(i) == '\n')
        {
            if (newLineIncreasesY)
            {
                glyphYPos += m_glyphHeight;
            }
            else
            {
                glyphYPos -= m_glyphHeight;
            }

            glyphXPos  = 0;
        }
        else if (m_text.at(i) == ' ')
        {
            glyphXPos += m_glyphWidth;
        }
        else
        {
            MCTextureGlyph & glyph = font.glyph(m_text.at(i));

            glTexCoord2f(glyph.uv(0).m_u, glyph.uv(0).m_v);
            glVertex3f(glyphXPos - w2, glyphYPos + h2, 0);
            glTexCoord2f(glyph.uv(1).m_u, glyph.uv(1).m_v);
            glVertex3f(glyphXPos + w2, glyphYPos + h2, 0);
            glTexCoord2f(glyph.uv(2).m_u, glyph.uv(2).m_v);
            glVertex3f(glyphXPos + w2, glyphYPos - h2, 0);
            glTexCoord2f(glyph.uv(3).m_u, glyph.uv(3).m_v);
            glVertex3f(glyphXPos - w2, glyphYPos - h2, 0);

            glyphXPos += m_glyphWidth;
        }
    }

    glEnd();

    glPopMatrix();
    glPopAttrib();
}
