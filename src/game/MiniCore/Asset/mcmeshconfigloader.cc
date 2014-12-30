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

#include "mcmeshconfigloader.hh"
#include "mclogger.hh"

#include <cassert>

MCMeshConfigLoader::MCMeshConfigLoader()
{
}

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

    QDomElement root = doc.documentElement();
    if (root.nodeName() == "meshes")
    {
        const std::string baseModelPath = root.attribute("baseModelPath", "./").toStdString();
        QDomNode node = root.firstChild();
        while(!node.isNull() && node.nodeName() == "mesh")
        {
            MeshMetaDataPtr newData(new MCMeshMetaData);
            QDomElement tag = node.toElement();
            if(!tag.isNull())
            {
                const std::string model = tag.attribute("model", "").toStdString();
                newData->handle         = tag.attribute("handle", "").toStdString();
                newData->texture1       = tag.attribute("texture1", "").toStdString();
                newData->texture2       = tag.attribute("texture2", "").toStdString();

                if (!model.empty())
                {
                    newData->modelPath = baseModelPath + QDir::separator().toLatin1() + model;
                }
            }

            m_meshes.push_back(newData);

            node = node.nextSibling();
        }
    }

    return true;
}

unsigned int MCMeshConfigLoader::meshCount() const
{
    return static_cast<unsigned int>(m_meshes.size());
}

const MCMeshMetaData & MCMeshConfigLoader::mesh(unsigned int index) const
{
    assert(index < static_cast<unsigned int>(m_meshes.size()));
    assert(m_meshes.at(index));
    return *m_meshes.at(index);
}
