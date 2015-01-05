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

#include "mctexturefontconfigloader.hh"
#include "mctexturefontdata.hh"
#include "mclogger.hh"

#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <cassert>

MCTextureFontConfigLoader::MCTextureFontConfigLoader()
{
}

void MCTextureFontConfigLoader::setConfigPath(const std::string & filePath)
{
    m_filePath = filePath;
}

bool MCTextureFontConfigLoader::loadFonts()
{
    QDomDocument doc;
    QFile file(m_filePath.c_str());
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    if (!doc.setContent(&file))
    {
        file.close();
        return false;
    }

    file.close();

    QDomElement root = doc.documentElement();
    if (root.nodeName() == "fonts")
    {
        MCTextureFontData * newData = nullptr;
        QDomNode node = root.firstChild();
        while(!node.isNull() && node.nodeName() == "font")
        {
            newData = new MCTextureFontData;
            QDomElement tag = node.toElement();
            if(!tag.isNull())
            {
                newData->name    = tag.attribute("name", "").toStdString();
                newData->surface = tag.attribute("surface", "").toStdString();

                MCLogger().info() << "Loading font '" << newData->name.c_str() << "'..";

                // Read child nodes of font node.
                QDomNode childNode = node.firstChild();
                while(!childNode.isNull())
                {
                    if (childNode.nodeName() == "glyph")
                    {
                        QDomElement tag = childNode.toElement();
                        if(!tag.isNull())
                        {
                            MCTextureFontData::Glyph glyph;
                            glyph.x0   = tag.attribute("x0", "0").toInt();
                            glyph.y0   = tag.attribute("y0", "0").toInt();
                            glyph.x1   = tag.attribute("x1", "0").toInt();
                            glyph.y1   = tag.attribute("y1", "0").toInt();
                            glyph.name = tag.attribute("name", "").toStdWString().at(0);

                            newData->glyphs.push_back(glyph);
                        }
                    }

                    childNode = childNode.nextSibling();
                }
            }

            m_fonts.push_back(newData);

            node = node.nextSibling();
        }
    }

    return true;
}

unsigned int MCTextureFontConfigLoader::fonts() const
{
    return static_cast<unsigned int>(m_fonts.size());
}

MCTextureFontData & MCTextureFontConfigLoader::font(unsigned int index) const
{
    assert(index < static_cast<unsigned int>(m_fonts.size()));
    assert(m_fonts.at(index));
    return *m_fonts.at(index);
}

MCTextureFontConfigLoader::~MCTextureFontConfigLoader()
{
    for (MCTextureFontData * font : m_fonts)
    {
        delete font;
    }
}
