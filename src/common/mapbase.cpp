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
#include "trackdatabase.hpp"

#include <QPoint>
#include <QPointF>

MapBase::MapBase(TrackDataBase & trackData, unsigned int cols, unsigned int rows)
    : m_trackData(trackData)
    , m_cols(cols)
    , m_rows(rows)
    , m_map(rows, TrackTileRow(m_cols, nullptr))
{}

unsigned int MapBase::cols() const
{
    return m_cols;
}

unsigned int MapBase::rows() const
{
    return m_rows;
}

void MapBase::resize(unsigned int newCols, unsigned int newRows)
{
    for (unsigned int row = 0; row < m_map.size(); row++)
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

bool MapBase::setTile(unsigned int x, unsigned int y, TrackTileBasePtr tile)
{
    if (x >= m_cols || y >= m_rows)
        return false;

    m_map[y][x] = tile;

    return true;
}

TrackTileBasePtr MapBase::getTile(unsigned int x, unsigned int y) const
{
    if (x >= m_cols || y >= m_rows)
        return nullptr;

    return m_map[y][x];
}

void MapBase::insertColumn(unsigned int at)
{
    for (auto & row : m_map)
    {
        row.insert(row.begin() + at, nullptr);
    }

    m_cols++;
}

std::vector<TrackTileBasePtr> MapBase::deleteColumn(unsigned int at)
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

void MapBase::insertRow(unsigned int at)
{
    m_map.insert(m_map.begin() + at, TrackTileRow(m_cols, nullptr));

    m_rows++;
}

std::vector<TrackTileBasePtr> MapBase::deleteRow(unsigned int at)
{
    std::vector<TrackTileBasePtr> deleted = *(m_map.begin() + at);
    m_map.erase(m_map.begin() + at);

    m_rows--;

    return deleted;
}

TrackDataBase & MapBase::trackData()
{
    return m_trackData;
}

MapBase::~MapBase()
{
}
