// This file is part of Dust Rallycross (DustRAC).
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

#ifndef TRACKTILE_H
#define TRACKTILE_H

#include <QGraphicsItem>
#include <QMenu>

class TileAnimator;
class TrackData;

//! A race track is built of TrackTiles.
class TrackTile : public QGraphicsItem
{
public:

    enum RouteDirection {RD_NONE = 0, RD_LEFT, RD_RIGHT, RD_UP, RD_DOWN};

    //! Tile width in pixels
    static const unsigned int TILE_W = 256;

    //! Tile height in pixels
    static const unsigned int TILE_H = 256;

    //! Constructor
    TrackTile(TrackData * trackData, QPointF location, QPoint matrixLocation,
              const QString & type = "clear");

    //! Destructor
    ~TrackTile();

    //! \reimp
    virtual QRectF boundingRect () const;

    //! \reimp
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

    //! Set tile active (a blue collar is drawn)
    void setActive(bool active);

    //! Set type: "corner", "straight", "grass", "finish", "clear".
    void setTileType(const QString & type);

    //! Get type
    const QString & tileType() const;

    //! Set route index
    void setRouteIndex(int index);

    //! Get route index, return -1 if not set.
    int routeIndex() const;

    //! Set direction towards next tile in the route.
    void setRouteDirection(TrackTile::RouteDirection direction);

    //! Get location in the tile matrix.
    QPoint matrixLocation() const;

    //! Get current active tile or NULL if not set.
    static TrackTile * activeTile();

    //! Set the active tile
    static void setActiveTile(TrackTile * tile);

    //! Rotate 90 degrees CW
    void rotate90CW();

    //! Rotate 90 degrees CCW
    void rotate90CCW();

private:

    void createContextMenu();

    TrackData        * m_trackData;
    static TrackTile * m_activeTile;
    QSizeF             m_size;
    QString            m_tileType;
    bool               m_active;
    TileAnimator     * m_animator;
    int                m_routeIndex;
    QPoint             m_matrixLocation;
    RouteDirection     m_routeDirection;
};

#endif // TRACKTILE_H
