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

#ifndef TRACKDATABASE_HPP
#define TRACKDATABASE_HPP

#include <QString>

class Objects;
class Route;
class MapBase;

class TrackDataBase
{
public:

    /** Keywords used in the track data files.
     *  TODO: Use constexpr when MSVC supports it properly. */
    struct DataKeywords
    {
        struct Header
        {
            static QString ver()
            {
                return "version";
            }

            static QString track()
            {
                return "track";
            }

            static QString name()
            {
                return "name";
            }

            static QString cols()
            {
                return "cols";
            }

            static QString rows()
            {
                return "rows";
            }

            static QString index()
            {
                return "index";
            }

            static QString user()
            {
                return "isUserTrack";
            }
        };

        struct Track
        {
            static QString node()
            {
                return "n";
            }

            static QString tile()
            {
                return "t";
            }

            static QString object()
            {
                return "o";
            }
        };

        struct Node
        {
            static QString index()
            {
                return "i";
            }

            static QString x()
            {
                return "x";
            }

            static QString y()
            {
                return "y";
            }

            static QString width()
            {
                return "w";
            }

            static QString height()
            {
                return "h";
            }
        };

        struct Tile
        {
            static QString type()
            {
                return "t";
            }

            static QString i()
            {
                return "i";
            }

            static QString j()
            {
                return "j";
            }

            static QString orientation()
            {
                return "o";
            }

            static QString computerHint()
            {
                return "c";
            }
        };

        struct Object
        {
            static QString role()
            {
                return "r";
            }

            static QString category()
            {
                return "c";
            }

            static QString x()
            {
                return "x";
            }

            static QString y()
            {
                return "y";
            }

            static QString orientation()
            {
                return "o";
            }
        };
    };

    //! Constructor.
    TrackDataBase(QString name, bool isUserTrack);

    TrackDataBase(TrackDataBase & other) = delete;
    TrackDataBase & operator= (TrackDataBase & other) = delete;

    //! Destructor.
    virtual ~TrackDataBase();

    //! Get name.
    virtual QString name() const;

    //! Get file name.
    virtual QString fileName() const = 0;

    //! Set file name.
    virtual void setFileName(QString fileName) = 0;

    //! Get lap index.
    virtual unsigned int index() const;

    //! Set track index.
    virtual void setIndex(unsigned int index);

    //! Get if the track is user track (unlocked by default).
    virtual bool isUserTrack() const;

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

    QString m_name;

    unsigned int m_index;

    bool m_isUserTrack;
};

#endif // TRACKDATABASE_HPP
