// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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
#include "mcsurfacemetadata.hh"
#include "mclogger.hh"

#include <cassert>
#include <exception>

MCSurfaceConfigLoader::MCSurfaceConfigLoader()
: m_surfaces()
{
    m_blendFuncMap["one"]              = GL_ONE;
    m_blendFuncMap["zero"]             = GL_ZERO;
    m_blendFuncMap["srcColor"]         = GL_SRC_COLOR;
    m_blendFuncMap["oneMinusSrcColor"] = GL_ONE_MINUS_SRC_COLOR;
    m_blendFuncMap["srcAlpha"]         = GL_SRC_ALPHA;
    m_blendFuncMap["oneMinusSrcAlpha"] = GL_ONE_MINUS_SRC_ALPHA;
    m_blendFuncMap["dstColor"]         = GL_DST_COLOR;
    m_blendFuncMap["oneMinusDstColor"] = GL_ONE_MINUS_DST_COLOR;
}

void MCSurfaceConfigLoader::parseAttributes(const QDomElement & element, SurfaceDataPtr newData, const std::string & baseImagePath)
{
    if (element.hasAttribute("image"))
    {
        const std::string image = element.attribute("image", "").toStdString();
        newData->imagePath = baseImagePath + QDir::separator().toLatin1() + image;
    }
    else
    {
        throw std::runtime_error("Attribute 'image' is required for a surface!");
    }

    if (element.hasAttribute("handle"))
    {
        newData->handle = element.attribute("handle", "").toStdString();
    }
    else
    {
        throw std::runtime_error("Attribute 'handle' is required for a surface!");
    }

    newData->handle2 = element.attribute("handle2", "").toStdString();
    newData->handle3 = element.attribute("handle3", "").toStdString();
    newData->xAxisMirror = element.attribute("xAxisMirror", "0").toInt();

    if (element.hasAttribute("z")) // Shorthand z
    {
        const float z = element.attribute("z", "0").toFloat();
        newData->z0 = z;
        newData->z1 = z;
        newData->z2 = z;
        newData->z3 = z;
    }
    else if (
        element.hasAttribute("z0") ||
        element.hasAttribute("z1") ||
        element.hasAttribute("z2") ||
        element.hasAttribute("z3"))
    {
        newData->z0 = element.attribute("z0", "0").toFloat();
        newData->z1 = element.attribute("z1", "0").toFloat();
        newData->z2 = element.attribute("z2", "0").toFloat();
        newData->z3 = element.attribute("z3", "0").toFloat();
    }

    if (element.hasAttribute("w"))
    {
        const unsigned int width = element.attribute("w", "0").toUInt();
        newData->width = std::pair<int, bool>(width, width > 0);
    }

    if (element.hasAttribute("h"))
    {
        const unsigned int height = element.attribute("h", "0").toUInt();
        newData->height = std::pair<int, bool>(height, height > 0);
    }

    if (element.hasAttribute("specularCoeff"))
    {
        const float specularCoeff = element.attribute("specularCoeff", "1").toFloat();
        newData->specularCoeff = std::pair<GLfloat, bool>(specularCoeff, specularCoeff > 1.0f);
    }
}

