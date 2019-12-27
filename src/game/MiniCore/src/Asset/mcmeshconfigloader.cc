// This file belongs to the "MiniCore" game engine.
// Copyright (C) 2013 Jussi Lind <jussi.lind@iki.fi>
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

#include "mclogger.hh"
#include "mcmeshconfigloader.hh"

#include <cassert>

bool MCMeshConfigLoader::load(const std::string & filePath)
{
    QDomDocument doc;
    QFile file(filePath.c_str());
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

    const auto && root = doc.documentElement();
    if (root.nodeName() == "meshes")
    {
        const std::string baseModelPath = root.attribute("baseModelPath", "./").toStdString();
        auto && node = root.firstChild();
        while (!node.isNull() && node.nodeName() == "mesh")
        {
            const auto newData = std::make_shared<MCMeshMetaData>();
            const auto && element = node.toElement();
            if (!element.isNull())
            {
                parseAttributes(element, newData, baseModelPath);
                parseChildNodes(node, newData);
            }

            m_meshes.push_back(newData);

            node = node.nextSibling();
        }
    }

    return true;
}

void MCMeshConfigLoader::parseAttributes(const QDomElement & element, MCMeshDataPtr newData, const std::string & baseModelPath)
{
    const std::string model = element.attribute("model", "").toStdString();

    newData->handle = element.attribute("handle", "").toStdString();

    newData->texture1 = element.attribute("texture1", "").toStdString();

    newData->texture2 = element.attribute("texture2", "").toStdString();

    if (!model.empty())
    {
        newData->modelPath = baseModelPath + QDir::separator().toLatin1() + model;
    }
}

void MCMeshConfigLoader::parseChildNodes(const QDomNode & node, MCMeshDataPtr newData)
{
    auto && childNode = node.firstChild();
    while (!childNode.isNull())
    {
        if (childNode.nodeName() == "color")
        {
            const auto && element = childNode.toElement();
            if (!element.isNull())
            {
                newData->color.setR(element.attribute("r", "1").toFloat());
                newData->color.setG(element.attribute("g", "1").toFloat());
                newData->color.setB(element.attribute("b", "1").toFloat());
                newData->color.setA(element.attribute("a", "1").toFloat());
            }
        }
        else if (childNode.nodeName() == "scale")
        {
            const auto && element = childNode.toElement();
            if (!element.isNull())
            {
                newData->scale.first.setI(element.attribute("x", "1").toFloat());
                newData->scale.first.setJ(element.attribute("y", "1").toFloat());
                newData->scale.first.setK(element.attribute("z", "1").toFloat());
                newData->scale.second = true;
            }
        }
        else
        {
            throw std::runtime_error("Unknown element '" + childNode.nodeName().toStdString() + "'");
        }

        childNode = childNode.nextSibling();
    }
}

size_t MCMeshConfigLoader::meshCount() const
{
    return m_meshes.size();
}

MCMeshDataPtr MCMeshConfigLoader::mesh(size_t index) const
{
    return m_meshes.at(index);
}
