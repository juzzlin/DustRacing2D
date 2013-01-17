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
#include <QDomDocument>
#include <QDomElement>
#include <QFile>

#include "mcsurfaceconfigloader.hh"
#include "mcsurfacedata.hh"
#include "mclogger.hh"

#include <cassert>

MCSurfaceConfigLoader::MCSurfaceConfigLoader()
: m_surfaces()
{}

bool MCSurfaceConfigLoader::load(const std::string & path)
{
    QDomDocument doc;
    QFile file(path.c_str());
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
    if (root.nodeName() == "surfaces")
    {
        const std::string baseImagePath = root.attribute("baseImagePath", "./").toStdString();

        MCSurfaceData * newData = nullptr;
        QDomNode node = root.firstChild();
        while(!node.isNull() && node.nodeName() == "surface")
        {
            newData = new MCSurfaceData;
            QDomElement tag = node.toElement();
            if(!tag.isNull())
            {
                const unsigned int width  = tag.attribute("w", "0").toUInt();
                const unsigned int height = tag.attribute("h", "0").toUInt();
                const std::string  image1 = tag.attribute("image1", "").toStdString();
                const std::string  image2 = tag.attribute("image2", "").toStdString();
                newData->imagePath1       = baseImagePath + QDir::separator().toAscii() + image1;
                newData->handle           = tag.attribute("handle", "").toStdString();
                newData->xAxisMirror      = tag.attribute("xAxisMirror", "0").toInt();
                newData->z0               = tag.attribute("z0", "0").toInt();
                newData->z1               = tag.attribute("z1", "0").toInt();
                newData->z2               = tag.attribute("z2", "0").toInt();
                newData->z3               = tag.attribute("z3", "0").toInt();

                if (!image2.empty())
                {
                    newData->imagePath2 = baseImagePath + QDir::separator().toAscii() + image2;
                }

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

                // Read child nodes of surface node.
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
                    else if (childNode.nodeName() == "alphaBlend")
                    {
                        QDomElement tag = childNode.toElement();
                        if(!tag.isNull())
                        {
                            newData->alphaBlend.m_src =
                                alphaBlendStringToEnum(
                                    tag.attribute("src", "srcAlpha").toStdString());
                            newData->alphaBlend.m_dst =
                                alphaBlendStringToEnum(
                                    tag.attribute(
                                        "dst", "srcAlphaMinusOne").toStdString());
                            newData->alphaBlendSet = true;
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
                    else if (childNode.nodeName() == "filter")
                    {
                        QDomElement tag = childNode.toElement();
                        if(!tag.isNull())
                        {
                            std::string min = tag.attribute("min", "").toStdString();
                            std::string mag = tag.attribute("mag", "").toStdString();

                            if (min == "linear")
                            {
                                newData->minFilterSet = true;
                                newData->minFilter    = GL_LINEAR;
                            }
                            else if (min == "nearest")
                            {
                                newData->minFilterSet = true;
                                newData->minFilter    = GL_NEAREST;
                            }
                            else
                            {
                                MCLogger().error() << "Unknown min filter '" << min.c_str() << "'";
                            }

                            if (mag == "linear")
                            {
                                newData->magFilterSet = true;
                                newData->magFilter    = GL_LINEAR;
                            }
                            else if (mag == "nearest")
                            {
                                newData->magFilterSet = true;
                                newData->magFilter    = GL_NEAREST;
                            }
                            else
                            {
                                MCLogger().error() << "Unknown mag filter '" << mag.c_str() << "'";
                            }
                        }
                    }
                    else
                    {
                        MCLogger().error() <<
                            "Unknown tag '" << childNode.nodeName().toStdString() << "'";
                    }

                    childNode = childNode.nextSibling();
                }
            }

            m_surfaces.push_back(newData);

            node = node.nextSibling();
        }
    }

    return true;
}

GLenum MCSurfaceConfigLoader::alphaBlendStringToEnum(
    const std::string & function) const
{
    if (function == "one")
    {
        return GL_ONE;
    }
    else if (function == "zero")
    {
        return GL_ZERO;
    }
    else if (function == "srcColor")
    {
        return GL_SRC_COLOR;
    }
    else if (function == "oneMinusSrcColor")
    {
        return GL_ONE_MINUS_SRC_COLOR;
    }
    else if (function == "srcAlpha")
    {
        return GL_SRC_ALPHA;
    }
    else if (function == "oneMinusSrcAlpha")
    {
        return GL_ONE_MINUS_SRC_ALPHA;
    }
    else if (function == "dstColor")
    {
        return GL_DST_COLOR;
    }
    else if (function == "oneMinusDstColor")
    {
        return GL_ONE_MINUS_DST_COLOR;
    }
    else
    {
        MCLogger().error() << "Unknown alpha blend function '" << function << "'";
        return GL_SRC_ALPHA;
    }
}

unsigned int MCSurfaceConfigLoader::surfaceCount() const
{
    return m_surfaces.size();
}

MCSurfaceData & MCSurfaceConfigLoader::surface(unsigned int index) const
{
    assert(index < static_cast<unsigned int>(m_surfaces.size()));
    assert(m_surfaces.at(index));
    return *m_surfaces.at(index);
}

MCSurfaceConfigLoader::~MCSurfaceConfigLoader()
{
    for (MCSurfaceData * surface : m_surfaces)
    {
        delete surface;
    }

    m_surfaces.clear();
}
