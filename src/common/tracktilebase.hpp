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

#ifndef TRACKTILEBASE_HPP
#define TRACKTILEBASE_HPP

#include <QPoint>
#include <QPointF>
#include <QString>

#include <memory>

/*! Base class for track tiles used in the editor and in the game.
 *
 *  This is not the most beautiful solution to share tile code between
 *  the editor and the game because it requires us to downcast objects as
 *  both have some incompatible additions to the base class.
 *
 *  However, we can avoid dynamic_cast<>'s because we "know" what the
 *  objects really are.
 */
class TrackTileBase
{
public:
    //! Hints for computer players.
    enum class ComputerHint
    {
        None = 0,
        BrakeHard,
        Brake
    };

    /*! Constructor.
     *  \param location Location (coordinates) in the track scene.
     *  \param matrixLocation Location in the tile matrix.
     *  \param type Type of the tile. See setType(). */
    TrackTileBase(QPointF location, QPoint matrixLocation, const QString & type = "clear");

    TrackTileBase(TrackTileBase & other) = delete;

    TrackTileBase & operator=(TrackTileBase & other) = delete;

    //! Destructor.
    virtual ~TrackTileBase();

    //! Get location in world / scene.
    virtual QPointF location() const;

    virtual void setLocation(QPointF location);

    //! Get location in the tile matrix.
    virtual QPoint matrixLocation() const;

    virtual void setMatrixLocation(QPoint matrixLocation);

    //! Set type: "corner", "straight", "grass", "finish", "clear"..
    virtual void setTileType(const QString & type);

    //! Get type
    const QString & tileType() const;

    //! \return Tile width in pixels
    static size_t width();

    //! \return Tile height in pixels
    static size_t height();

    //! Set computer hint
    virtual void setComputerHint(ComputerHint hint);

    //! Get computer hint
    ComputerHint computerHint() const;

    /*! Don't show the tile in minimap / preview even if an asphalt tile.
     *  (Some tracks might include asphalt tiles just for looks) */
    void setExcludeFromMinimap(bool exclude);

    bool excludeFromMinimap() const;

private:
    //! Type string.
    QString m_tileType;

    //! Location in the world / scene.
    QPointF m_location;

    //! Location in the tile matrix.
    QPoint m_matrixLocation;

    //! Computer hint
    ComputerHint m_computerHint;

    bool m_excludeFromMinimap;
};

using TrackTileBasePtr = std::shared_ptr<TrackTileBase>;

#endif // TRACKTILEBASE_HPP
