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

#ifndef TRACKTILEBASE_HPP
#define TRACKTILEBASE_HPP

#include <QPoint>
#include <QPointF>
#include <QString>

class TrackDataBase;

//! Base class for track tiles used in the editor and in the game.
class TrackTileBase
{
public:

    //! Direction to the next tile in the route.
    enum RouteDirection
    {
        RD_NONE = 0,
        RD_LEFT,
        RD_RIGHT,
        RD_UP,
        RD_DOWN,
        RD_UP_LEFT,
        RD_UP_RIGHT,
        RD_DOWN_LEFT,
        RD_DOWN_RIGHT
    };

    //! Hints for computer players.
    enum ComputerHint
    {
        CH_NONE = 0,
        CH_FIRST_BEFORE_CORNER,
        CH_SECOND_BEFORE_CORNER
    };

    //! Driving line hints for computer players.
    enum DrivingLineHint
    {
        DLH_NONE = 0,
        DLH_LEFT,
        DLH_RIGHT,
        DLH_TOP,
        DLH_BOTTOM
    };

    //! Constructor.
    //! \param trackData Reference to the "parent" TrackData.
    //! \param location Location (coordinates) in the track scene.
    //! \param matrixLocation Location in the tile matrix.
    //! \param type Type of the tile. See setType().
    TrackTileBase(TrackDataBase & trackData, QPointF location, QPoint matrixLocation,
        const QString & type = "clear");

    //! Destructor.
    virtual ~TrackTileBase();

    //! Return reference to the "parent" track data.
    TrackDataBase & trackData() const;

    //! Get location in world / scene.
    virtual QPointF location() const;

    //! Get location in the tile matrix.
    virtual QPoint matrixLocation() const;

    //! Set index in the route vector.
    virtual void setRouteIndex(int index);

    //! Get route index, return -1 if not set.
    int routeIndex() const;

    //! Set type: "corner", "straight", "grass", "finish", "clear"..
    void setTileType(const QString & type);

    //! Get type
    const QString & tileType() const;

    //! Set direction towards next tile in the route.
    void setRouteDirection(TrackTileBase::RouteDirection direction);

    //! Get direction towards next tile in the route.
    TrackTileBase::RouteDirection routeDirection() const;

    //! Set computer hint
    void setComputerHint(ComputerHint hint);

    //! Get computer hint
    ComputerHint computerHint() const;

    //! Set driving line hint
    void setDrivingLineHint(DrivingLineHint hint);

    //! Get driving line hint
    DrivingLineHint drivingLineHint() const;

private:

    //! Index in the route vector.
    int m_routeIndex;

    //! Type string.
    QString m_tileType;

    //! Location in the world / scene.
    QPointF m_location;

    //! Location in the tile matrix.
    QPoint m_matrixLocation;

    //! Pointer to the "parent" track data.
    TrackDataBase & m_trackData;

    //! Direction towards the next tile in the route.
    RouteDirection m_routeDirection;

    //! Computer hint
    ComputerHint m_computerHint;

    //! Driving line hint
    DrivingLineHint m_drivingLineHint;
};

#endif // TRACKTILEBASE_HPP
