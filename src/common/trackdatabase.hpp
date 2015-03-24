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

    struct IO
    {
        struct Header
        {
            static QString VER()   { return "version"; }
            static QString TRACK() { return "track"; }
            static QString NAME()  { return "name"; }
            static QString COLS()  { return "cols"; }
            static QString ROWS()  { return "rows"; }
            static QString INDEX() { return "index"; }
            static QString USER()  { return "isUserTrack"; }
        };

        struct Track
        {
            static QString NODE()   { return "n"; }
            static QString TILE()   { return "t"; }
            static QString OBJECT() { return "o"; }
        };

        struct Node
        {
            static QString INDEX()  { return "i"; }
            static QString X()      { return "x"; }
            static QString Y()      { return "y"; }
            static QString WIDTH()  { return "w"; }
            static QString HEIGHT() { return "h"; }
        };

        struct Tile
        {
            static QString TYPE()          { return "t"; }
            static QString I()             { return "i"; }
            static QString J()             { return "j"; }
            static QString ORIENTATION()   { return "o"; }
            static QString COMPUTER_HINT() { return "c"; }
        };

        struct Object
        {
            static QString ROLE()        { return "r"; }
            static QString CATEGORY()    { return "c"; }
            static QString X()           { return "x"; }
            static QString Y()           { return "y"; }
            static QString ORIENTATION() { return "o"; }
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
