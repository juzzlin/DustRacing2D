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

#include "trackdata.hpp"

#include "targetnode.hpp"
#include "tracktile.hpp"

#include "object.hpp"
#include "objectfactory.hpp"

#include <memory>

TrackData::TrackData(QString name, bool isUserTrack, unsigned int cols, unsigned int rows)
    : TrackDataBase(name, isUserTrack)
    , m_map(cols, rows)
{}

TrackData::TrackData(const TrackData & other)
    : TrackDataBase(other)
    , m_fileName(other.m_fileName)
    , m_map(other.m_map)
{
    copyObjects(other);

    copyRoute(other);
}

void TrackData::copyObjects(const TrackData & other)
{
    m_objects.clear();

    for (auto iter = other.m_objects.cbegin(); iter != other.m_objects.cend(); iter++)
    {
        auto object = std::dynamic_pointer_cast<Object>(*iter);
        auto newObject = new Object(*(object.get()));

        m_objects.add(ObjectBasePtr(newObject));
    }
}

void TrackData::copyRoute(const TrackData & other)
{
    m_route.clear();

    for (auto iter = other.m_route.cbegin(); iter != other.m_route.cend(); iter++)
    {
        auto node = std::dynamic_pointer_cast<TargetNode>(*iter);
        auto newNode = new TargetNode(*(node.get()));

        m_route.push(TargetNodeBasePtr(newNode));
    }
}

QString TrackData::fileName() const
{
    return m_fileName;
}

void TrackData::setFileName(QString newFileName)
{
    m_fileName = newFileName;
}

Route & TrackData::route()
{
    return m_route;
}

const Route & TrackData::route() const
{
    return m_route;
}

MapBase & TrackData::map()
{
    return m_map;
}

const MapBase & TrackData::map() const
{
    return m_map;
}

Objects & TrackData::objects()
{
    return m_objects;
}

const Objects & TrackData::objects() const
{
    return m_objects;
}

void TrackData::insertColumn(unsigned int at, MapBase::InsertDirection insertDirection)
{
    at = m_map.insertColumn(at, insertDirection);

    moveObjectsAfterColumnInsertion(at);

    moveTargetNodesAfterColumnInsertion(at);
}

void TrackData::insertRow(unsigned int at, MapBase::InsertDirection insertDirection)
{
    at = m_map.insertRow(at, insertDirection);

    moveObjectsAfterRowInsertion(at);

    moveTargetNodesAfterRowInsertion(at);
}

std::vector<TrackTileBasePtr> TrackData::deleteColumn(unsigned int at)
{
    moveObjectsAfterColumnDeletion(at);

    moveTargetNodesAfterColumnDeletion(at);

    return m_map.deleteColumn(at);
}

std::vector<TrackTileBasePtr> TrackData::deleteRow(unsigned int at)
{
    moveObjectsAfterRowDeletion(at);

    moveTargetNodesAfterRowDeletion(at);

    return m_map.deleteRow(at);
}

void TrackData::moveObjectsAfterColumnInsertion(unsigned int at)
{
    for (auto object : m_objects)
    {
        if (object->location().x() > TrackTile::TILE_W * at)
        {
            object->setLocation(QPointF(object->location().x() + TrackTile::TILE_W, object->location().y()));
        }
    }
}

void TrackData::moveObjectsAfterRowInsertion(unsigned int at)
{
    for (auto object : m_objects)
    {
        if (object->location().y() > TrackTile::TILE_H * at)
        {
            object->setLocation(QPointF(object->location().x(), object->location().y() + TrackTile::TILE_H));
        }
    }
}

void TrackData::moveTargetNodesAfterColumnInsertion(unsigned int at)
{
    for (auto tnode : m_route)
    {
        if (tnode->location().x() > TrackTile::TILE_W * at)
        {
            tnode->setLocation(QPointF(tnode->location().x() + TrackTile::TILE_W, tnode->location().y()));
        }
    }
}

void TrackData::moveTargetNodesAfterRowInsertion(unsigned int at)
{
    for (auto tnode : m_route)
    {
        if (tnode->location().y() > TrackTile::TILE_H * at)
        {
            tnode->setLocation(QPointF(tnode->location().x(), tnode->location().y() + TrackTile::TILE_H));
        }
    }
}

void TrackData::moveObjectsAfterColumnDeletion(unsigned int at)
{
    for (auto object : m_objects)
    {
        if (object->location().x() > TrackTile::TILE_W * at)
        {
            object->setLocation(QPointF(object->location().x() - TrackTile::TILE_W, object->location().y()));
        }
    }
}

void TrackData::moveObjectsAfterRowDeletion(unsigned int at)
{
    for (auto object : m_objects)
    {
        if (object->location().y() > TrackTile::TILE_H * at)
        {
            object->setLocation(QPointF(object->location().x(), object->location().y() - TrackTile::TILE_H));
        }
    }
}

void TrackData::moveTargetNodesAfterColumnDeletion(unsigned int at)
{
    for (auto tnode : m_route)
    {
        if (tnode->location().x() > TrackTile::TILE_W * at)
        {
            tnode->setLocation(QPointF(tnode->location().x() - TrackTile::TILE_W, tnode->location().y()));
        }
    }
}

void TrackData::moveTargetNodesAfterRowDeletion(unsigned int at)
{
    for (auto tnode : m_route)
    {
        if (tnode->location().y() > TrackTile::TILE_H * at)
        {
            tnode->setLocation(QPointF(tnode->location().x(), tnode->location().y() - TrackTile::TILE_H));
        }
    }
}

TrackData::~TrackData()
{
}
