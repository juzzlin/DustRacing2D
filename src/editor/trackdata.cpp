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

TrackData::TrackData(QString name, bool isUserTrack, size_t cols, size_t rows)
  : TrackDataBase(name, isUserTrack)
  , m_map(cols, rows)
{
}

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
        const auto object = std::dynamic_pointer_cast<Object>(*iter);
        const auto newObject = std::make_shared<Object>(*object);

        m_objects.add(ObjectBasePtr(newObject));
    }
}

void TrackData::copyRoute(const TrackData & other)
{
    m_route.clear();

    for (auto iter = other.m_route.cbegin(); iter != other.m_route.cend(); iter++)
    {
        const auto node = std::dynamic_pointer_cast<TargetNode>(*iter);
        const auto newNode = std::make_shared<TargetNode>(*node);

        m_route.push(newNode);
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

void TrackData::insertColumn(size_t at, MapBase::InsertDirection insertDirection)
{
    at = m_map.insertColumn(at, insertDirection);

    moveObjectsAfterColumnInsertion(at);

    moveTargetNodesAfterColumnInsertion(at);
}

void TrackData::insertRow(size_t at, MapBase::InsertDirection insertDirection)
{
    at = m_map.insertRow(at, insertDirection);

    moveObjectsAfterRowInsertion(at);

    moveTargetNodesAfterRowInsertion(at);
}

std::vector<TrackTileBasePtr> TrackData::deleteColumn(size_t at)
{
    moveObjectsAfterColumnDeletion(at);

    moveTargetNodesAfterColumnDeletion(at);

    return m_map.deleteColumn(at);
}

std::vector<TrackTileBasePtr> TrackData::deleteRow(size_t at)
{
    moveObjectsAfterRowDeletion(at);

    moveTargetNodesAfterRowDeletion(at);

    return m_map.deleteRow(at);
}

void TrackData::moveObjectsAfterColumnInsertion(size_t at)
{
    for (auto && object : m_objects)
    {
        if (object->location().x() > TrackTile::width() * at)
        {
            object->setLocation(QPointF(object->location().x() + TrackTile::width(), object->location().y()));
        }
    }
}

void TrackData::moveObjectsAfterRowInsertion(size_t at)
{
    for (auto && object : m_objects)
    {
        if (object->location().y() > TrackTile::height() * at)
        {
            object->setLocation(QPointF(object->location().x(), object->location().y() + TrackTile::height()));
        }
    }
}

void TrackData::moveTargetNodesAfterColumnInsertion(size_t at)
{
    for (auto && targetNode : m_route)
    {
        if (targetNode->location().x() > TrackTile::width() * at)
        {
            targetNode->setLocation(QPointF(targetNode->location().x() + TrackTile::width(), targetNode->location().y()));
        }
    }
}

void TrackData::moveTargetNodesAfterRowInsertion(size_t at)
{
    for (auto && targetNode : m_route)
    {
        if (targetNode->location().y() > TrackTile::height() * at)
        {
            targetNode->setLocation(QPointF(targetNode->location().x(), targetNode->location().y() + TrackTile::height()));
        }
    }
}

void TrackData::moveObjectsAfterColumnDeletion(size_t at)
{
    for (auto && object : m_objects)
    {
        if (object->location().x() > TrackTile::width() * at)
        {
            object->setLocation(QPointF(object->location().x() - TrackTile::width(), object->location().y()));
        }
    }
}

void TrackData::moveObjectsAfterRowDeletion(size_t at)
{
    for (auto && object : m_objects)
    {
        if (object->location().y() > TrackTile::height() * at)
        {
            object->setLocation(QPointF(object->location().x(), object->location().y() - TrackTile::height()));
        }
    }
}

void TrackData::moveTargetNodesAfterColumnDeletion(size_t at)
{
    for (auto && targetNode : m_route)
    {
        if (targetNode->location().x() > TrackTile::width() * at)
        {
            targetNode->setLocation(QPointF(targetNode->location().x() - TrackTile::width(), targetNode->location().y()));
        }
    }
}

void TrackData::moveTargetNodesAfterRowDeletion(size_t at)
{
    for (auto && targetNode : m_route)
    {
        if (targetNode->location().y() > TrackTile::height() * at)
        {
            targetNode->setLocation(QPointF(targetNode->location().x(), targetNode->location().y() - TrackTile::height()));
        }
    }
}

TrackData::~TrackData()
{
}
