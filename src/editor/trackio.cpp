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

#include "mainwindow.hpp"
#include "object.hpp"
#include "objectmodel.hpp"
#include "objectmodelloader.hpp"
#include "trackdata.hpp"
#include "trackio.hpp"
#include "tracktile.hpp"

#include "../common/config.hpp"
#include "../common/objectbase.hpp"

bool TrackIO::save(const TrackData * trackData, QString path)
{
    // Create content
    QDomDocument doc;
    QDomElement root = doc.createElement("track");
    root.setAttribute("version", Config::Editor::EDITOR_VERSION);
    root.setAttribute("name", trackData->name());
    root.setAttribute("cols", trackData->map().cols());
    root.setAttribute("rows", trackData->map().rows());
    root.setAttribute("lapCount", trackData->lapCount());
    doc.appendChild(root);

    // Add information about tiles
    for (unsigned int i = 0; i < trackData->map().cols(); i++)
        for (unsigned int j = 0; j < trackData->map().rows(); j++)
            if (TrackTile * tile = static_cast<TrackTile *>(trackData->map().getTile(i, j)))
            {
                QDomElement tileTag = doc.createElement("tile");
                tileTag.setAttribute("type", tile->tileType());
                tileTag.setAttribute("i", i);
                tileTag.setAttribute("j", j);
                tileTag.setAttribute("o", tile->rotation());

                if (tile->routeIndex() != -1)
                {
                    tileTag.setAttribute("index", tile->routeIndex());
                }

                if (tile->computerHint() != TrackTile::CH_NONE)
                {
                    tileTag.setAttribute("computerHint", tile->computerHint());
                }

                if (tile->drivingLineHintH() != TrackTile::DLHH_NONE)
                {
                    tileTag.setAttribute("drivingLineHintH", tile->drivingLineHintH());
                }

                if (tile->drivingLineHintV() != TrackTile::DLHV_NONE)
                {
                    tileTag.setAttribute("drivingLineHintV", tile->drivingLineHintV());
                }

                root.appendChild(tileTag);
            }

    // Add information about objects
    for (unsigned int i = 0; i < trackData->objects().count(); i++)
    {
        QDomElement objectTag = doc.createElement("object");
        Object & object = static_cast<Object &>(trackData->objects().object(i));
        objectTag.setAttribute("category", object.category());
        objectTag.setAttribute("role", object.role());
        objectTag.setAttribute("x", static_cast<int>(object.location().x()));
        objectTag.setAttribute("y", static_cast<int>(object.location().y()));
        objectTag.setAttribute("o", static_cast<int>(object.rotation()));
        root.appendChild(objectTag);
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
        return nullptr;
    }

    if (!doc.setContent(&file))
    {
        file.close();
        return nullptr;
    }

    file.close();

    const QDomElement  root     = doc.documentElement();
    const QString      version  = root.attribute("version", Config::Editor::EDITOR_VERSION);
    const QString      name     = root.attribute("name", "undefined");
    const unsigned int cols     = root.attribute("cols", "0").toUInt();
    const unsigned int rows     = root.attribute("rows", "0").toUInt();
    const unsigned int lapCount = root.attribute("lapCount", "0").toUInt();

    TrackData * newData = nullptr;
    if (cols > 0 && rows > 0)
    {
        newData = new TrackData(name, cols, rows);
        newData->setFileName(path);
        newData->setLapCount(lapCount);

        QVector<TrackTileBase *> routeVector;

        QDomNode node = root.firstChild();
        while(!node.isNull())
        {
            QDomElement tag = node.toElement();
            if(!tag.isNull())
            {
                // Read a tile tag
                if (tag.nodeName() == "tile")
                {
                    const QString      id      = tag.attribute("type", "clear");
                    const unsigned int i       = tag.attribute("i", "0").toUInt();
                    const unsigned int j       = tag.attribute("j", "0").toUInt();
                    const int          o       = tag.attribute("o", "0").toInt();
                    const int      index       = tag.attribute("index", "-1").toInt();
                    const int computerHint     = tag.attribute("computerHint", "0").toInt();
                    const int drivingLineHintH = tag.attribute("drivingLineHintH", "0").toInt();
                    const int drivingLineHintV = tag.attribute("drivingLineHintV", "0").toInt();

                    // Init a new tile. QGraphicsScene will take
                    // the ownership eventually.
                    if (TrackTile * tile = static_cast<TrackTile *>(newData->map().getTile(i, j)))
                    {
                        tile->setRotation(o);
                        tile->setTileType(id);
                        tile->setPixmap(MainWindow::instance()->objectModelLoader().getPixmapByRole(id));
                        tile->setRouteIndex(index);
                        tile->setComputerHint(
                            static_cast<TrackTileBase::ComputerHint>(computerHint));
                        tile->setDrivingLineHintH(
                            static_cast<TrackTileBase::DrivingLineHintH>(drivingLineHintH));
                        tile->setDrivingLineHintV(
                            static_cast<TrackTileBase::DrivingLineHintV>(drivingLineHintV));

                        if (index >= 0)
                            routeVector << tile;
                    }
                }
                // Read an object tag
                else if (tag.nodeName() == "object")
                {
                    const QString role     = tag.attribute("role", "clear");
                    const QString category = tag.attribute("category", "clear");
                    const int     x        = tag.attribute("x", "0").toInt();
                    const int     y        = tag.attribute("y", "0").toInt();
                    const int     o        = tag.attribute("o", "0").toInt();

                    ObjectModel model =
                        MainWindow::instance()->objectModelLoader().getObjectModelByRole(
                            role);

                    // Create a new object. QGraphicsScene will take
                    // the ownership eventually.
                    Object * object = new Object(category, role,
                        QSizeF(model.width, model.height),
                        model.pixmap);
                    object->setLocation(QPointF(x, y));
                    object->setRotation(o);
                    newData->objects().add(*object);
                }
            }

            node = node.nextSibling();
        }

        newData->route().buildFromVector(routeVector);
    }

    return newData;
}
