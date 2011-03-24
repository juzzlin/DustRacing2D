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

#include "trackdata.h"
#include "tracktile.h"

TrackData::TrackData(QString name, unsigned int cols, unsigned int rows) :
    m_name(name),
    m_cols(cols),
    m_rows(rows),
    m_finishLineSet(false)
{
    // Create an empty map
    for (unsigned int j = 0; j < m_rows; j++)
    {
        QVector<TrackTile *> row;
        for (unsigned int i = 0; i < m_cols; i++)
        {
            row << NULL;
        }

        m_map << row;
    }

    // Create tiles
    for (unsigned int i = 0; i < m_cols; i++)
        for (unsigned int j = 0; j < m_rows; j++)
        {
            TrackTile * newTile = new TrackTile(QPointF(TrackTile::TILE_W / 2 + i * TrackTile::TILE_W,
                                                        TrackTile::TILE_H / 2 + j * TrackTile::TILE_H));
            setTile(i, j, newTile);
        }
}

QString TrackData::name() const
{
    return m_name;
}

QString TrackData::fileName() const
{
    return m_fileName;
}

void TrackData::setFileName(QString newFileName)
{
    m_fileName = newFileName;
}

unsigned int TrackData::cols() const
{
    return m_cols;
}

unsigned int TrackData::rows() const
{
    return m_rows;
}

bool TrackData::setFinishLine(unsigned int x0, unsigned int y0, unsigned int x1, unsigned int y1)
{
    if (x0 != x1 && y0 != y1)
        return false;

    if (x0 >= m_cols || y0 >= m_rows)
        return false;

    m_flx0 = x0;
    m_fly0 = y0;
    m_flx1 = x1;
    m_fly1 = y1;

    m_finishLineSet = true;

    return true;
}

void TrackData::finishLine(unsigned int & x0, unsigned int & y0, unsigned int & x1, unsigned int & y1) const
{
    x0 = m_flx0;
    y0 = m_fly0;
    x1 = m_flx1;
    y1 = m_fly1;
}

bool TrackData::finishLineSet() const
{
    return m_finishLineSet;
}

bool TrackData::setTile(unsigned int x, unsigned int y, TrackTile * pTile)
{
    if (x >= m_cols || y >= m_rows)
        return false;

    m_map[y][x] = pTile;

    return true;
}

TrackTile * TrackData::tile(unsigned int x, unsigned int y) const
{
    if (x >= m_cols || y >= m_rows)
        return NULL;

    return m_map[y][x];
}
