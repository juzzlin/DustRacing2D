// This file is part of Dust Racing 2D.
// Copyright (C) 2015 Jussi Lind <jussi.lind@iki.fi>
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

#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QTextStream>

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

#include <cassert>
#include <memory>

namespace {

void readTile(TrackData & newData, const QDomElement & element)
{
    // Init a new tile. QGraphicsScene will take
    // the ownership eventually.
    auto tile = dynamic_cast<TrackTile *>(newData.map().getTile(
                                                         element.attribute(TrackDataBase::DataKeywords::Tile::i, "0").toUInt(),
                                                         element.attribute(TrackDataBase::DataKeywords::Tile::j, "0").toUInt())
                                            .get());
    assert(tile);

    tile->setRotation(element.attribute(TrackDataBase::DataKeywords::Tile::orientation, "0").toInt());

    const auto type = element.attribute(TrackDataBase::DataKeywords::Tile::type, "clear");

    tile->setTileType(type);

    tile->setComputerHint(
      static_cast<TrackTileBase::ComputerHint>(element.attribute(TrackDataBase::DataKeywords::Tile::computerHint, "0").toInt()));

    tile->setExcludeFromMinimap(element.attribute(TrackDataBase::DataKeywords::Tile::excludeFromMinimap, "0").toUInt());
}

void readObject(TrackData & newData, const QDomElement & element)
{
    // Create a new object. QGraphicsScene will take
    // the ownership eventually.
    Object & object = ObjectFactory::createObject(element.attribute(TrackDataBase::DataKeywords::Object::role, "clear"));

    object.setLocation(QPointF(
      element.attribute(TrackDataBase::DataKeywords::Object::x, "0").toInt(),
      element.attribute(TrackDataBase::DataKeywords::Object::y, "0").toInt()));

    object.setRotation(element.attribute(TrackDataBase::DataKeywords::Object::orientation, "0").toInt());

    object.setForceStationary(element.attribute(TrackDataBase::DataKeywords::Object::forceStationary, "0").toUInt());

    newData.objects().add(ObjectBasePtr(&object));
}

void readTargetNode(std::vector<TargetNodeBasePtr> & route, const QDomElement & element)
{
    // Create a new object. QGraphicsScene will take
    // the ownership eventually.
    auto tnode = new TargetNode;
    tnode->setIndex(element.attribute(TrackDataBase::DataKeywords::Node::index, "0").toInt());

    const int x = element.attribute(TrackDataBase::DataKeywords::Node::x, "0").toInt();
    const int y = element.attribute(TrackDataBase::DataKeywords::Node::y, "0").toInt();

    tnode->setLocation(QPointF(x, y));

    const int w = element.attribute(TrackDataBase::DataKeywords::Node::width, "0").toInt();
    const int h = element.attribute(TrackDataBase::DataKeywords::Node::height, "0").toInt();

    if (w > 0 && h > 0)
    {
        tnode->setSize(QSizeF(w, h));
    }

    route.push_back(TargetNodeBasePtr(tnode));
}

void writeTiles(const TrackDataPtr trackData, QDomElement & root, QDomDocument & doc)
{
    for (unsigned int i = 0; i < trackData->map().cols(); i++)
    {
        for (unsigned int j = 0; j < trackData->map().rows(); j++)
        {
            auto tile = std::dynamic_pointer_cast<TrackTile>(trackData->map().getTile(i, j));
            assert(tile);

            QDomElement tileElement = doc.createElement(TrackDataBase::DataKeywords::Track::tile);
            tileElement.setAttribute(TrackDataBase::DataKeywords::Tile::type, tile->tileType());
            tileElement.setAttribute(TrackDataBase::DataKeywords::Tile::i, i);
            tileElement.setAttribute(TrackDataBase::DataKeywords::Tile::j, j);
            tileElement.setAttribute(TrackDataBase::DataKeywords::Tile::orientation, tile->rotation());

            if (tile->excludeFromMinimap())
            {
                tileElement.setAttribute(TrackDataBase::DataKeywords::Tile::excludeFromMinimap, true);
            }

            if (tile->computerHint() != TrackTile::ComputerHint::None)
            {
                tileElement.setAttribute(TrackDataBase::DataKeywords::Tile::computerHint, static_cast<int>(tile->computerHint()));
            }

            root.appendChild(tileElement);
        }
    }
}

void writeObjects(TrackDataPtr trackData, QDomElement & root, QDomDocument & doc)
{
    for (auto objectPtr : trackData->objects())
    {
        auto object = std::dynamic_pointer_cast<Object>(objectPtr);
        assert(object);

        QDomElement objectElement = doc.createElement(TrackDataBase::DataKeywords::Track::object);
        objectElement.setAttribute(TrackDataBase::DataKeywords::Object::category, object->category());
        objectElement.setAttribute(TrackDataBase::DataKeywords::Object::role, object->role());
        objectElement.setAttribute(TrackDataBase::DataKeywords::Object::x, static_cast<int>(object->location().x()));
        objectElement.setAttribute(TrackDataBase::DataKeywords::Object::y, static_cast<int>(object->location().y()));
        objectElement.setAttribute(TrackDataBase::DataKeywords::Object::orientation, static_cast<int>(object->rotation()));

        if (object->forceStationary())
        {
            objectElement.setAttribute(TrackDataBase::DataKeywords::Object::forceStationary, static_cast<int>(object->forceStationary()));
        }

        root.appendChild(objectElement);
    }
}

void writeTargetNodes(TrackDataPtr trackData, QDomElement & root, QDomDocument & doc)
{
    for (auto tnode : trackData->route())
    {
        QDomElement tnodeElement = doc.createElement(TrackDataBase::DataKeywords::Track::node);
        tnodeElement.setAttribute(TrackDataBase::DataKeywords::Node::index, tnode->index());
        tnodeElement.setAttribute(TrackDataBase::DataKeywords::Node::x, static_cast<int>(tnode->location().x()));
        tnodeElement.setAttribute(TrackDataBase::DataKeywords::Node::y, static_cast<int>(tnode->location().y()));
        tnodeElement.setAttribute(TrackDataBase::DataKeywords::Node::width, static_cast<int>(tnode->size().width()));
        tnodeElement.setAttribute(TrackDataBase::DataKeywords::Node::height, static_cast<int>(tnode->size().height()));

        root.appendChild(tnodeElement);
    }
}

} // namespace

