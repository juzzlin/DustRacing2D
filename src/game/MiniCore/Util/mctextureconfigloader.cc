// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
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

#include <QDir>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QDomDocument>
#include <QDomElement>

#include "mctextureconfigloader.hh"
#include "mctexturedata.hh"
#include "../Core/MCLogger"

#include <cassert>

MCTextureConfigLoader::MCTextureConfigLoader()
: m_filePath("")
, m_textures()
{}

void MCTextureConfigLoader::setConfigPath(QString path)
{
    m_filePath = path;
}

bool MCTextureConfigLoader::loadTextures()
{
    QDomDocument doc;
    QFile file(m_filePath);
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
    if (root.nodeName() == "textures")
    {
        QString basePath = root.attribute("basePath", "./");

        MCTextureData * newData = nullptr;
        QDomNode node = root.firstChild();
        while(!node.isNull() && node.nodeName() == "texture")
        {
            newData = new MCTextureData;
            QDomElement tag = node.toElement();
            if(!tag.isNull())
            {
                bool         xAxisMirror = tag.attribute("xAxisMirror", "0").toInt();
                unsigned int width       = tag.attribute("w", "0").toUInt();
                unsigned int height      = tag.attribute("h", "0").toUInt();
                QString      file        = tag.attribute("file", "");
                QString      handle      = tag.attribute("handle", "");

                newData->imagePath   = basePath + QDir::separator() + file;
                newData->handle      = handle;
                newData->xAxisMirror = xAxisMirror;

                if (width)
                {
                    newData->width    = width;
                    newData->widthSet = true;
                }

                if (height)
                {
                    newData->height    = height;
                    newData->heightSet = true;
                }

                // Read child nodes of texture node.
                QDomNode childNode = node.firstChild();
                while(!childNode.isNull())
                {
                    if (childNode.nodeName() == "colorKey")
                    {
                        QDomElement tag = childNode.toElement();
                        if(!tag.isNull())
                        {
                            newData->colorKey.m_r = tag.attribute("r", "0").toUInt();
                            newData->colorKey.m_g = tag.attribute("g", "0").toUInt();
                            newData->colorKey.m_b = tag.attribute("b", "0").toUInt();
                            newData->colorKeySet  = true;
                        }
                    }
                    else if (childNode.nodeName() == "alphaTest")
                    {
                        QDomElement tag = childNode.toElement();
                        if(!tag.isNull())
                        {
                            newData->alphaTest.m_threshold = tag.attribute("threshold", "0").toFloat();
                            newData->alphaTest.m_function  = alphaFunctionStringToEnum(tag.attribute("function", ""));
                            newData->alphaTestSet          = true;
                        }
                    }
                    else if (childNode.nodeName() == "center")
                    {
                        QDomElement tag = childNode.toElement();
                        if(!tag.isNull())
                        {
                            const int x = tag.attribute("x", "0").toInt();
                            const int y = tag.attribute("y", "0").toInt();

                            newData->center    = MCVector2d<int>(x, y);
                            newData->centerSet = true;
                        }
                    }

                    childNode = childNode.nextSibling();
                }
            }

            m_textures << newData;

            node = node.nextSibling();
        }
    }

    return true;
}

GLenum MCTextureConfigLoader::alphaFunctionStringToEnum(QString function) const
{
    if (function == "never")
    {
        return GL_NEVER;
    }
    else if (function == "less")
    {
        return GL_LESS;
    }
    else if (function == "equal")
    {
        return GL_EQUAL;
    }
    else if (function == "lequal")
    {
        return GL_LEQUAL;
    }
    else if (function == "greater")
    {
        return GL_GREATER;
    }
    else if (function == "notequal")
    {
        return GL_NOTEQUAL;
    }
    else if (function == "gequal")
    {
        return GL_GEQUAL;
    }
    else
    {
        return GL_ALWAYS;
    }
}

unsigned int MCTextureConfigLoader::textures() const
{
    return m_textures.size();
}

MCTextureData & MCTextureConfigLoader::texture(unsigned int index) const
{
    assert(index < static_cast<unsigned int>(m_textures.size()));
    assert(m_textures.at(index));
    return *m_textures.at(index);
}

MCTextureConfigLoader::~MCTextureConfigLoader()
{
    for (MCTextureData * texture : m_textures)
    {
        delete texture;
    }

    m_textures.clear();
}
