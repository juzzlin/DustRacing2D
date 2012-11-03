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

#ifndef TRACKIO_HPP
#define TRACKIO_HPP

#include <QString>
#include <vector>

class TargetNodeBase;
class TrackData;
class QDomDocument;
class QDomElement;

class TrackIO
{
public:
    //! Save given track data. Returns false if failed.
    static bool save(const TrackData * trackData, QString path);

    //! Load given track data. Returns the new TrackData object,
    //! or nullptr if failed.
    static TrackData * open(QString path);

private:

    static void readTile(TrackData & newData, const QDomElement & element);
    static void readObject(TrackData & newData, const QDomElement & element);
    static void readTargetNode(std::vector<TargetNodeBase *> & route, const QDomElement & element);
    static void writeTiles(
        const TrackData & newData, QDomElement & element, QDomDocument & doc);
    static void writeObjects(
        const TrackData & newData, QDomElement & element, QDomDocument & doc);
    static void writeTargetNodes(
        const TrackData & newData, QDomElement & element, QDomDocument & doc);
};

#endif // TRACKIO_HPP
