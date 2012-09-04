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

#ifndef TRACKDATABASE_HPP
#define TRACKDATABASE_HPP

#include <QString>

class Objects;
class Route;
class MapBase;

class TrackDataBase
{
public:

    //! Constructor.
    TrackDataBase();

    //! Destructor.
    virtual ~TrackDataBase();

    //! Get name.
    virtual QString name() const = 0;

    //! Get file name.
    virtual QString fileName() const = 0;

    //! Set file name.
    virtual void setFileName(QString fileName) = 0;

    //! Get lap count.
    virtual unsigned int lapCount() const;

    //! Set lap count.
    virtual void setLapCount(unsigned int laps);

    //! Get lap index.
    virtual unsigned int index() const;

    //! Set track index.
    virtual void setIndex(unsigned int index);

    //! Get map object.
    virtual MapBase & map() = 0;

    //! Get map object.
    virtual const MapBase & map() const = 0;

    //! Get route object.
    virtual Route & route() = 0;

    //! Get route object.
    virtual const Route & route() const = 0;

    //! Get objects object.
    virtual Objects & objects() = 0;

    //! Get objects object.
    virtual const Objects & objects() const = 0;

private:

    unsigned int m_lapCount;
    unsigned int m_index;
};

#endif // TRACKDATABASE_HPP
