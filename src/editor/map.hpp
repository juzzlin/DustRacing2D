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

#ifndef MAP_HPP
#define MAP_HPP

#include "../common/mapbase.hpp"

//! Base class for the tile matrix used by TrackData.
class Map : public MapBase
{
public:
    //! Constuctor.
    Map(size_t cols, size_t rows);

    //! Copy constructor.
    Map(const Map & other);

    void resize(size_t newCols, size_t newRows) override;

    size_t insertColumn(size_t at, InsertDirection insertDirection) override;

    std::vector<TrackTileBasePtr> deleteColumn(size_t at) override;

    size_t insertRow(size_t at, InsertDirection insertDirection) override;

    std::vector<TrackTileBasePtr> deleteRow(size_t at) override;

    //! Destructor.
    virtual ~Map() override;

private:
    void moveTilesAfterColumnDeletion(size_t at);

    void moveTilesAfterRowDeletion(size_t at);

    void copyTiles(const Map & other);

    void createEmptyTiles();
};

#endif // MAP_HPP
