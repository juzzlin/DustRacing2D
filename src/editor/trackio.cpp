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
#include "../common/datakeywords.hpp"
#include "../common/objectbase.hpp"
#include "../common/targetnodebase.hpp"

#include <cassert>
#include <memory>

namespace {

void readTile(TrackData & newData, const QDomElement & element)
{
    // Init a new tile. QGraphicsScene will take
    // the ownership eventually.
    const auto tile = std::dynamic_pointer_cast<TrackTile>(newData.map().getTile(
      element.attribute(DataKeywords::Tile::i, "0").toUInt(),
      element.attribute(DataKeywords::Tile::j, "0").toUInt()));
    assert(tile);

    tile->setRotation(element.attribute(DataKeywords::Tile::orientation, "0").toInt());

    tile->setTileType(element.attribute(DataKeywords::Tile::type, "clear"));

    tile->setComputerHint(
      static_cast<TrackTileBase::ComputerHint>(element.attribute(DataKeywords::Tile::computerHint, "0").toInt()));

    tile->setExcludeFromMinimap(element.attribute(DataKeywords::Tile::excludeFromMinimap, "0").toUInt());
}

void readObject(TrackData & newData, const QDomElement & element)
{
    // Create a new object. QGraphicsScene will take
    // the ownership eventually.
    auto object = std::make_shared<Object>(ObjectFactory::createObject(element.attribute(DataKeywords::Object::role, "clear")));

    object->setLocation(QPointF(
      element.attribute(DataKeywords::Object::x, "0").toInt(),
      element.attribute(DataKeywords::Object::y, "0").toInt()));

    object->setRotation(element.attribute(DataKeywords::Object::orientation, "0").toInt());

    object->setForceStationary(element.attribute(DataKeywords::Object::forceStationary, "0").toUInt());

    newData.objects().add(object);
}

void readTargetNode(std::vector<TargetNodeBasePtr> & route, const QDomElement & element)
{
    // Create a new object. QGraphicsScene will take
    // the ownership eventually.
    const auto targetNode = std::make_shared<TargetNode>();
    targetNode->setIndex(element.attribute(DataKeywords::Node::index, "0").toInt());

    const int x = element.attribute(DataKeywords::Node::x, "0").toInt();
    const int y = element.attribute(DataKeywords::Node::y, "0").toInt();

    targetNode->setLocation(QPointF(x, y));

    const int w = element.attribute(DataKeywords::Node::width, "0").toInt();
    const int h = element.attribute(DataKeywords::Node::height, "0").toInt();

    if (w > 0 && h > 0)
    {
        targetNode->setSize(QSizeF(w, h));
    }

    route.push_back(targetNode);
}

void writeTiles(const TrackDataPtr trackData, QDomElement & root, QDomDocument & doc)
{
    for (size_t i = 0; i < trackData->map().cols(); i++)
    {
        for (size_t j = 0; j < trackData->map().rows(); j++)
        {
            auto && tileElement = doc.createElement(DataKeywords::Track::tile);
            const auto tile = std::dynamic_pointer_cast<TrackTile>(trackData->map().getTile(i, j));
            tileElement.setAttribute(DataKeywords::Tile::type, tile->tileType());
            tileElement.setAttribute(DataKeywords::Tile::i, static_cast<int>(i));
            tileElement.setAttribute(DataKeywords::Tile::j, static_cast<int>(j));
            tileElement.setAttribute(DataKeywords::Tile::orientation, tile->rotation());

            if (tile->excludeFromMinimap())
            {
                tileElement.setAttribute(DataKeywords::Tile::excludeFromMinimap, true);
            }

            if (tile->computerHint() != TrackTile::ComputerHint::None)
            {
                tileElement.setAttribute(DataKeywords::Tile::computerHint, static_cast<int>(tile->computerHint()));
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

        auto && objectElement = doc.createElement(DataKeywords::Track::object);
        objectElement.setAttribute(DataKeywords::Object::category, object->category());
        objectElement.setAttribute(DataKeywords::Object::role, object->role());
        objectElement.setAttribute(DataKeywords::Object::x, static_cast<int>(object->location().x()));
        objectElement.setAttribute(DataKeywords::Object::y, static_cast<int>(object->location().y()));
        objectElement.setAttribute(DataKeywords::Object::orientation, static_cast<int>(object->rotation()));

        if (object->forceStationary())
        {
            objectElement.setAttribute(DataKeywords::Object::forceStationary, static_cast<int>(object->forceStationary()));
        }

        root.appendChild(objectElement);
    }
}

void writeTargetNodes(TrackDataPtr trackData, QDomElement & root, QDomDocument & doc)
{
    for (auto && targetNode : trackData->route())
    {
        auto && targetNodeElement = doc.createElement(DataKeywords::Track::node);
        targetNodeElement.setAttribute(DataKeywords::Node::index, targetNode->index());
        targetNodeElement.setAttribute(DataKeywords::Node::x, static_cast<int>(targetNode->location().x()));
        targetNodeElement.setAttribute(DataKeywords::Node::y, static_cast<int>(targetNode->location().y()));
        targetNodeElement.setAttribute(DataKeywords::Node::width, static_cast<int>(targetNode->size().width()));
        targetNodeElement.setAttribute(DataKeywords::Node::height, static_cast<int>(targetNode->size().height()));

        root.appendChild(targetNodeElement);
    }
}

} // namespace

bool TrackIO::save(TrackDataPtr trackData, QString path)
{
    // Create content
    QDomDocument doc;
    doc.appendChild(doc.createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'"));

    auto && root = doc.createElement(DataKeywords::Header::track);
    root.setAttribute(DataKeywords::Header::version, Config::Editor::EDITOR_VERSION);
    root.setAttribute(DataKeywords::Header::name, trackData->name());
    root.setAttribute(DataKeywords::Header::cols, static_cast<int>(trackData->map().cols()));
    root.setAttribute(DataKeywords::Header::rows, static_cast<int>(trackData->map().rows()));
    root.setAttribute(DataKeywords::Header::index, trackData->index());

    if (trackData->isUserTrack()) // Don't add the attribute at all, if not set
    {
        root.setAttribute(DataKeywords::Header::user, true);
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

    auto && root = doc.documentElement();
    const size_t cols = root.attribute(DataKeywords::Header::cols, "0").toUInt();
    const size_t rows = root.attribute(DataKeywords::Header::rows, "0").toUInt();

    TrackDataPtr newData;
    if (cols && rows)
    {
        const auto name = root.attribute(DataKeywords::Header::name, "undefined");
        const bool isUserTrack = root.attribute(DataKeywords::Header::user, "0").toInt();
        newData = std::make_shared<TrackData>(name, isUserTrack, cols, rows);
        newData->setFileName(path);
        newData->setIndex(root.attribute(DataKeywords::Header::index, "0").toUInt());

        // Temporary route vector.
        std::vector<TargetNodeBasePtr> route;

        auto && node = root.firstChild();
        while (!node.isNull())
        {
            auto element = node.toElement();
            if (!element.isNull())
            {
                // Read a tile element
                if (element.nodeName() == DataKeywords::Track::tile)
                {
                    readTile(*newData, element);
                }
                // Read an object element
                else if (element.nodeName() == DataKeywords::Track::object)
                {
                    readObject(*newData, element);
                }
                // Read a target node element
                else if (element.nodeName() == DataKeywords::Track::node)
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
