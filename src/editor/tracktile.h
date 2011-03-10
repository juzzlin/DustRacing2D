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

class TrackTile : public QGraphicsItem
{
public:

    //! All possible types of tiles.
    enum TileType {TT_NONE, TT_CORNER_GRASS, TT_STRAIGHT_GRASS, TT_GRASS};

    //! Constructor
    TrackTile(QSizeF size, QPointF location, TileType type = TT_NONE);

    //! Destructor
    ~TrackTile();

    //! \reimp
    virtual QRectF boundingRect () const;

    //! \reimp
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

    //! Set tile active (a blue collar is drawn)
    void setActive(bool active);

    //! Set type
    void setType(TileType type);

    //! Get current active tile or NULL if not set.
    static TrackTile * activeTile();

protected:

    //! \reimp
    void mousePressEvent(QGraphicsSceneMouseEvent * event);

    //! \reimp
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

private slots:

    void rotate90CW();
    void rotate90CCW();

private:

    void createContextMenu();

    static TrackTile * m_activeTile;
    QSizeF             m_size;
    TileType           m_type;
    bool               m_active;
    TileAnimator     * m_animator;
    QMenu              m_menu;
};

#endif // TRACKTILE_H
