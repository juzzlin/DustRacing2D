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

#ifndef TRACKLOADER_H
#define TRACKLOADER_H

#include <QString>
#include <QVector>

class Track;
class TrackData;
class MCTextureManager;

//! Manages the track loading procedure.
//! TODO: This can be shared with the editor or inherit from
//! a shared loader.
class TrackLoader
{
public:

    //! Constructor.
    //! \param textureManager Texture manager to be used when determining
    //!                       tile textures.
    TrackLoader(MCTextureManager * textureManager);

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

private:

    //! Load the given track.
    //! \return Valid data pointer or nullptr if fails.
    TrackData * loadTrack(QString path);

    MCTextureManager * m_textureManager;
    QVector<QString>   m_paths;
    QVector<Track *>   m_tracks;
};

#endif // TRACKLOADER_H
