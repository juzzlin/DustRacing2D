// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#include <QFile>
#include <QTextStream>
#include <QDomDocument>
#include <QDomElement>

#include "objectloader.h"
#include "version.h"

bool ObjectLoader::load(QString path)
{
    QDomDocument doc;

    QFile file(path);
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

    QDomElement  root    = doc.documentElement();
    QString      version = root.attribute("version", Version::EDITOR_VERSION);

    objectDataVector.clear();

    QDomNode node = root.firstChild();
    while(!node.isNull())
    {
        QDomElement tag = node.toElement();
        if(!tag.isNull())
        {
            // Parse an object tag
            if (tag.nodeName() == "object")
            {
                ObjectData newData;

                newData.category  = tag.attribute("category",  "undefined");
                newData.role      = tag.attribute("role",      "undefined");
                newData.imagePath = tag.attribute("imagePath", "undefined");

                objectDataVector << newData;
            }

            node = node.nextSibling();
        }
    }

    return true;
}

ObjectLoader::ObjectDataVector ObjectLoader::getObjectsByCategory(QString category) const
{
    ObjectDataVector result;

    for (int i = 0; i < objectDataVector.size(); i++)
    {
        if (objectDataVector[i].category == category)
        {
            result << objectDataVector[i];
        }
    }

    return result;
}

ObjectLoader::ObjectDataVector ObjectLoader::getObjectsByRole(QString role) const
{
    ObjectDataVector result;

    for (int i = 0; i < objectDataVector.size(); i++)
    {
        if (objectDataVector[i].role == role)
        {
            result << objectDataVector[i];
        }
    }

    return result;
}

