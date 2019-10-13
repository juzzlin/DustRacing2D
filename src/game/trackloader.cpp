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

#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QStringList>
#include <QTextStream>

#include "../common/config.hpp"
#include "layers.hpp"
#include "renderer.hpp"
#include "settings.hpp"
#include "track.hpp"
#include "trackdata.hpp"
#include "trackloader.hpp"
#include "trackobject.hpp"
#include "tracktile.hpp"

#include <MCAssetManager>
#include <MCObjectFactory>
#include <MCShapeView>

#include "simple_logger.hpp"

#include <algorithm>
#include <cassert>
#include <memory>

using std::dynamic_pointer_cast;

static const int UNLOCK_LIMIT = 6; // Position required to unlock a new track

TrackLoader * TrackLoader::m_instance = nullptr;

TrackLoader::TrackLoader()
  : m_assetManager(
    Config::Common::dataPath,
    (std::string(Config::Common::dataPath) + QDir::separator().toLatin1() + std::string("surfaces.conf")),
    "",
    (std::string(Config::Common::dataPath) + QDir::separator().toLatin1() + std::string("meshes.conf")))
  , m_objectFactory(m_assetManager)
  , m_trackObjectFactory(m_objectFactory)
  , m_paths()
  , m_tracks()
{
    assert(!TrackLoader::m_instance);
    TrackLoader::m_instance = this;
}

TrackLoader & TrackLoader::instance()
{
    assert(TrackLoader::m_instance);
    return *m_instance;
}

void TrackLoader::addTrackSearchPath(QString path)
{
    m_paths.push_back(path);
}

void TrackLoader::loadAssets()
{
    m_assetManager.load();
}

int TrackLoader::loadTracks(int lapCount, DifficultyProfile::Difficulty difficulty)
{
    int numLoaded = 0;
    for (QString path : m_paths)
    {
        juzzlin::L().info() << "Loading race tracks from '" << path.toStdString() << "'..";
        QStringList trackPaths(QDir(path).entryList(QStringList("*.trk")));
        for (QString trackPath : trackPaths)
        {
            trackPath = path + QDir::separator() + trackPath;
            if (TrackData * trackData = loadTrack(trackPath))
            {
                m_tracks.push_back(new Track(trackData));
                numLoaded++;

                juzzlin::L().info() << "  Found '" << trackPath.toStdString() << "', index="
                                    << trackData->index();
            }
            else
            {
                juzzlin::L().error() << "Couldn't load '" << trackPath.toStdString() << "'..";
            }
        }

        if (!trackPaths.size())
        {
            juzzlin::L().info() << "  No race tracks found.";
        }
    }

    if (numLoaded)
    {
        updateLockedTracks(lapCount, difficulty);
    }

    return numLoaded;
}

void TrackLoader::updateLockedTracks(int lapCount, DifficultyProfile::Difficulty difficulty)
{
    sortTracks();

    Track * firstOfficialTrack = nullptr;

    // Check if the tracks are locked/unlocked.
    for (Track * track : m_tracks)
    {
        if (!track->trackData().isUserTrack() && !Settings::instance().loadTrackUnlockStatus(*track, lapCount, difficulty))
        {
#ifndef UNLOCK_ALL_TRACKS
            track->trackData().setIsLocked(true);
#else
            track->trackData().setIsLocked(false);
#endif
        }
        else
        {
            track->trackData().setIsLocked(false);

            // This is needed in the case new tracks are added to the game afterwards.
            const int bestPos = Settings::instance().loadBestPos(*track, lapCount, difficulty);
            if (bestPos >= 1 && bestPos <= UNLOCK_LIMIT)
            {
                if (track->next())
                {
                    track->next()->trackData().setIsLocked(false);
                    Settings::instance().saveTrackUnlockStatus(*track->next(), lapCount, difficulty);
                }
            }
        }

        // Always unlock the first official track
        if (!track->trackData().isUserTrack() && !firstOfficialTrack)
        {
            firstOfficialTrack = track;
            firstOfficialTrack->trackData().setIsLocked(false);
        }
    }
}

