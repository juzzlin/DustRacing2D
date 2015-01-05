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

#include "mcexception.hh"
#include "mcsurfaceconfigloader.hh"
#include "mcsurfacemetadata.hh"
#include "mclogger.hh"

#include <cassert>

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
            QDomElement tag = node.toElement();
            if(!tag.isNull())
            {
                const std::string image = tag.attribute("image", "").toStdString();
                newData->imagePath      = baseImagePath + QDir::separator().toLatin1() + image;
                newData->handle         = tag.attribute("handle", "").toStdString();
                newData->handle2        = tag.attribute("handle2", "").toStdString();
                newData->handle3        = tag.attribute("handle3", "").toStdString();
                newData->xAxisMirror    = tag.attribute("xAxisMirror", "0").toInt();
                newData->z0             = tag.attribute("z0", "0").toInt();
                newData->z1             = tag.attribute("z1", "0").toInt();
                newData->z2             = tag.attribute("z2", "0").toInt();
                newData->z3             = tag.attribute("z3", "0").toInt();

                const unsigned int width = tag.attribute("w", "0").toUInt();
                newData->width = std::pair<int, bool>(width, width > 0);

                const unsigned int height = tag.attribute("h", "0").toUInt();
                newData->height = std::pair<int, bool>(height, height > 0);

                const float specularCoeff = tag.attribute("specularCoeff", "1").toFloat();
                newData->specularCoeff = std::pair<GLfloat, bool>(specularCoeff, (specularCoeff > 1.0));

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
                            newData->alphaBlend.first.m_src =
                                alphaBlendStringToEnum(
                                    tag.attribute("src", "srcAlpha").toStdString());
                            newData->alphaBlend.first.m_dst =
                                alphaBlendStringToEnum(
                                    tag.attribute(
                                        "dst", "srcAlphaMinusOne").toStdString());
                            newData->alphaBlend.second = true;
                        }
                    }
                    else if (childNode.nodeName() == "center")
                    {
                        QDomElement tag = childNode.toElement();
                        if(!tag.isNull())
                        {
                            const int x = tag.attribute("x", "0").toInt();
                            const int y = tag.attribute("y", "0").toInt();

                            newData->center.first  = MCVector2d<int>(x, y);
                            newData->center.second = true;
                        }
                    }
                    else if (childNode.nodeName() == "filter")
                    {
                        QDomElement tag = childNode.toElement();
                        if(!tag.isNull())
                        {
                            const std::string min = tag.attribute("min", "").toStdString();
                            const std::string mag = tag.attribute("mag", "").toStdString();

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
                                throw MCException("Unknown min filter '" + min + "'");
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
                                throw MCException("Unknown mag filter '" + mag + "'");
                            }
                        }
                    }
                    else if (childNode.nodeName() == "wrap")
                    {
                        QDomElement tag = childNode.toElement();
                        if(!tag.isNull())
                        {
                            const std::string s = tag.attribute("s", "").toStdString();
                            const std::string t = tag.attribute("t", "").toStdString();

                            if (s == "clamp")
                            {
                                newData->wrapS = std::pair<GLint, bool>(GL_CLAMP, true);
                            }
                            else if (s == "repeat")
                            {
                                newData->wrapS = std::pair<GLint, bool>(GL_REPEAT, true);
                            }
                            else
                            {
                                throw MCException("Unknown s wrap '" + s + "'");
                            }

                            if (t == "clamp")
                            {
                                newData->wrapT = std::pair<GLint, bool>(GL_CLAMP, true);
                            }
                            else if (t == "repeat")
                            {
                                newData->wrapT = std::pair<GLint, bool>(GL_REPEAT, true);
                            }
                            else
                            {
                                throw MCException("Unknown t wrap '" + t + "'");
                            }
                        }
                    }
                    else
                    {
                        throw MCException("Unknown tag '" + childNode.nodeName().toStdString() + "'");
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
    try
    {
        return m_blendFuncMap.at(function);
    }
    catch (...)
    {
        throw MCException("Unknown alpha blend function '" + function + "'");
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
