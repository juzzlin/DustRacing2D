// This file is part of Dust Racing 2D.
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// Dust Racing 2D is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// Dust Racing 2D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dust Racing 2D. If not, see <http://www.gnu.org/licenses/>.

#include <QFile>
#include <QTextStream>
#include <QDomDocument>
#include <QDomElement>

#include "mainwindow.hpp"
#include "object.hpp"
#include "objectfactory.hpp"
#include "objectmodel.hpp"
#include "objectmodelloader.hpp"
#include "targetnode.hpp"
#include "trackdata.hpp"
#include "trackio.hpp"
#include "tracktile.hpp"

#include "../common/config.hpp"
#include "../common/objectbase.hpp"
#include "../common/targetnodebase.hpp"

bool TrackIO::save(const TrackData * trackData, QString path)
{
    // Create content
    QDomDocument doc;
    QDomElement root = doc.createElement(TrackDataBase::IO::Header::TRACK());
    root.setAttribute(TrackDataBase::IO::Header::VER(),   Config::Editor::EDITOR_VERSION);
    root.setAttribute(TrackDataBase::IO::Header::NAME(),  trackData->name());
    root.setAttribute(TrackDataBase::IO::Header::COLS(),  trackData->map().cols());
    root.setAttribute(TrackDataBase::IO::Header::ROWS(),  trackData->map().rows());
    root.setAttribute(TrackDataBase::IO::Header::INDEX(), trackData->index());

    if (trackData->isUserTrack()) // Don't add the attribute at all, if not set
    {
        root.setAttribute(TrackDataBase::IO::Header::USER(), true);
    }

    doc.appendChild(root);

    writeTiles(*trackData, root, doc);
    writeObjects(*trackData, root, doc);
    writeTargetNodes(*trackData, root, doc);

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

    const QDomElement root = doc.documentElement();

    const QString name =
        root.attribute(TrackDataBase::IO::Header::NAME(), "undefined");
    const unsigned int cols =
        root.attribute(TrackDataBase::IO::Header::COLS(), "0").toUInt();
    const unsigned int rows =
        root.attribute(TrackDataBase::IO::Header::ROWS(), "0").toUInt();
    const unsigned int index =
        root.attribute(TrackDataBase::IO::Header::INDEX(), "0").toUInt();
    const bool isUserTrack =
        root.attribute(TrackDataBase::IO::Header::USER(), "0").toInt();

    TrackData * newData = nullptr;
    if (cols && rows)
    {
        newData = new TrackData(name, isUserTrack, cols, rows);
        newData->setFileName(path);
        newData->setIndex(index);

        // Temporary route vector.
        std::vector<TargetNodePtr> route;

        QDomNode node = root.firstChild();
        while(!node.isNull())
        {
            QDomElement element = node.toElement();
            if(!element.isNull())
            {
                // Read a tile element
                if (element.nodeName() == TrackDataBase::IO::Track::TILE())
                {
                    readTile(*newData, element);
                }
                // Read an object element
                else if (element.nodeName() == TrackDataBase::IO::Track::OBJECT())
                {
                    readObject(*newData, element);
                }
                // Read a target node element
                else if (element.nodeName() == TrackDataBase::IO::Track::NODE())
                {
                    readTargetNode(route, element);
                }
            }

            node = node.nextSibling();
        }

        // Sort and build route from the temporary vector.
        newData->route().buildFromVector(route);
    }

    return newData;
}

void TrackIO::readTile(TrackData & newData, const QDomElement & element)
{
    const QString id =
        element.attribute(TrackDataBase::IO::Tile::TYPE(), "clear");
    const unsigned int i =
        element.attribute(TrackDataBase::IO::Tile::I(), "0").toUInt();
    const unsigned int j =
        element.attribute(TrackDataBase::IO::Tile::J(), "0").toUInt();
    const int orientation =
        element.attribute(TrackDataBase::IO::Tile::ORIENTATION(), "0").toInt();
    const int computerHint =
        element.attribute(TrackDataBase::IO::Tile::COMPUTER_HINT(), "0").toInt();

    // Init a new tile. QGraphicsScene will take
    // the ownership eventually.
    if (TrackTile * tile = static_cast<TrackTile *>(newData.map().getTile(i, j)))
    {
        tile->setRotation(orientation);
        tile->setTileType(id);
        tile->setPixmap(MainWindow::instance()->objectModelLoader().getPixmapByRole(id));
        tile->setComputerHint(static_cast<TrackTileBase::ComputerHint>(computerHint));
    }
}