void MCSurfaceConfigLoader::parseChildNodes(const QDomNode & node, SurfaceDataPtr newData)
{
    // Read child nodes of surface node.
    QDomNode childNode = node.firstChild();
    while (!childNode.isNull())
    {
        if (childNode.nodeName() == "colorKey")
        {
            const QDomElement element = childNode.toElement();
            if (!element.isNull())
            {
                newData->colorKey.m_r = element.attribute("r", "0").toUInt();
                newData->colorKey.m_g = element.attribute("g", "0").toUInt();
                newData->colorKey.m_b = element.attribute("b", "0").toUInt();
                newData->colorKeySet  = true;
            }
        }
        else if (childNode.nodeName() == "alphaBlend")
        {
            const QDomElement element = childNode.toElement();
            if (!element.isNull())
            {
                newData->alphaBlend.first.m_src =
                    alphaBlendStringToEnum(
                        element.attribute("src", "srcAlpha").toStdString());
                newData->alphaBlend.first.m_dst =
                    alphaBlendStringToEnum(
                        element.attribute(
                            "dst", "srcAlphaMinusOne").toStdString());
                newData->alphaBlend.second = true;
            }
        }
        else if (childNode.nodeName() == "center")
        {
            const QDomElement element = childNode.toElement();
            if (!element.isNull())
            {
                const int x = element.attribute("x", "0").toInt();
                const int y = element.attribute("y", "0").toInt();

                newData->center.first  = MCVector2d<int>(x, y);
                newData->center.second = true;
            }
        }
        else if (childNode.nodeName() == "filter")
        {
            const QDomElement element = childNode.toElement();
            if (!element.isNull())
            {
                const std::string min = element.attribute("min", "").toStdString();
                const std::string mag = element.attribute("mag", "").toStdString();

                if (min == "linear")
                {
                    newData->minFilter = std::pair<GLint, bool>(GL_LINEAR, true);
                }
                else if (min == "nearest")
                {
                    newData->minFilter = std::pair<GLint, bool>(GL_NEAREST, true);
                }
                else
                {
                    throw std::runtime_error("Unknown min filter '" + min + "'");
                }

                if (mag == "linear")
                {
                    newData->magFilter = std::pair<GLint, bool>(GL_LINEAR, true);
                }
                else if (mag == "nearest")
                {
                    newData->magFilter = std::pair<GLint, bool>(GL_NEAREST, true);
                }
                else
                {
                    throw std::runtime_error("Unknown mag filter '" + mag + "'");
                }
            }
        }
        else if (childNode.nodeName() == "wrap")
        {
            const QDomElement element = childNode.toElement();
            if (!element.isNull())
            {
                const std::string s = element.attribute("s", "").toStdString();
                const std::string t = element.attribute("t", "").toStdString();

                // Windows build hack
                #ifndef GL_CLAMP_TO_EDGE
                    #define GL_CLAMP_TO_EDGE 0x812F
                #endif

                if (s == "clamp")
                {
                    newData->wrapS = std::pair<GLint, bool>(GL_CLAMP_TO_EDGE, true);
                }
                else if (s == "repeat")
                {
                    newData->wrapS = std::pair<GLint, bool>(GL_REPEAT, true);
                }
                else
                {
                    throw std::runtime_error("Unknown s wrap '" + s + "'");
                }

                if (t == "clamp")
                {
                    newData->wrapT = std::pair<GLint, bool>(GL_CLAMP_TO_EDGE, true);
                }
                else if (t == "repeat")
                {
                    newData->wrapT = std::pair<GLint, bool>(GL_REPEAT, true);
                }
                else
                {
                    throw std::runtime_error("Unknown t wrap '" + t + "'");
                }
            }
        }
        else
        {
            throw std::runtime_error("Unknown element '" + childNode.nodeName().toStdString() + "'");
        }

        childNode = childNode.nextSibling();
    }
}

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
        QDomNode node = root.firstChild();
        while(!node.isNull() && node.nodeName() == "surface")
        {
            SurfaceDataPtr newData(new MCSurfaceMetaData);
            QDomElement element = node.toElement();
            if(!element.isNull())
            {
                parseAttributes(element, newData, baseImagePath);
                parseChildNodes(node, newData);
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
    try
    {
        return m_blendFuncMap.at(function);
    }
    catch (...)
    {
        throw std::runtime_error("Unknown alpha blend function '" + function + "'");
    }

    return GL_SRC_ALPHA;
}

unsigned int MCSurfaceConfigLoader::surfaceCount() const
{
    return static_cast<unsigned int>(m_surfaces.size());
}

const MCSurfaceMetaData & MCSurfaceConfigLoader::surface(unsigned int index) const
{
    assert(index < static_cast<unsigned int>(m_surfaces.size()));
    assert(m_surfaces.at(index));
    return *m_surfaces.at(index);
}
