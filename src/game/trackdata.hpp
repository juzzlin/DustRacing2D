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

#ifndef TRACKDATA_HPP
#define TRACKDATA_HPP

#include <QString>

#include "../common/mapbase.hpp"
#include "../common/objects.hpp"
#include "../common/route.hpp"
#include "../common/trackdatabase.hpp"

#include "map.hpp"

class TrackData : public TrackDataBase
{
public:
    //! Constructor.
    TrackData(QString name, bool isUserTrack, unsigned int cols, unsigned int rows);

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

    //! Return true if the track is locked.
    bool isLocked() const;

    //! Set the locked state.
    void setIsLocked(bool locked);

private:
    QString m_fileName;
    Map m_map;
    Objects m_objects;
    Route m_route;
    bool m_isLocked;
};

#endif // TRACKDATA_HPP