void TrackIO::readObject(TrackData & newData, const QDomElement & element)
{
    const QString role = element.attribute(TrackDataBase::IO::Object::ROLE(), "clear");
    const int x = element.attribute(TrackDataBase::IO::Object::X(),            "0").toInt();
    const int y = element.attribute(TrackDataBase::IO::Object::Y(),            "0").toInt();
    const int o = element.attribute(TrackDataBase::IO::Object::ORIENTATION(),  "0").toInt();

    // Create a new object. QGraphicsScene will take
    // the ownership eventually.
    Object & object = ObjectFactory::createObject(role);
    object.setLocation(QPointF(x, y));
    object.setRotation(o);
    newData.objects().add(ObjectPtr(&object));
}

void TrackIO::readTargetNode(std::vector<TargetNodePtr> & route, const QDomElement & element)
{
    const int x = element.attribute(TrackDataBase::IO::Node::X(),      "0").toInt();
    const int y = element.attribute(TrackDataBase::IO::Node::Y(),      "0").toInt();
    const int w = element.attribute(TrackDataBase::IO::Node::WIDTH(),  "0").toInt();
    const int h = element.attribute(TrackDataBase::IO::Node::HEIGHT(), "0").toInt();
    const int i = element.attribute(TrackDataBase::IO::Node::INDEX(),  "0").toInt();

    // Create a new object. QGraphicsScene will take
    // the ownership eventually.
    TargetNode * tnode = new TargetNode;
    tnode->setIndex(i);
    tnode->setLocation(QPointF(x, y));

    if (w > 0 && h > 0)
    {
        tnode->setSize(QSizeF(w, h));
    }

    route.push_back(TargetNodePtr(tnode));
}

void TrackIO::writeTiles(
    const TrackData & trackData, QDomElement & root, QDomDocument & doc)
{
    for (unsigned int i = 0; i < trackData.map().cols(); i++)
    {
        for (unsigned int j = 0; j < trackData.map().rows(); j++)
        {
            if (TrackTile * tile = static_cast<TrackTile *>(trackData.map().getTile(i, j)))
            {
                QDomElement tileElement = doc.createElement(TrackDataBase::IO::Track::TILE());
                tileElement.setAttribute(TrackDataBase::IO::Tile::TYPE(), tile->tileType());
                tileElement.setAttribute(TrackDataBase::IO::Tile::I(), i);
                tileElement.setAttribute(TrackDataBase::IO::Tile::J(), j);
                tileElement.setAttribute(TrackDataBase::IO::Tile::ORIENTATION(), tile->rotation());

                if (tile->computerHint() != TrackTile::CH_NONE)
                {
                    tileElement.setAttribute(TrackDataBase::IO::Tile::COMPUTER_HINT(), tile->computerHint());
                }

                root.appendChild(tileElement);
            }
        }
    }
}

void TrackIO::writeObjects(const TrackData & trackData, QDomElement & root, QDomDocument & doc)
{
    for (unsigned int i = 0; i < trackData.objects().count(); i++)
    {
        QDomElement objectElement = doc.createElement(TrackDataBase::IO::Track::OBJECT());
        Object *    object        = static_cast<Object *>(trackData.objects().object(i).get());

        objectElement.setAttribute(TrackDataBase::IO::Object::CATEGORY(), object->category());
        objectElement.setAttribute(TrackDataBase::IO::Object::ROLE(), object->role());
        objectElement.setAttribute(TrackDataBase::IO::Object::X(), static_cast<int>(object->location().x()));
        objectElement.setAttribute(TrackDataBase::IO::Object::Y(), static_cast<int>(object->location().y()));
        objectElement.setAttribute(TrackDataBase::IO::Object::ORIENTATION(), static_cast<int>(object->rotation()));

        root.appendChild(objectElement);
    }
}

void TrackIO::writeTargetNodes(
    const TrackData & trackData, QDomElement & root, QDomDocument & doc)
{
    const Route & route = trackData.route();
    for (unsigned int i = 0; i < route.numNodes(); i++)
    {
        QDomElement   tnodeElement = doc.createElement(TrackDataBase::IO::Track::NODE());
        TargetNodePtr tnode        = route.get(i);

        tnodeElement.setAttribute(TrackDataBase::IO::Node::INDEX(), tnode->index());
        tnodeElement.setAttribute(TrackDataBase::IO::Node::X(), static_cast<int>(tnode->location().x()));
        tnodeElement.setAttribute(TrackDataBase::IO::Node::Y(), static_cast<int>(tnode->location().y()));
        tnodeElement.setAttribute(TrackDataBase::IO::Node::WIDTH(), static_cast<int>(tnode->size().width()));
        tnodeElement.setAttribute(TrackDataBase::IO::Node::HEIGHT(), static_cast<int>(tnode->size().height()));

        root.appendChild(tnodeElement);
    }
}
