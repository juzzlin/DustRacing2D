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

#ifndef TRACKLOADER_HPP
#define TRACKLOADER_HPP

#include <QString>
#include <vector>

#include <MCAssetManager>
#include <MCObjectFactory>

#include "difficultyprofile.hpp"
#include "tracktile.hpp"
#include "trackobjectfactory.hpp"

#include "../common/targetnodebase.hpp"

class TargetNodeBase;
class Track;
class TrackData;
class TrackTileBase;
class QDomElement;

//! A singleton class that handles track loading.
//! TODO: This can be shared with the editor or inherit from
//! a shared loader.
class TrackLoader
{
public:

    //! Constructor.
    TrackLoader();

    //! Destructor.
    ~TrackLoader();

    //! Add path to search level files for.
    void addTrackSearchPath(QString path);

    void loadAssets();

    /*! Load all tracks found in the added paths.
     *  Lock/unlock tracks according to the given lap count.
     *  \return Number of track loaded. */
    int loadTracks(int lapCount, DifficultyProfile::Difficulty difficulty);

    //! Update locked tracks when lap count changes.
    void updateLockedTracks(int lapCount, DifficultyProfile::Difficulty difficulty);

    //! Get track count.
    unsigned int tracks() const;

    //! Get a track of given index.
    Track * track(unsigned int index) const;

    static TrackLoader & instance();

private:

    //! Load the given track.
    //! \return Valid data pointer or nullptr if fails.
    TrackData * loadTrack(QString path);

    void sortTracks();

    //! Read a tile element.
    void readTile(QDomElement & element, TrackData & newData);

    //! Read an object element.
    void readObject(QDomElement & element, TrackData & newData);

    //! Read a target node element and push to the given vector.
    void readTargetNode(QDomElement & element, TrackData & newData, std::vector<TargetNodePtr> & route);

    //! Convert tile type string to a type enum.
    TrackTile::TileType tileTypeEnumFromString(std::string str);

    MCAssetManager m_assetManager;

    MCObjectFactory m_objectFactory;

    TrackObjectFactory m_trackObjectFactory;

    std::vector<QString> m_paths;

    std::vector<Track *> m_tracks;

    static TrackLoader * m_instance;
};

#endif // TRACKLOADER_HPP
