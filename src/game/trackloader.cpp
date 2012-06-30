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

#include <QDir>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QDomDocument>
#include <QDomElement>

#include "layers.hpp"

#include "../common/targetnodebase.hpp"

#include "track.hpp"
#include "trackdata.hpp"
#include "trackloader.hpp"
#include "trackobject.hpp"
#include "tracktile.hpp"
#include "treeview.hpp"
#include "wall.hpp"

#include "MiniCore/Core/MCLogger"
#include "MiniCore/Core/MCObjectFactory"
#include "MiniCore/Core/MCTextureManager"

#include <cassert>

TrackLoader::TrackLoader(MCTextureManager & textureManager, MCObjectFactory  & objectFactory)
: m_textureManager(textureManager)
, m_objectFactory(objectFactory)
, m_paths()
, m_tracks()
{
}

void TrackLoader::addTrackSearchPath(QString path)
{
    if (!m_paths.contains(path))
    {
        m_paths << path;
    }
}

int TrackLoader::loadTracks()
{
    int numLoaded = 0;
    for (QString path : m_paths)
    {
        MCLogger().info() << "Loading tracks from '" << path.toStdString() << "'..";
        QStringList trackPaths(QDir(path).entryList(QStringList("*.trk")));
        for (QString trackPath : trackPaths)
        {
            MCLogger().info() << "Found '" << trackPath.toStdString() << "'..";
            trackPath = path + QDir::separator() + trackPath;
            if (TrackData * trackData = loadTrack(trackPath))
            {
                m_tracks << new Track(trackData);
                numLoaded++;
            }
            else
            {
                MCLogger().error() << "Couldn't load '" << trackPath.toStdString() << "'..";
            }
        }
    }
    return numLoaded;
}

TrackData * TrackLoader::loadTrack(QString path)
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

    TrackData * newData = nullptr;

    QDomElement root = doc.documentElement();
    if (root.nodeName() == "track")
    {
        const QString      name     = root.attribute("name", "undefined");
        const unsigned int cols     = root.attribute("cols", "0").toUInt();
        const unsigned int rows     = root.attribute("rows", "0").toUInt();
        const unsigned int lapCount = root.attribute("lapCount", "1").toUInt();

        if (cols > 0 && rows > 0)
        {
            newData = new TrackData(name, cols, rows);
            newData->setFileName(path);
            newData->setLapCount(lapCount);

            // A temporary route vector.
            std::vector<TargetNodeBase *> route;

            QDomNode node = root.firstChild();
            while(!node.isNull())
            {
                QDomElement element = node.toElement();
                if(!element.isNull())
                {
                    // Read a tile element
                    if (element.nodeName() == "tile")
                    {
                        readTile(element, *newData);
                    }
                    // Read an object element
                    else if (element.nodeName() == "object")
                    {
                        readObject(element, *newData);
                    }
                    // Read a target node element
                    else if (element.nodeName() == "tnode")
                    {
                        readTargetNode(element, *newData, route);
                    }
                }

                node = node.nextSibling();
            }

            newData->route().buildFromVector(route);
        }
    }

    return newData;
}

void TrackLoader::readTile(
    QDomElement & element, TrackData & newData)
{
    const std::string  id           = element.attribute("type", "clear").toStdString();
    const unsigned int computerHint = element.attribute("computerHint", "0").toUInt();

    // X-coordinate in the tile matrix
    unsigned int i = element.attribute("i", "0").toUInt();

    // Y-coordinate in the tile matrix
    unsigned int j = element.attribute("j", "0").toUInt();

    // Orientation angle in degrees.
    int o = element.attribute("o", "0").toInt();

    // Mirror the angle and y-index, because game has the
    // y-axis pointing up.
    o = -o;
    j = newData.map().rows() - 1 - j;

    TrackTile * tile = dynamic_cast<TrackTile *>(newData.map().getTile(i, j));
    assert(tile);

    tile->setRotation(o);
    tile->setTileType(id.c_str());
    tile->setTileTypeEnum(tileTypeEnumFromString(id.c_str()));
    tile->setComputerHint(static_cast<TrackTileBase::ComputerHint>(computerHint));

    // Associate with a surface object corresponging
    // to the tile type.
    // surface() throws if fails. Handled of higher level.
    tile->setSurface(&m_textureManager.surface(id));
}

TrackTile::TileType TrackLoader::tileTypeEnumFromString(std::string str)
{
    if (str == "corner90")
    {
        return TrackTile::TT_CORNER_90;
    }
    else if (str == "corner45Left")
    {
        return TrackTile::TT_CORNER_45_LEFT;
    }
    else if (str == "corner45Right")
    {
        return TrackTile::TT_CORNER_45_RIGHT;
    }
    else if (str == "straight")
    {
        return TrackTile::TT_STRAIGHT;
    }
    else if (str == "straight45Male")
    {
        return TrackTile::TT_STRAIGHT_45_MALE;
    }
    else if (str == "straight45Female")
    {
        return TrackTile::TT_STRAIGHT_45_FEMALE;
    }
    else if (str == "grass")
    {
        return TrackTile::TT_GRASS;
    }
    else if (str == "sand")
    {
        return TrackTile::TT_SAND;
    }
    else if (str == "sandGrassStraight")
    {
        return TrackTile::TT_SAND_GRASS_STRAIGHT;
    }
    else if (str == "sandGrassStraight45Female")
    {
        return TrackTile::TT_SAND_GRASS_STRAIGHT_45_FEMALE;
    }
    else if (str == "sandGrassCorner")
    {
        return TrackTile::TT_SAND_GRASS_CORNER;
    }
    else if (str == "sandGrassCorner2")
    {
        return TrackTile::TT_SAND_GRASS_CORNER_2;
    }
    else if (str == "finish")
    {
        return TrackTile::TT_FINISH;
    }
    else if (str == "clear")
    {
        return TrackTile::TT_NONE;
    }
    else
    {
        MCLogger().error() << "No mapping for tile '" << str << "'..";
    }

    return TrackTile::TT_NONE;
}

