// This file belongs to the "Dust" car racing game
// Copyright (C) 2010 Jussi Lind <jussi.lind@iki.fi>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//

#include "trackdata.h"
#include "tracktile.h"

TrackData::TrackData(QString name, unsigned int horSize, unsigned int verSize) :
    m_name(name),
    m_horSize(horSize),
    m_verSize(verSize)
{
    // Create an empty map
    for (unsigned int j = 0; j < m_verSize; j++)
    {
        QVector<TrackTile *> row;
        for (unsigned int i = 0; i < m_horSize; i++)
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

unsigned int TrackData::horSize() const
{
    return m_horSize;
}

unsigned int TrackData::verSize() const
{
    return m_verSize;
}

TrackData::~TrackData()
{
    // Delete map and tiles
    for (unsigned int j = 0; j < m_verSize; j++)
    {
        for (unsigned int i = 0; i < m_horSize; i++)
        {
            delete m_map[j][i];
        }
    }

    m_map.clear();
}
