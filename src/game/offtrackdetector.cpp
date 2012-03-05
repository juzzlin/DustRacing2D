// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2012 Jussi Lind <jussi.lind@iki.fi>
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

#include "offtrackdetector.hpp"
#include "car.hpp"
#include "track.hpp"
#include "tracktile.hpp"

#include <cassert>

OffTrackDetector::OffTrackDetector(Car & car)
: m_car(car)
, m_tileWLimit(TrackTile::TILE_W / 2 - TrackTile::TILE_W / 10)
, m_tileHLimit(TrackTile::TILE_H / 2 - TrackTile::TILE_H / 10)
{
}

void OffTrackDetector::setTrack(Track & track)
{
    m_pTrack = &track;
}

void OffTrackDetector::update()
{
    m_car.setOffTrack(false);

    assert(m_pTrack);
    TrackTile & rTile = *m_pTrack->trackTileAtLocation(
        m_car.location().i(), m_car.location().j());
    if (rTile.tileType() == "grass")
    {
        m_car.setOffTrack(true);
    }
    else if (rTile.tileType() == "straight" || rTile.tileType() == "finish")
    {
        if (rTile.rotation() == 0 || rTile.rotation() == 180)
        {
            MCFloat y = m_car.location().j();
            if (y > rTile.location().y() + m_tileWLimit ||
                y < rTile.location().y() - m_tileWLimit)
            {
                m_car.setOffTrack(true);
            }
        }
        else if (rTile.rotation() == 90 || rTile.rotation() == 270)
        {
            MCFloat x = m_car.location().i();
            if (x > rTile.location().x() + m_tileHLimit ||
                x < rTile.location().x() - m_tileHLimit)
            {
                m_car.setOffTrack(true);
            }
        }
    }
}
