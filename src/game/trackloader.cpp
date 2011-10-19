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

#include "track.h"
#include "trackloader.h"
#include "tracktile.h"
#include "../common/trackdata.h"

#include "MiniCore/Core/MCLogger"
#include "MiniCore/Core/MCTextureManager"

TrackLoader::TrackLoader(MCTextureManager * textureManager)
: m_textureManager(textureManager)
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
    Q_FOREACH(QString path, m_paths)
    {
        MCLogger::logInfo("Loading tracks from '%s'..", path.toStdString().c_str());
        QStringList trackPaths(QDir(path).entryList(QStringList("*.trk")));
        Q_FOREACH(QString trackPath, trackPaths)
        {
            MCLogger::logInfo("Found '%s'..", trackPath.toStdString().c_str());
            trackPath = path + QDir::separator() + trackPath;
            if (TrackData * trackData = loadTrack(trackPath))
            {
                m_tracks << new Track(trackData);
                numLoaded++;
            }
            else
            {
                MCLogger::logError("Couldn't load '%s'..",
                                   trackPath.toStdString().c_str());
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

    QDomElement  root = doc.documentElement();
    QString      name = root.attribute("name", "undefined");
    unsigned int cols = root.attribute("cols", "0").toUInt();
    unsigned int rows = root.attribute("rows", "0").toUInt();

    TrackData * newData = nullptr;
    if (cols > 0 && rows > 0)
    {
        newData = new TrackData(name, cols, rows);
        newData->setFileName(path);

        QVector<TrackTile *> routeVector;

        QDomNode node = root.firstChild();
        while(!node.isNull())
        {
            QDomElement tag = node.toElement();
            if(!tag.isNull())
            {
                // Read a tile tag
                if (tag.nodeName() == "tile")
                {
                    QString      id = tag.attribute("type", "clear");
                    unsigned int i  = tag.attribute("i", "0").toUInt();
                    unsigned int j  = tag.attribute("j", "0").toUInt();
                    int          o  = tag.attribute("o", "0").toInt();
                    int      index  = tag.attribute("index", "-1").toInt();

                    if (TrackTile * tile = newData->map().getTile(i, j))
                    {
                        tile->setRotation(o);
                        tile->setTileType(id);
                        tile->setRouteIndex(index);

                        // Associate with a surface object corresponging
                        // to the tile type.
                        // surface() throws if fails. Handled of higher level.
                        tile->setSurface(m_textureManager->surface(id));

                        if (index >= 0)
                            routeVector << tile;
                    }
                }
            }

            node = node.nextSibling();
        }

        newData->route().buildFromVector(routeVector);
    }

    return newData;
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
    Q_FOREACH(Track * track, m_tracks)
    {
        delete track;
    }

    m_tracks.clear();
}