void TrackLoader::sortTracks()
{
    // Sort tracks with respect to their indices. Move user tracks to the
    // beginning of the track array.
    std::stable_sort(m_tracks.begin(), m_tracks.end(),
                     [](Track * lhs, Track * rhs) -> bool {
                         const int left = lhs->trackData().isUserTrack() ? -1 : lhs->trackData().index();
                         return left < static_cast<int>(rhs->trackData().index());
                     });

    // Cross-link the tracks
    for (unsigned int i = 0; i + 1 < m_tracks.size(); i++)
    {
        m_tracks[i]->setNext(*m_tracks[i + 1]);
        m_tracks[i + 1]->setPrev(*m_tracks[i]);
    }
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
        const unsigned int cols =
          root.attribute(TrackDataBase::DataKeywords::Header::cols, "0").toUInt();
        const unsigned int rows =
          root.attribute(TrackDataBase::DataKeywords::Header::rows, "0").toUInt();

        if (cols > 0 && rows > 0)
        {
            const QString name =
              root.attribute(TrackDataBase::DataKeywords::Header::name, "undefined");

            const bool isUserTrack =
              root.attribute(TrackDataBase::DataKeywords::Header::user, "0").toUInt();

            newData = new TrackData(name, isUserTrack, cols, rows);
            newData->setFileName(path);

            newData->setIndex(root.attribute(TrackDataBase::DataKeywords::Header::index, "999").toUInt());

            // A temporary route vector.
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
                        readTile(element, *newData);
                    }
                    // Read an object element
                    else if (element.nodeName() == TrackDataBase::DataKeywords::Track::object)
                    {
                        readObject(element, *newData);
                    }
                    // Read a target node element
                    else if (element.nodeName() == TrackDataBase::DataKeywords::Track::node)
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
    // X-coordinate in the tile matrix
    const unsigned int i = element.attribute(TrackDataBase::DataKeywords::Tile::i, "0").toUInt();

    // Mirror the y-index, because game has the y-axis pointing up.
    const unsigned int j = newData.map().rows() - 1 - element.attribute(TrackDataBase::DataKeywords::Tile::j, "0").toUInt();

    auto tile = dynamic_pointer_cast<TrackTile>(newData.map().getTile(i, j));
    assert(tile);

    const std::string type =
      element.attribute(TrackDataBase::DataKeywords::Tile::type, "clear").toStdString();
    tile->setTileType(type.c_str());
    tile->setTileTypeEnum(tileTypeEnumFromString(type.c_str()));

    // Mirror the angle, because game has the y-axis pointing up.
    const int o = -element.attribute("o", "0").toInt();
    tile->setRotation(o);

    tile->setComputerHint(
      static_cast<TrackTileBase::ComputerHint>(element.attribute(TrackDataBase::DataKeywords::Tile::computerHint, "0").toUInt()));

    tile->setExcludeFromMinimap(element.attribute(TrackDataBase::DataKeywords::Tile::excludeFromMinimap, "0").toInt());

    // Associate with a surface object corresponging
    // to the tile type.
    // surface() throws if fails. Handled of higher level.
    tile->setSurface(&MCAssetManager::surfaceManager().surface(type));

    // Set preview surface, if found.
    try
    {
        tile->setPreviewSurface(&MCAssetManager::surfaceManager().surface(type + "Preview"));
    } catch (...)
    {
        // Don't care
    }
}

TrackTile::TileType TrackLoader::tileTypeEnumFromString(std::string str)
{
    static std::map<string, TrackTile::TileType> mappings = {
        { "bridge", TrackTile::TT_BRIDGE },
        { "corner90", TrackTile::TT_CORNER_90 },
        { "corner45Left", TrackTile::TT_CORNER_45_LEFT },
        { "corner45Right", TrackTile::TT_CORNER_45_RIGHT },
        { "straight", TrackTile::TT_STRAIGHT },
        { "straight45Male", TrackTile::TT_STRAIGHT_45_MALE },
        { "straight45Female", TrackTile::TT_STRAIGHT_45_FEMALE },
        { "grass", TrackTile::TT_GRASS },
        { "sand", TrackTile::TT_SAND },
        { "sandGrassStraight", TrackTile::TT_SAND_GRASS_STRAIGHT },
        { "sandGrassCorner", TrackTile::TT_SAND_GRASS_CORNER },
        { "sandGrassCorner2", TrackTile::TT_SAND_GRASS_CORNER_2 },
        { "finish", TrackTile::TT_FINISH },
        { "clear", TrackTile::TT_NONE }
    };

    if (!mappings.count(str))
    {
        juzzlin::L().error() << "No mapping for tile '" << str << "'..";
        return TrackTile::TT_NONE;
    }

    return mappings[str];
}

void TrackLoader::readObject(QDomElement & element, TrackData & newData)
{
    // X-coordinate in the world
    const int x = element.attribute(TrackDataBase::DataKeywords::Object::x, "0").toInt();

    // Y-coordinate in the world
    const int y = element.attribute(TrackDataBase::DataKeywords::Object::y, "0").toInt();

    // Height of the map.
    const int h = newData.map().rows() * TrackTile::TILE_H;

    // The y-coordinates needs to be mirrored, because the y-axis is pointing
    // down in the editor's coordinate system.
    const MCVector2dF location(x, h - y);

    // Mirror the angle, because the y-axis is pointing
    // down in the editor's coordinate system.
    const int angle = -element.attribute(TrackDataBase::DataKeywords::Object::orientation, "0").toInt();

    const QString category = element.attribute(TrackDataBase::DataKeywords::Object::category, "");
    const QString role = element.attribute(TrackDataBase::DataKeywords::Object::role, "");

    const bool forceStationary = element.attribute(TrackDataBase::DataKeywords::Object::forceStationary, "0").toUInt();

    if (TrackObject * object = m_trackObjectFactory.build(category, role, location, angle, forceStationary))
    {
        newData.objects().add(std::shared_ptr<TrackObject>(object));
    }
}

void TrackLoader::readTargetNode(QDomElement & element, TrackData & newData, std::vector<TargetNodeBasePtr> & route)
{
    // Height of the map. The y-coordinates needs to be mirrored, because
    // the coordinate system is y-wise mirrored in the editor.
    const int mapHeight = newData.map().rows() * TrackTile::TILE_H;

    TargetNodeBase * tnode = new TargetNodeBase;
    tnode->setIndex(element.attribute(TrackDataBase::DataKeywords::Node::index, "0").toInt());

    const int x = element.attribute(TrackDataBase::DataKeywords::Node::x, "0").toInt();
    const int y = element.attribute(TrackDataBase::DataKeywords::Node::y, "0").toInt();

    tnode->setLocation(QPointF(x, mapHeight - y));

    const int w = element.attribute(TrackDataBase::DataKeywords::Node::width, "0").toInt();
    const int h = element.attribute(TrackDataBase::DataKeywords::Node::height, "0").toInt();

    if (w > 0 && h > 0)
    {
        tnode->setSize(QSizeF(w, h));
    }

    route.push_back(TargetNodeBasePtr(tnode));
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
