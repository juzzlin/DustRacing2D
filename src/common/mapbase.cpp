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

#include "mapbase.hpp"

#include "tracktilebase.hpp"

#include <QPoint>
#include <QPointF>

MapBase::MapBase(size_t cols, size_t rows)
  : m_cols(cols)
  , m_rows(rows)
  , m_map(rows, TrackTileRow(m_cols, nullptr))
{
}

size_t MapBase::cols() const
{
    return m_cols;
}

size_t MapBase::rows() const
{
    return m_rows;
}

void MapBase::resize(size_t newCols, size_t newRows)
{
    for (size_t row = 0; row < m_map.size(); row++)
    {
        if (newCols > m_map[row].size())
        {
            m_map[row].resize(newCols, nullptr);
        }
    }

    if (newRows > m_rows)
    {
        m_map.resize(newRows, TrackTileRow(newCols, nullptr));
    }

    m_cols = newCols;
    m_rows = newRows;
}

bool MapBase::setTile(size_t x, size_t y, TrackTileBasePtr tile)
{
    if (x >= m_cols || y >= m_rows)
        return false;

    m_map[y][x] = tile;

    return true;
}

TrackTileBasePtr MapBase::getTile(size_t x, size_t y) const
{
    if (x >= m_cols || y >= m_rows)
        return nullptr;

    return m_map[y][x];
}

size_t MapBase::insertColumn(size_t at, MapBase::InsertDirection insertDirection)
{
    at = at + (insertDirection == MapBase::InsertDirection::Before ? 0 : 1);

    for (auto & row : m_map)
    {
        row.insert(row.begin() + at, nullptr);
    }

    m_cols++;

    return at;
}

std::vector<TrackTileBasePtr> MapBase::deleteColumn(size_t at)
{
    std::vector<TrackTileBasePtr> deleted;

    for (auto & row : m_map)
    {
        deleted.push_back(*(row.begin() + at));
        row.erase(row.begin() + at);
    }

    m_cols--;

    return deleted;
}

size_t MapBase::insertRow(size_t at, MapBase::InsertDirection insertDirection)
{
    at = at + (insertDirection == MapBase::InsertDirection::Before ? 0 : 1);

    m_map.insert(m_map.begin() + at, TrackTileRow(m_cols, nullptr));

    m_rows++;

    return at;
}

std::vector<TrackTileBasePtr> MapBase::deleteRow(size_t at)
{
    std::vector<TrackTileBasePtr> deleted = *(m_map.begin() + at);
    m_map.erase(m_map.begin() + at);

    m_rows--;

    return deleted;
}

MapBase::~MapBase()
{
}
