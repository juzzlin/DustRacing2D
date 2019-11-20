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

#ifndef TRACKDATA_HPP
#define TRACKDATA_HPP

#include <QString>

#include "../common/mapbase.hpp"
#include "../common/objects.hpp"
#include "../common/route.hpp"
#include "../common/trackdatabase.hpp"

#include "map.hpp"

class ObjectModelLoader;

class TrackData : public TrackDataBase
{
public:
    //! Constructor.
    TrackData(QString name, bool isUserTrack, size_t cols, size_t rows);

    //! Copy constructor
    TrackData(const TrackData & other);

    //! Destructor.
    virtual ~TrackData();

    //! Get file name.
    QString fileName() const;

    //! Set file name.
    void setFileName(QString fileName);

    //! Get map object.
    MapBase & map();

    //! Get map object.
    const MapBase & map() const;

    //! Get route object.
    Route & route();

    //! Get route object.
    const Route & route() const;

    //! Get objects object.
    Objects & objects();

    //! Get objects object.
    const Objects & objects() const;

    //! Insert tile column at given index and move objects.
    void insertColumn(size_t at, MapBase::InsertDirection insertDirection);

    //! Insert tile row at given index and move objects.
    void insertRow(size_t at, MapBase::InsertDirection insertDirection);

    //! Delete tile column at given index and move objects.
    std::vector<TrackTileBasePtr> deleteColumn(size_t at);

    //! Delete tile row at given index and move objects.
    std::vector<TrackTileBasePtr> deleteRow(size_t at);

private:
    void copyObjects(const TrackData & other);

    void copyRoute(const TrackData & other);

    void moveObjectsAfterColumnInsertion(size_t at);

    void moveObjectsAfterRowInsertion(size_t at);

    void moveTargetNodesAfterColumnInsertion(size_t at);

    void moveTargetNodesAfterRowInsertion(size_t at);

    void moveObjectsAfterColumnDeletion(size_t at);

    void moveObjectsAfterRowDeletion(size_t at);

    void moveTargetNodesAfterColumnDeletion(size_t at);

    void moveTargetNodesAfterRowDeletion(size_t at);

    QString m_fileName;

    Map m_map;

    Objects m_objects;

    Route m_route;
};

typedef std::shared_ptr<TrackData> TrackDataPtr;

#endif // TRACKDATA_HPP
