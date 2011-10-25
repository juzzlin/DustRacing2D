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

#ifndef MCTEXTUREPARSER_HH
#define MCTEXTUREPARSER_HH

#include "../Core/mcmacros.hh"
#include "../Core/mcvector2d.hh"
#include "mctexturedata.hh"

#include <QtXml>
#include <QList>


//! Parser for the XML-formatted texture file.
class MCTextureParser : public QXmlDefaultHandler
{
public:

    //! Constructor
    MCTextureParser();

    //! Get a reference to the data
    QList<MCTextureData> textureData() const;

    //! \reimp
    virtual bool startDocument();

    //! \reimp
    virtual bool endDocument();

    //! \reimp
    virtual bool startElement(const QString &, const QString &, const QString &, const QXmlAttributes &);

    //! \reimp
    virtual bool endElement(const QString &, const QString &, const QString &);

    //! \reimp
    virtual bool fatalError(const QXmlParseException & exception);

private:

    //! Disable copy constructor
    DISABLE_COPY(MCTextureParser);

    //! Disable assignment
    DISABLE_ASSI(MCTextureParser);

    //! Print element error to stderr
    void elementError(const QString & qName);

    //! Texture data struct vector
    QList<MCTextureData> m_textureData;

    //! True if colorkey set
    bool m_colorKeySet;

    //! Current colorkey, if set
    ColorKey m_colorKey;

    //! X-axis mirroring
    bool m_xAxisMirror;

    //! Default image path
    QString m_strDefaultPath;
};

#endif // MCTEXTUREPARSER_HH
