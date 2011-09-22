// This file is part of Dust Rallycross (DustRAC).
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

#include "trackio.h"
#include "tracktile.h"
#include "version.h"
#include "../common/trackdata.h"

bool TrackIO::save(const TrackData * trackData, QString path)
{
    // Create content
    QDomDocument doc;
    QDomElement root = doc.createElement("track");
    root.setAttribute("version", Version::EDITOR_VERSION);
    root.setAttribute("name", trackData->name());
    root.setAttribute("cols", trackData->map().cols());
    root.setAttribute("rows", trackData->map().rows());
    doc.appendChild(root);

    // Add information about tiles
    for (unsigned int i = 0; i < trackData->map().cols(); i++)
        for (unsigned int j = 0; j < trackData->map().rows(); j++)
            if (TrackTile * tile = trackData->map().tile(i, j))
            {
                QDomElement tileTag = doc.createElement("tile");
                tileTag.setAttribute("type", tile->tileType());
                tileTag.setAttribute("i", i);
                tileTag.setAttribute("j", j);
                tileTag.setAttribute("o", tile->rotation());

                if (tile->routeIndex() != -1)
                    tileTag.setAttribute("index", tile->routeIndex());

                root.appendChild(tileTag);
            }

    // Save to file
    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        out << doc.toString();
        file.close();
        return true;
    }

    return false;
}

TrackData * TrackIO::open(QString path)
{
    QDomDocument doc;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        return NULL;
    }

    if (!doc.setContent(&file))
    {
        file.close();
        return NULL;
    }

    file.close();

    QDomElement  root    = doc.documentElement();
    QString      version = root.attribute("version", Version::EDITOR_VERSION);
    QString      name    = root.attribute("name", "undefined");
    unsigned int cols    = root.attribute("cols", "0").toUInt();
    unsigned int rows    = root.attribute("rows", "0").toUInt();

    TrackData * newData = NULL;
    if (cols > 0 && rows > 0)
    {
        newData = new TrackData(name, cols, rows);
        newData->setFileName(path);

        QVector<TrackTile *> routeVector;

        QDomNode node = root.firstChild();
        while(!node.isNull())
        {
            QDomElement tag = node.toElement();
            if(!tag.isNull())
            {
                // Read a tile tag
                if (tag.nodeName() == "tile")
                {
                    QString      id = tag.attribute("type", "clear");
                    unsigned int i  = tag.attribute("i", "0").toUInt();
                    unsigned int j  = tag.attribute("j", "0").toUInt();
                    int          o  = tag.attribute("o", "0").toInt();
                    int      index  = tag.attribute("index", "-1").toInt();

                    if (TrackTile * tile = newData->map().tile(i, j))
                    {
                        tile->setRotation(o);
                        tile->setTileType(id);
                        tile->setRouteIndex(index);

                        if (index >= 0)
                            routeVector << tile;
                    }
                }
            }

            node = node.nextSibling();
        }

        newData->route().buildFromVector(routeVector);
    }

    return newData;
}
