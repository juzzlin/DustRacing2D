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

#ifndef TRACKTILE_HPP
#define TRACKTILE_HPP

#include <QGraphicsItem>
#include <QMenu>
#include <QString>

#include "../common/tracktilebase.hpp"

class QGraphicsLineItem;
class TileAnimator;

/*! A race track is built of TrackTiles. The TrackTile class
 *  extends TrackTileBase with features needed to render
 *  track tiles in editor's scene. Note that the track tile instances
 *  do not change. Only their types change when user sets a tile. */
class TrackTile : public QGraphicsItem, public TrackTileBase
{
public:
    /*! Constructor.
     *  \param location Location (coordinates) in the track scene.
     *  \param matrixLocation Location in the tile matrix.
     *  \param type Type of the tile. See setType(). */
    TrackTile(QPointF location, QPoint matrixLocation,
              const QString & type = "clear");

    /*! Copy constructor.
     *  NOTE!!: This is used when saving undo points so make sure all properties are copied! */
    TrackTile(const TrackTile & other);

    TrackTile & operator=(const TrackTile & other) = delete;

    //! Destructor
    virtual ~TrackTile();

    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter * painter,
                       const QStyleOptionGraphicsItem * option, QWidget * widget = 0) override;

    //! Set tile active (a blue collar is drawn)
    void setActive(bool active);

    virtual void setTileType(const QString & type) override;

    virtual void setComputerHint(ComputerHint hint) override;

    virtual void setLocation(QPointF location) override;

    /*! Get current active tile.
     * \returns Valid pointer or nullptr if not set. */
    static TrackTile * activeTile();

    //! Set the active tile
    static void setActiveTile(TrackTile * tile);

    //! Rotate 90 degrees CW
    bool rotate90CW();

    //! Rotate 90 degrees CCW
    bool rotate90CCW();

    //! Get the current pixmap
    QPixmap pixmap() const;

    //! Set the current pixmap
    void setPixmap(const QPixmap & pixmap);

    //! Swap data with given tile. Used in drag'n'drop.
    void swap(TrackTile & other);

    void setAdded(bool state);

    bool added() const;

private:
    //! Create the menu that is shown when right-clicking on the tile.
    void createContextMenu();

    //! Pointer to the active/selected tile.
    static TrackTile * m_activeTile;

    //! Original size of the tile in pixels.
    QSizeF m_size;

    //! True if the tile is the seleced tile.
    bool m_active;

    //! Animator used when rotating the tile.
    TileAnimator * m_animator;

    //! Image of the tile.
    QPixmap m_pixmap;

    bool m_added;
};

using TrackTilePtr = std::shared_ptr<TrackTile>;

#endif // TRACKTILE_HPP
