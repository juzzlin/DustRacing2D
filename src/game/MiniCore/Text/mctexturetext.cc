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

class MCTextureTextImpl
{
    explicit MCTextureTextImpl(const std::string & text);

    void updateTextDimensions();

    void render(MCFloat x, MCFloat y, MCCamera * pCamera,
        MCTextureFont & font, bool newLineIncreasesY = false) const;

    void renderShadow(MCFloat x, MCFloat y, MCCamera * pCamera,
        MCTextureFont & font, bool newLineIncreasesY = false) const;

    void renderWithShadow(MCFloat x, MCFloat y, MCCamera * pCamera,
        MCTextureFont & font, bool newLineIncreasesY = false) const;

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
, r(1.0f)
, g(1.0f)
, b(1.0f)
, a(1.0f)
, xOffset(2.0f)
, yOffset(2.0f)
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
    MCTextureFont & font, bool newLineIncreasesY) const
{
    m_pImpl->renderWithShadow(x, y, pCamera, font, newLineIncreasesY);
}

void MCTextureTextImpl::renderWithShadow(MCFloat x, MCFloat y, MCCamera * pCamera,
    MCTextureFont & font, bool newLineIncreasesY) const
{
    renderShadow(x + xOffset, y + yOffset, pCamera, font, newLineIncreasesY);
    render(x, y, pCamera, font, newLineIncreasesY);
}

void MCTextureText::render(MCFloat x, MCFloat y, MCCamera * pCamera,
    MCTextureFont & font, bool newLineIncreasesY) const
{
    m_pImpl->render(x, y, pCamera, font, newLineIncreasesY);
}

void MCTextureTextImpl::render(MCFloat x, MCFloat y, MCCamera * pCamera,
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
    glColor4f(r, g, b, a);

    glBindTexture(GL_TEXTURE_2D, font.surface().handle());

    glPushMatrix();
    glTranslated(x, y, 0);

    const MCFloat w2 = glyphWidth / 2;
    const MCFloat h2 = glyphHeight / 2;

    MCFloat glyphXPos = 0;
    MCFloat glyphYPos = 0;

    glBegin(GL_QUADS);

    const MCUint textSize = text.size();
    for (MCUint i = 0; i < textSize; i++)
    {
        if (text.at(i) == '\n')
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
        else if (text.at(i) == ' ')
        {
            glyphXPos += glyphWidth;
        }
        else
        {
            MCTextureGlyph & glyph = font.glyph(text.at(i));

            glTexCoord2f(glyph.uv(0).m_u, glyph.uv(0).m_v);
            glVertex3f(glyphXPos - w2, glyphYPos + h2, 0);
            glTexCoord2f(glyph.uv(1).m_u, glyph.uv(1).m_v);
            glVertex3f(glyphXPos + w2, glyphYPos + h2, 0);
            glTexCoord2f(glyph.uv(2).m_u, glyph.uv(2).m_v);
            glVertex3f(glyphXPos + w2, glyphYPos - h2, 0);
            glTexCoord2f(glyph.uv(3).m_u, glyph.uv(3).m_v);
            glVertex3f(glyphXPos - w2, glyphYPos - h2, 0);

            glyphXPos += glyphWidth;
        }
    }

    glEnd();

    glPopMatrix();
    glPopAttrib();
}

void MCTextureText::renderShadow(MCFloat x, MCFloat y, MCCamera * pCamera,
    MCTextureFont & font, bool newLineIncreasesY) const
{
    m_pImpl->renderShadow(x, y, pCamera, font, newLineIncreasesY);
}

void MCTextureTextImpl::renderShadow(MCFloat x, MCFloat y, MCCamera * pCamera,
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

    const MCFloat w2 = glyphWidth / 2;
    const MCFloat h2 = glyphHeight / 2;

    MCFloat glyphXPos = 0;
    MCFloat glyphYPos = 0;

    glBegin(GL_QUADS);

    const MCUint textSize = text.size();
    for (MCUint i = 0; i < textSize; i++)
    {
        if (text.at(i) == '\n')
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
        else if (text.at(i) == ' ')
        {
            glyphXPos += glyphWidth;
        }
        else
        {
            MCTextureGlyph & glyph = font.glyph(text.at(i));

            glTexCoord2f(glyph.uv(0).m_u, glyph.uv(0).m_v);
            glVertex3f(glyphXPos - w2, glyphYPos + h2, 0);
            glTexCoord2f(glyph.uv(1).m_u, glyph.uv(1).m_v);
            glVertex3f(glyphXPos + w2, glyphYPos + h2, 0);
            glTexCoord2f(glyph.uv(2).m_u, glyph.uv(2).m_v);
            glVertex3f(glyphXPos + w2, glyphYPos - h2, 0);
            glTexCoord2f(glyph.uv(3).m_u, glyph.uv(3).m_v);
            glVertex3f(glyphXPos - w2, glyphYPos - h2, 0);

            glyphXPos += glyphWidth;
        }
    }

    glEnd();

    glPopMatrix();
    glPopAttrib();
}
