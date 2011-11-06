// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
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

#include "mctext.hh"
#include "mcsurface.hh"

MCText::MCText(const QString & t) :
    m_text(t)
{}

MCText::MCText(const MCText & r) :
    m_text(r.m_text)
{}

MCText::MCText() :
    m_text("")
{}

void MCText::setText(const QString & t)
{
    m_text = t;
}

void MCText::append(const QString & t)
{
    m_text += t;
}

void MCText::append(const MCText & r)
{
    m_text += r.m_text;
}

void MCText::print(ostream & os)
{
    os << m_text.toStdString();
}

void MCText::render(const MCFont * pFont, int x, int y)
{
    MCSurface * p = nullptr;

    // Get actual height of the font
    MCUint h = pFont->height();

    // Loop through the letters of the QString
    const MCUint i2 = m_text.length();
    for (MCUint i = 0; i < i2; i++)
    {
        try
        {
            QString sub(m_text.at(i));
            if (sub == " ")
            {
                x += pFont->width();
            }
            else
            {
                // Try to get a MCSurface for a letter
                p = pFont->surface(sub);
                if (p)
                {
                    MCUint w = p->width() / pFont->sizeDivider();

                    p->renderShadowScaled(nullptr, x + 2, y + 2, w / 2, h / 2, 0);
                    p->renderScaled(nullptr, x, y, 0, w / 2, h / 2, 0);

                    x += w;
                }
            }
        }
        catch (MCException & e)
        {
            // MCSurface not found. Use "?" (that is guaranteed to be
            // found in Font).
            p = pFont->surface("?");
            if (p)
            {
                MCUint w = p->width() / pFont->sizeDivider();

                p->renderShadowScaled(nullptr, x + 2, y + 2, w / 2, h / 2, 0);
                p->renderScaled(nullptr, x, y, 0, w / 2, h / 2, 0);

                x += w;
            }
        }
    }
}

void MCText::padToLength(MCUint l, char c)
{
    for (MCUint i = m_text.length(); i < l; i++)
    {
        m_text.push_back(c);
    }
}

MCUint MCText::height(const MCFont * pFont) const
{
    return pFont->height();
}

MCUint MCText::width(const MCFont * pFont) const
{
    MCSurface * p = nullptr;

    MCUint w = 0;
    const MCUint i2 = m_text.length();
    for (MCUint i = 0; i < i2; i++)
    {
        try
        {
            QString sub(m_text.at(i));
            if (sub == " ")
            {
                w += pFont->width();
            }
            else
            {
                // Try to get a MCSurface for a letter
                p = pFont->surface(sub);
                if (p)
                {
                    w += p->width() / pFont->sizeDivider();
                }
            }
        }
        catch (MCException & e)
        {
            // MCSurface not found. Use "?" (that is guaranteed to be
            // found in Font).
            p = pFont->surface("?");
            if (p)
            {
                w += p->width() / pFont->sizeDivider();
            }
        }
    }

    return w;
}

const QString & MCText::text() const
{
    return m_text;
}

MCText & MCText::operator=(const MCText & old)
{
    if (&old != this)
    {
        m_text = old.m_text;
    }
    return *this;
}

MCText::~MCText()
{}