void TrackLoader::readObject(QDomElement & element, TrackData & newData)
{
    const QString role     = element.attribute("role", "");
    const QString category = element.attribute("category", "");

    // X-coordinate in the world
    const int x = element.attribute("x", "0").toInt();

    // Y-coordinate in the world
    const int y = element.attribute("y", "0").toInt();

    // Angle in degrees
    int o = element.attribute("o", "0").toInt();

    // Mirror the angle, because game has y-axis pointing up.
    o = -o;

    // Height of the map. The y-coordinates needs to be mirrored, because
    // the coordinate system is y-wise mirrored in the editor.
    const int h = newData.map().rows() * TrackTile::TILE_H;

    // TODO: A separate config file for these
    if (role == "brake")
    {
        MCSurfaceObjectData data("brake");
        data.setMass(1000);
        data.setSurfaceId("brake");
        data.setRestitution(0.5);
        data.setXYFriction(1.0);
        data.setBatchMode(true);

        MCObject & object = m_objectFactory.build(data);
        object.setInitialLocation(
            MCVector2d<MCFloat>(x, h - y));
        object.setInitialAngle(o);

        // Wrap the MCObject in a TrackObject and add to
        // the TrackData
        newData.objects().add(*new TrackObject(category, role, object), true);
    }
    else if (role == "tire")
    {
        MCSurfaceObjectData data("tire");
        data.setMass(1000); // Exaggerate the mass on purpose
        data.setSurfaceId("tire");
        data.setRestitution(0.5);
        data.setXYFriction(0.25);
        data.setBatchMode(true);

        MCObject & object = m_objectFactory.build(data);
        object.setInitialLocation(
            MCVector2d<MCFloat>(x, h - y));
        object.setInitialAngle(o);

        // Wrap the MCObject in a TrackObject and add to
        // the TrackData
        newData.objects().add(*new TrackObject(category, role, object), true);
    }
    else if (role == "tree")
    {
        const MCFloat treeViewRadius = 32;
        const MCFloat treeBodyRadius = 8;

        MCSurfaceObjectData data("tree");
        data.setStationary(true);
        data.setRestitution(0.25);
        data.setShapeWidth(treeBodyRadius);
        data.setShapeHeight(treeBodyRadius);
        data.setLayer(Layers::Tree);

        // Create a custom view.
        TreeView * view = new TreeView(
            m_textureManager.surface("tree"), treeViewRadius, 2, 90, 5);
        MCObject & object = m_objectFactory.build(data, *view);
        object.setInitialLocation(
            MCVector2d<MCFloat>(x, h - y));
        object.setInitialAngle(o);

        // Wrap the MCObject in a TrackObject and add to
        // the TrackData
        newData.objects().add(*new TrackObject(category, role, object), true);
    }
    else if (role == "wall")
    {
        const int wallSize = 16;
        MCObject * object = new Wall(
            m_textureManager.surface("wall"), x, h - y, wallSize, wallSize, wallSize);
        object->setLayer(Layers::Walls, false);
        object->setInitialLocation(MCVector3dF(x, h - y, 0));
        object->setInitialAngle(o);
        object->setMass(0, true); // stationary
        object->setXYFriction(0.5);
        object->setDamping(0.9);

        // Wrap the MCObject in a TrackObject and add to
        // the TrackData
        newData.objects().add(*new TrackObject(category, role, *object), true);
    }
}

void TrackLoader::readTargetNode(
    QDomElement & element, TrackData & newData, std::vector<TargetNodeBase *> & route)
{
    const int x = element.attribute("x", "0").toInt();
    const int y = element.attribute("y", "0").toInt();
    const int i = element.attribute("i", "0").toInt();

    // Height of the map. The y-coordinates needs to be mirrored, because
    // the coordinate system is y-wise mirrored in the editor.
    const int h = newData.map().rows() * TrackTile::TILE_H;

    TargetNodeBase * tnode = new TargetNodeBase;
    tnode->setIndex(i);
    tnode->setLocation(QPointF(x, h - y));
    route.push_back(tnode);
}

unsigned int TrackLoader::tracks() const
{
    return m_tracks.size();
}

Track * TrackLoader::track(unsigned int index) const
{
    if (index < tracks())
    {
        return m_tracks.at(index);
    }

    return nullptr;
}

TrackLoader::~TrackLoader()
{
    for (Track * track : m_tracks)
    {
        delete track;
    }

    m_tracks.clear();
}
