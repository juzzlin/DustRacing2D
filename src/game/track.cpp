// This file is part of Dust Racing (DustRAC).
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

#include "../common/map.h"
#include "../common/trackdata.h"
#include "track.h"
#include "tracktile.h"
#include "MiniCore/Core/MCSurface"

Track::Track(TrackData * trackData)
: m_trackData(trackData)
{
}

void Track::render(MCCamera * camera)
{
    Map & map = m_trackData->map();
    const unsigned int cols = map.cols();
    const unsigned int rows = map.rows();

    for (unsigned int j = 0; j < rows; j++)
    {
        for (unsigned int i = 0; i < rows; i++)
        {
            TrackTile * tile = map.getTile(i, j);
            if (tile)
            {
                MCSurface * surface = tile->surface();
                if (surface)
                {
                    // TODO
                }
            }
        }
    }
}

Track::~Track()
{
    delete m_trackData;
}
