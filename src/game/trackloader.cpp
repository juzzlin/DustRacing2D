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
#include "../common/datakeywords.hpp"

#include "database.hpp"
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
    Config::General::dataPath,
    (std::string(Config::General::dataPath) + QDir::separator().toLatin1() + std::string("surfaces.conf")),
    "",
    (std::string(Config::General::dataPath) + QDir::separator().toLatin1() + std::string("meshes.conf")))
  , m_objectFactory(m_assetManager)
  , m_trackObjectFactory(m_objectFactory)
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
    for (auto && path : m_paths)
    {
        juzzlin::L().info() << "Loading race tracks from '" << path.toStdString() << "'..";
        auto trackPaths(QDir(path).entryList(QStringList("*.trk")));
        for (auto && trackPath : trackPaths)
        {
            trackPath = path + QDir::separator() + trackPath;
            if (auto trackData = loadTrack(trackPath))
            {
                juzzlin::L().info() << "  Found '" << trackPath.toStdString() << "', index=" << trackData->index();
                m_tracks.push_back(std::make_shared<Track>(std::move(trackData)));
                numLoaded++;
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

    bool firstOfficialTrackUnlocked = false;

    // Check if the tracks are locked/unlocked.
    for (auto && track : m_tracks)
    {
        if (!track->trackData().isUserTrack() && !Database::instance().loadTrackUnlockStatus(*track, lapCount, difficulty))
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
            const auto bestPos = Database::instance().loadBestPos(*track, lapCount, difficulty);
            if (bestPos.second && bestPos.first >= 1 && bestPos.first <= UNLOCK_LIMIT)
            {
                auto && next = track->next().lock();
                if (next)
                {
                    next->trackData().setIsLocked(false);
                    Database::instance().saveTrackUnlockStatus(*next, lapCount, difficulty);
                }
            }
        }

        // Always unlock the first official track
        if (!track->trackData().isUserTrack() && !firstOfficialTrackUnlocked)
        {
            track->trackData().setIsLocked(false);
            firstOfficialTrackUnlocked = true;
        }
    }
}

void TrackLoader::sortTracks()
{
    // Sort tracks with respect to their indices. Move user tracks to the
    // beginning of the track array.
    std::stable_sort(m_tracks.begin(), m_tracks.end(),
                     [](auto lhs, auto rhs) -> bool {
                         const int left = lhs->trackData().isUserTrack() ? -1 : static_cast<int>(lhs->trackData().index());
                         return left < static_cast<int>(rhs->trackData().index());
                     });

    // Cross-link the tracks
    for (size_t i = 0; i + 1 < m_tracks.size(); i++)
    {
        m_tracks[i]->setNext(m_tracks[i + 1]);
        m_tracks[i + 1]->setPrev(m_tracks[i]);
    }
}

std::unique_ptr<TrackData> TrackLoader::loadTrack(QString path)
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

    std::unique_ptr<TrackData> newData;
    const auto root = doc.documentElement();
    if (root.nodeName() == "track")
    {
        const auto cols = root.attribute(DataKeywords::Header::cols, "0").toUInt();
        const auto rows = root.attribute(DataKeywords::Header::rows, "0").toUInt();

        if (cols > 0 && rows > 0)
        {
            const auto name = root.attribute(DataKeywords::Header::name, "undefined");
            const bool isUserTrack = root.attribute(DataKeywords::Header::user, "0").toUInt();

            newData = std::make_unique<TrackData>(name, isUserTrack, cols, rows);
            newData->setFileName(path);
            newData->setIndex(root.attribute(DataKeywords::Header::index, "999").toUInt());

            // A temporary route vector.
            std::vector<TargetNodeBasePtr> route;

            auto node = root.firstChild();
            while (!node.isNull())
            {
                auto element = node.toElement();
                if (!element.isNull())
                {
                    // Read a tile element
                    if (element.nodeName() == DataKeywords::Track::tile)
                    {
                        readTile(element, *newData);
                    }
                    // Read an object element
                    else if (element.nodeName() == DataKeywords::Track::object)
                    {
                        readObject(element, *newData);
                    }
                    // Read a target node element
                    else if (element.nodeName() == DataKeywords::Track::node)
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

void TrackLoader::readTile(QDomElement & element, TrackData & newData)
{
    // X-coordinate in the tile matrix
    const auto i = element.attribute(DataKeywords::Tile::i, "0").toUInt();

    // Mirror the y-index, because game has the y-axis pointing up.
    const auto j = newData.map().rows() - 1 - element.attribute(DataKeywords::Tile::j, "0").toUInt();

    const auto tile = dynamic_pointer_cast<TrackTile>(newData.map().getTile(i, j));
    assert(tile);

    const auto type = element.attribute(DataKeywords::Tile::type, "clear").toStdString();
    tile->setTileType(type.c_str());
    tile->setTileTypeEnum(tileTypeEnumFromString(type.c_str()));

    // Mirror the angle, because game has the y-axis pointing up.
    const int o = -element.attribute("o", "0").toInt();
    tile->setRotation(o);
    tile->setComputerHint(
      static_cast<TrackTileBase::ComputerHint>(element.attribute(DataKeywords::Tile::computerHint, "0").toUInt()));
    tile->setExcludeFromMinimap(element.attribute(DataKeywords::Tile::excludeFromMinimap, "0").toInt());

    // Associate with a surface object corresponging
    // to the tile type.
    // surface() throws if fails. Handled of higher level.
    tile->setSurface(MCAssetManager::surfaceManager().surface(type));

    // Set preview surface, if found.
    try
    {
        tile->setPreviewSurface(MCAssetManager::surfaceManager().surface(type + "Preview"));
    } catch (...)
    {
        // Don't care
    }
}

TrackTile::TileType TrackLoader::tileTypeEnumFromString(std::string str)
{
    static std::map<string, TrackTile::TileType> mappings = {
        { "bridge", TrackTile::TileType::Bridge },
        { "corner90", TrackTile::TileType::Corner90 },
        { "corner45Left", TrackTile::TileType::Corner45Left },
        { "corner45Right", TrackTile::TileType::Corner45Right },
        { "straight", TrackTile::TileType::Straight },
        { "straight45Male", TrackTile::TileType::Straight45Male },
        { "straight45Female", TrackTile::TileType::Straight45Female },
        { "grass", TrackTile::TileType::Grass },
        { "sand", TrackTile::TileType::Sand },
        { "sandGrassStraight", TrackTile::TileType::SandGrassStraight },
        { "sandGrassCorner", TrackTile::TileType::SandGrassCorner },
        { "sandGrassCorner2", TrackTile::TileType::SandGrassCorner2 },
        { "finish", TrackTile::TileType::Finish },
        { "clear", TrackTile::TileType::None }
    };

    if (!mappings.count(str))
    {
        juzzlin::L().error() << "No mapping for tile '" << str << "'..";
        return TrackTile::TileType::None;
    }

    return mappings[str];
}

void TrackLoader::readObject(QDomElement & element, TrackData & newData)
{
    // X-coordinate in the world
    const int x = element.attribute(DataKeywords::Object::x, "0").toInt();

    // Y-coordinate in the world
    const int y = element.attribute(DataKeywords::Object::y, "0").toInt();

    // Height of the map.
    const int h = static_cast<int>(newData.map().rows() * TrackTile::height());

    // The y-coordinates needs to be mirrored, because the y-axis is pointing
    // down in the editor's coordinate system.
    const MCVector2dF location(x, h - y);

    // Mirror the angle, because the y-axis is pointing
    // down in the editor's coordinate system.
    const int angle = -element.attribute(DataKeywords::Object::orientation, "0").toInt();

    const auto category = element.attribute(DataKeywords::Object::category, "");
    const auto role = element.attribute(DataKeywords::Object::role, "");
    const bool forceStationary = element.attribute(DataKeywords::Object::forceStationary, "0").toUInt();

    if (const auto object = m_trackObjectFactory.build(category, role, location, angle, forceStationary))
    {
        newData.objects().add(std::shared_ptr<TrackObject>(object));
    }
}

void TrackLoader::readTargetNode(QDomElement & element, TrackData & newData, std::vector<TargetNodeBasePtr> & route)
{
    // Height of the map. The y-coordinates needs to be mirrored, because
    // the coordinate system is y-wise mirrored in the editor.
    const int mapHeight = static_cast<int>(newData.map().rows() * TrackTile::height());

    const auto targetNode = std::make_shared<TargetNodeBase>();
    targetNode->setIndex(element.attribute(DataKeywords::Node::index, "0").toInt());

    const int x = element.attribute(DataKeywords::Node::x, "0").toInt();
    const int y = element.attribute(DataKeywords::Node::y, "0").toInt();

    targetNode->setLocation(QPointF(x, mapHeight - y));

    const int w = element.attribute(DataKeywords::Node::width, "0").toInt();
    const int h = element.attribute(DataKeywords::Node::height, "0").toInt();

    if (w > 0 && h > 0)
    {
        targetNode->setSize(QSizeF(w, h));
    }

    route.push_back(targetNode);
}

size_t TrackLoader::tracks() const
{
    return m_tracks.size();
}

std::shared_ptr<Track> TrackLoader::track(size_t index) const
{
    if (index < tracks())
    {
        return m_tracks.at(index);
    }

    return nullptr;
}
