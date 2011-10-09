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

#ifndef TRACKTILE_H
#define TRACKTILE_H

#include <QGraphicsItem>
#include <QString>
#include <QMenu>

#include "../common/tracktilebase.h"

class QGraphicsLineItem;
class TileAnimator;
class TrackData;

//! A race track is built of TrackTiles. The TrackTile class
//! extends TrackTileBase with features needed to visualize
//! track tiles in the editor.
class TrackTile : public QGraphicsItem, public TrackTileBase
{
public:

    //! Tile width in pixels
    static const unsigned int TILE_W = 256;

    //! Tile height in pixels
    static const unsigned int TILE_H = 256;

    //! Constructor.
    //! \param trackData Pointer to the "parent" TrackData.
    //! \param location Location (coordinates) in the track scene.
    //! \param matrixLocation Location in the tile matrix.
    //! \param type Type of the tile. See setType().
    TrackTile(TrackData * trackData, QPointF location, QPoint matrixLocation,
              const QString & type = "clear");

    //! Destructor
    virtual ~TrackTile();

    //! \reimp
    virtual QRectF boundingRect () const;

    //! \reimp
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

    //! Set tile active (a blue collar is drawn)
    void setActive(bool active);

    //! \reimp
    virtual void setRouteIndex(int index);

    //! \reimp
    virtual void setTileType(const QString & type);

    //! \reimp
    virtual void setRouteDirection(TrackTileBase::RouteDirection direction);

    //! Set a line item that is drawn from a tile to the previous tile.
    void setRouteLine(QGraphicsLineItem * routeLine);

    //! Get the route line.
    //! \returns Valid pointer or NULL if not set.
    QGraphicsLineItem * routeLine() const;

    //! Get current active tile.
    //! \returns Valid pointer or NULL if not set.
    static TrackTile * activeTile();

    //! Set the active tile
    static void setActiveTile(TrackTile * tile);

    //! Rotate 90 degrees CW
    void rotate90CW();

    //! Rotate 90 degrees CCW
    void rotate90CCW();

private:

    //! Create the menu that is shown when right-clicking on the tile.
    void createContextMenu();

    //! Pointer to the object containing the data of the track.
    TrackData * m_trackData;

    //! Pointer to the active/selected tile.
    static TrackTile * m_activeTile;

    //! Original size of the tile in pixels.
    QSizeF m_size;

    //! True if the tile is the seleced tile.
    bool m_active;

    //! Animator used when rotating the tile.
    TileAnimator * m_animator;

    //! Line that is drawn from a tile to the previous tile
    //! according to the route vector.
    QGraphicsLineItem * m_routeLine;
};

#endif // TRACKTILE_H
