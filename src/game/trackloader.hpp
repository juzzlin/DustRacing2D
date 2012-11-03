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

#include "tracktile.hpp"

class TargetNodeBase;
class Track;
class TrackData;
class TrackTileBase;
class MCSurfaceManager;
class MCObjectFactory;
class QDomElement;

//! A singleton class that handles track loading.
//! TODO: This can be shared with the editor or inherit from
//! a shared loader.
class TrackLoader
{
public:

    //! Constructor.
    //! \param textureManager Texture manager to be used when determining
    //! tile textures.
    //! \param objectFactory  Object factory that creates objects other
    //! than tiles.
    TrackLoader(MCSurfaceManager & textureManager, MCObjectFactory  & objectFactory);

    //! Destructor.
    ~TrackLoader();

    //! Add path to search level files for.
    void addTrackSearchPath(QString path);

    //! Load all tracks found in the added paths.
    //! \return Number of track loaded.
    int loadTracks();

    //! Get track count.
    unsigned int tracks() const;

    //! Get a track of given index.
    Track * track(unsigned int index) const;

    static TrackLoader & instance();

private:

    //! Load the given track.
    //! \return Valid data pointer or nullptr if fails.
    TrackData * loadTrack(QString path);

    //! Read a tile element.
    void readTile(QDomElement & element, TrackData & newData);

    //! Read an object element.
    void readObject(QDomElement & element, TrackData & newData);

    //! Read a target node element and push to the given vector.
    void readTargetNode(
        QDomElement & element, TrackData & newData, std::vector<TargetNodeBase *> & route);

    //! Convert tile type string to a type enum.
    TrackTile::TileType tileTypeEnumFromString(std::string str);

    MCSurfaceManager   & m_textureManager;
    MCObjectFactory    & m_objectFactory;
    std::vector<QString> m_paths;
    std::vector<Track *> m_tracks;

    static TrackLoader * m_instance;
};

#endif // TRACKLOADER_HPP