bool TrackIO::save(TrackDataPtr trackData, QString path)
{
    // Create content
    QDomDocument doc;
    QDomElement root = doc.createElement(TrackDataBase::DataKeywords::Header::track);
    root.setAttribute(TrackDataBase::DataKeywords::Header::version, Config::Editor::EDITOR_VERSION);
    root.setAttribute(TrackDataBase::DataKeywords::Header::name, trackData->name());
    root.setAttribute(TrackDataBase::DataKeywords::Header::cols, trackData->map().cols());
    root.setAttribute(TrackDataBase::DataKeywords::Header::rows, trackData->map().rows());
    root.setAttribute(TrackDataBase::DataKeywords::Header::index, trackData->index());

    if (trackData->isUserTrack()) // Don't add the attribute at all, if not set
    {
        root.setAttribute(TrackDataBase::DataKeywords::Header::user, true);
    }

    doc.appendChild(root);

    writeTiles(trackData, root, doc);
    writeObjects(trackData, root, doc);
    writeTargetNodes(trackData, root, doc);

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

TrackDataPtr TrackIO::open(QString path)
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

    const unsigned int cols =
      root.attribute(TrackDataBase::DataKeywords::Header::cols, "0").toUInt();
    const unsigned int rows =
      root.attribute(TrackDataBase::DataKeywords::Header::rows, "0").toUInt();

    TrackData * newData = nullptr;
    if (cols && rows)
    {
        const QString name = root.attribute(TrackDataBase::DataKeywords::Header::name, "undefined");
        const bool isUserTrack = root.attribute(TrackDataBase::DataKeywords::Header::user, "0").toInt();
        newData = new TrackData(name, isUserTrack, cols, rows);
        newData->setFileName(path);

        const unsigned int index = root.attribute(TrackDataBase::DataKeywords::Header::index, "0").toUInt();
        newData->setIndex(index);

        // Temporary route vector.
        std::vector<TargetNodeBasePtr> route;

        QDomNode node = root.firstChild();
        while (!node.isNull())
        {
            QDomElement element = node.toElement();
            if (!element.isNull())
            {
                // Read a tile element
                if (element.nodeName() == TrackDataBase::DataKeywords::Track::tile)
                {
                    readTile(*newData, element);
                }
                // Read an object element
                else if (element.nodeName() == TrackDataBase::DataKeywords::Track::object)
                {
                    readObject(*newData, element);
                }
                // Read a target node element
                else if (element.nodeName() == TrackDataBase::DataKeywords::Track::node)
                {
                    readTargetNode(route, element);
                }
            }

            node = node.nextSibling();
        }

        // Sort and build route from the temporary vector.
        newData->route().buildFromVector(route);
    }

    return TrackDataPtr(newData);
}
