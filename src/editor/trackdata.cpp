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
    m_rows(rows)
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

TrackData::~TrackData()
{
    // Delete map and tiles
    for (unsigned int j = 0; j < m_rows; j++)
    {
        for (unsigned int i = 0; i < m_cols; i++)
        {
            delete m_map[j][i];
        }
    }

    m_map.clear();
}
