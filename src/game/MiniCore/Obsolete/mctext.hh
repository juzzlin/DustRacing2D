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

#ifndef MCTEXT_HH
#define MCTEXT_HH

#include "mcfont.hh"
#include "mctypes.hh"

#include <iostream>

using std::ostream;

#include <QString>

//! Textured text object using Font
class MCText
{
public:

    //! Constructor
    MCText(const QString & text);

    //! Copy constructor
    MCText(const MCText & text);

    //! Default constructor
    MCText();

    //! Destructor
    virtual ~MCText();

    //! Set text
    virtual void setText(const QString & text);

    //! Append
    virtual void append(const QString & text);

    //! Append
    virtual void append(const MCText & text);

    //! Render current text at (x,y) using given font
    virtual void render(const MCFont * rFont, int x, int y);

    //! Print to stream
    virtual void print(ostream & os);

    //! Pad to given length with given char
    virtual void padToLength(UINT length, char c);

    //! Get width of the current text using given font
    virtual UINT width(const MCFont * rFont) const;

    //! Get height of the current text using given font
    virtual UINT height(const MCFont * rFont) const;

    //! Get text as a std QString
    virtual const QString & text() const;

    //! Substitution operator
    MCText & operator=(const MCText & old);

private:

    QString m_text;
};

#endif // MCTEXT_HH
