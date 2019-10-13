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

#ifndef TRACKTILE_HPP
#define TRACKTILE_HPP

#include "../common/tracktilebase.hpp"
#include <MCSurface>

//! The track tile used in the game.
class TrackTile : public TrackTileBase
{
public:
    //! All possible types.
    enum TileType
    {
        TT_NONE = 0,
        TT_BRIDGE,
        TT_CORNER_90,
        TT_CORNER_45_LEFT,
        TT_CORNER_45_RIGHT,
        TT_STRAIGHT,
        TT_STRAIGHT_45_MALE,
        TT_STRAIGHT_45_FEMALE,
        TT_GRASS,
        TT_SAND,
        TT_SAND_GRASS_STRAIGHT,
        TT_SAND_GRASS_CORNER,
        TT_SAND_GRASS_CORNER_2,
        TT_FINISH
    };

    /*! Constructor.
     *  \see TrackTileBase. */
    TrackTile(QPointF location, QPoint matrixLocation,
              const QString & type = "", TileType typeEnum = TT_NONE);

    //! Destructor
    virtual ~TrackTile();

    //! Set the orientation in XY-plane in degrees when loading
    //! a track.
    void setRotation(int rotation);

    //! Set the renderable surface object corresponding
    //! to the track tile. TrackTile won't take the
    //! ownership, because many TrackTile's might use
    //! the same surface object.
    void setSurface(MCSurface * surface);

    //! Get the corresponding surface.
    MCSurface * surface() const;

    //! Set the renderable surface object corresponding
    //! to the preview of track tile. TrackTile won't take the
    //! ownership, because many TrackTile's might use
    //! the same surface object.
    void setPreviewSurface(MCSurface * surface);

    //! Get the corresponding preview surface.
    MCSurface * previewSurface() const;

    //! Get the orientation in XY-plane in degrees.
    int rotation() const;

    //! Get the type as an enum (performance optimization).
    TileType tileTypeEnum() const;

    //! Set the type as an enum (performance optimization).
    void setTileTypeEnum(TileType type);

    //! Returns true if the tile needs a separate asphalt background.
    bool hasAsphalt() const;

private:
    //! Disable the string-based type getter. Use tileTypeEnum() instead.
    const QString & tileType() const;

    int m_rotation;

    MCSurface * m_surface;

    MCSurface * m_previewSurface;

    TileType m_typeEnum;

    bool m_hasAsphalt;
};

using TrackTilePtr = std::shared_ptr<TrackTile>;

#endif // TRACKTILE_HPP
